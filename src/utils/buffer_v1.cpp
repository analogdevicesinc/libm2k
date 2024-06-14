/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "buffer_v1.hpp"
#include "channel_v1.hpp"
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/logger.hpp>
#include <libm2k/utils/utils.hpp>

#include <cstring>
#include <iostream>
#include <memory>
#include <algorithm>

using namespace std;
using namespace libm2k::utils;

Buffer::Buffer(struct iio_device *dev, struct iio_channels_mask *mask) {
	m_dev = dev;
	m_mask = mask;

	if (!m_dev) {
		m_dev = nullptr;
		THROW_M2K_EXCEPTION("Buffer: Device not found, so no buffer can be created", libm2k::EXC_INVALID_PARAMETER);
	}
	m_dev_name = iio_device_get_name(m_dev);
	m_buffer = nullptr;
	m_block = nullptr;
	m_block_enqueued = false;
	m_stream = nullptr;
	m_last_nb_samples = 0;
	m_nb_kernel_buffers = 4;
}

Buffer::~Buffer() {
	stop();
	destroy();
	destroyBuffer();
	m_data.clear();
	m_data_short.clear();
}

void Buffer::initializeBuffer(unsigned int size, bool cyclic, bool output, bool enableFlag)
{
	/* In non-cyclic mode pushing samples will fill the internal buffers, creating the possibility of continuous
		* data transferring; the buffer must be destroy when its size is changed
	*
	* In cyclic mode the very first buffer pushed will be repeated; in order to push any other buffer the
	* old buffer must be destroyed and a new one must be created*/
	if (!m_buffer) {
		if (m_mask) {
			m_buffer = iio_device_create_buffer(m_dev, 0, m_mask);
			int buf_err = iio_err(m_buffer);
			if (buf_err) {
				m_buffer = nullptr;
				if (buf_err == -ETIMEDOUT) {
					THROW_M2K_EXCEPTION("Buffer: Cannot create the buffer", libm2k::EXC_TIMEOUT, -ETIMEDOUT);
				}
				THROW_M2K_EXCEPTION("Buffer: Cannot create the buffer", libm2k::EXC_RUNTIME_ERROR, buf_err);
			}
		} else {
			LIBM2K_LOG(INFO, "[FAIL] Buffer NOT created, no mask provided.");
		}
	}
	if (size != m_last_nb_samples || cyclic) {
		if (enableFlag && cyclic) {
			for(auto channel : m_channel_list) {
				if (channel->hasAttribute("raw_enable")) {
					channel->setStringValue("raw_enable", "enabled");
				}
			}
		}
		destroy();

		m_last_nb_samples = size;
		if (cyclic && output) {
			ssize_t sample_size = iio_device_get_sample_size(m_dev, m_mask);
			m_block = iio_buffer_create_block(m_buffer, sample_size * m_last_nb_samples);
			int ret = iio_err(m_block);
			if (ret) {
				destroy();
				THROW_M2K_EXCEPTION("Buffer: Cannot create block", libm2k::EXC_RUNTIME_ERROR, ret);
				return;
			}
		} else {
			m_stream = iio_buffer_create_stream(m_buffer, m_nb_kernel_buffers, size);
			int ret = iio_err(m_stream);
			if (ret) {
				destroy();
				// timeout error code
				THROW_M2K_EXCEPTION("Buffer: Cannot create stream", libm2k::EXC_RUNTIME_ERROR, ret);
				return;
			}
			if (output) {
				m_block = (struct iio_block*)iio_stream_get_next_block(m_stream);
				ret = iio_err(m_block);
				if (ret) {
					destroy();
					THROW_M2K_EXCEPTION("Buffer: Cannot get next block", libm2k::EXC_RUNTIME_ERROR, ret);
					return;
				}
			}
		}

		if (enableFlag) {
			for(auto channel : m_channel_list) {
				if (channel->hasAttribute("raw_enable")) {
					channel->setStringValue("raw_enable", "disabled");
				}
			}
		}
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, std::string((output ? "TX" : "RX")) + " buffer created (" + std::to_string(size) +
							     " samples)"));
		LIBM2K_LOG_IF(WARNING, size % 4 != 0 || size < 16,
			      libm2k::buildLoggingMessage({m_dev_name}, "Incorrect number of samples"));

		LIBM2K_LOG_IF(WARNING, output && cyclic && size < 1024,
			      libm2k::buildLoggingMessage({m_dev_name}, "Cyclic buffer too small. The length of the buffer should be greater than 1024"));
	}
}

void Buffer::setChannels(std::vector<Channel*> channels)
{
	m_channel_list = channels;
}

void Buffer::push(unsigned short *data, unsigned int channel, unsigned int nb_samples,
	  bool cyclic, bool multiplex, bool enableFlag)
{
	pushGeneric((void*)data, sizeof *data, nb_samples, channel, cyclic,
		    multiplex, enableFlag);
}

//push on a certain channel
void Buffer::push(std::vector<short> const &data, unsigned int channel,
	  bool cyclic, bool multiplex, bool enableFlag)
{
	size_t size = data.size();

	pushGeneric((void*)data.data(), sizeof *(data.data()), size, channel, cyclic,
		    multiplex, enableFlag);
}

void Buffer::pushGeneric(void *data, ssize_t data_type_size, size_t data_size, unsigned int channel,
	  bool cyclic, bool multiplex, bool enableFlag)
{
	if (Utils::getIioDeviceDirection(m_dev) != OUTPUT) {
		THROW_M2K_EXCEPTION("Device not output buffer capable, so no buffer was created", libm2k::EXC_INVALID_PARAMETER);
	}

	/* If the data vector is empty, then it means we want
	 * to remove what was pushed earlier to the device, so
	 * we destroy the buffer */
	if (data_size == 0) {
		return;
	}
	initializeBuffer(data_size, cyclic, true, enableFlag);

	if (channel < m_channel_list.size() ) {
		if (!multiplex) {
			m_channel_list.at(channel)->write(m_block, (void*)data, data_type_size, data_size, false);
		} else {
			m_channel_list.at(channel)->write(m_block, (void*)data, data_type_size, data_size, true);
		}
		if (!cyclic) {
			m_block = (struct iio_block*)iio_stream_get_next_block(m_stream);
			int ret = iio_err(m_block);
			if (ret) {
				destroy();
				THROW_M2K_EXCEPTION("Buffer: Cannot get next block", libm2k::EXC_RUNTIME_ERROR, ret);
				return;
			}
		} else {
			int err = iio_block_enqueue(m_block, 0, cyclic);
			if (err) {
				THROW_M2K_EXCEPTION("Buffer: Unable to enqueue block.", libm2k::EXC_INVALID_PARAMETER, err);
			}
			m_block_enqueued = true;
			err = iio_buffer_enable(m_buffer);
			if (err) {
				THROW_M2K_EXCEPTION("Buffer: Unable to enable buffer.", libm2k::EXC_INVALID_PARAMETER, err);
			}
		}
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer pushed"));
	} else {
		THROW_M2K_EXCEPTION("Buffer: Please setup channels before pushing data", libm2k::EXC_INVALID_PARAMETER);

	}
}


void Buffer::push(std::vector<unsigned short> const &data, unsigned int channel,
	  bool cyclic, bool multiplex, bool enableFlag)
{
	size_t size = data.size();
	pushGeneric((void*)data.data(), sizeof *(data.data()), size, channel, cyclic,
		    multiplex, enableFlag);
}

void Buffer::push(std::vector<double> const &data, unsigned int channel, bool cyclic, bool enableFlag)
{
	size_t size = data.size();
	bool multiplex = false;

	pushGeneric((void*)data.data(), sizeof *(data.data()), size, channel, cyclic,
		    multiplex, enableFlag);
}

void Buffer::push(std::vector<std::vector<short>> const &data)
{
	size_t data_ch_nb = data.size();

	if (data_ch_nb > m_channel_list.size()) {
		THROW_M2K_EXCEPTION("Buffer: Please setup channels before pushing data", libm2k::EXC_OUT_OF_RANGE);
	}

	for (unsigned int i = 0; i < data_ch_nb; i++) {
		push(data.at(i), i);
	}
}

void Buffer::push(double *data, unsigned int channel, unsigned int nb_samples, bool cyclic, bool enableFlag)
{
	bool multiplex = false;
	pushGeneric((void*)data, sizeof *data, nb_samples, channel, cyclic,
		    multiplex, enableFlag);
}

void Buffer::push(short *data, unsigned int channel, unsigned int nb_samples, bool cyclic, bool enableFlag)
{
	bool multiplex = false;
	pushGeneric((void*)data, sizeof *data, nb_samples, channel, cyclic,
		    multiplex, enableFlag);
}

void Buffer::getSamples(std::vector<unsigned short> &data, unsigned int nb_samples)
{
	if (Utils::getIioDeviceDirection(m_dev) == OUTPUT) {
		THROW_M2K_EXCEPTION("Device not input-buffer capable, so no buffer was created", libm2k::EXC_RUNTIME_ERROR);
	}

	data.clear();

	initializeBuffer(nb_samples, false, false);

	const struct iio_block *block = iio_stream_get_next_block(m_stream);
	int ret = iio_err(block);
	if (ret) {
		destroy();
		THROW_M2K_EXCEPTION("Buffer: Cannot get next block", libm2k::EXC_RUNTIME_ERROR, ret);
		return;
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer refilled"));

	unsigned short* d_ptr = (unsigned short*)iio_block_start(block);
	for (unsigned int i = 0; i < nb_samples; i++) {
		data.push_back(d_ptr[i]);
	}
}

void Buffer::setKernelBuffersCount(unsigned int count)
{
	m_nb_kernel_buffers = count;
}

unsigned int Buffer::getKernelBuffersCount()
{
	return m_nb_kernel_buffers;
}

std::vector<unsigned short> Buffer::getSamples(unsigned int nb_samples)
{
	m_data_short.clear();
	getSamples(m_data_short, nb_samples);
	return m_data_short;
}

const unsigned short* Buffer::getSamplesP(unsigned int nb_samples)
{
	if (Utils::getIioDeviceDirection(m_dev) == OUTPUT) {
		THROW_M2K_EXCEPTION("Device not input-buffer capable, so no buffer was created", libm2k::EXC_INVALID_PARAMETER);
		return nullptr;
	}

	initializeBuffer(nb_samples, false, false);

	const struct iio_block *block = iio_stream_get_next_block(m_stream);
	int ret = iio_err(block);
	if (ret) {
		destroy();
		THROW_M2K_EXCEPTION("Buffer: Cannot get next block", libm2k::EXC_RUNTIME_ERROR, ret);
		return nullptr;
	}

	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer refilled"));

	const unsigned short* data = (const unsigned short*)iio_block_start(block);
	return data;
}


void Buffer::getSamples(std::vector<std::vector<double>> &data, unsigned int nb_samples,
				const std::function<double(int16_t, unsigned int)> &process)
{
	short* data_p = (short *) getSamplesRawInterleaved(nb_samples);

	std::vector<bool> channels_enabled;
	data.clear();

	for (auto chn : m_channel_list) {
		bool en  = chn->isEnabled(m_mask);
		channels_enabled.push_back(en);
		std::vector<double> ch_data {};
		ch_data.reserve(nb_samples);
		data.push_back(ch_data);
	}

	unsigned int i;
	unsigned int nb_channels = m_channel_list.size();


	for (i = 0; i < nb_samples; i++) {
		for (unsigned int ch = 0; ch < nb_channels; ch++) {
			if (channels_enabled[ch]) {
				data[ch].push_back(process(data_p[i * nb_channels + ch], ch));
			}
		}
	}
}

std::vector<std::vector<double>> Buffer::getSamples(unsigned int nb_samples,
				const std::function<double(int16_t, unsigned int)> &process)
{
	m_data.clear();
	getSamples(m_data, nb_samples, process);
	return m_data;
}

const short* Buffer::getSamplesRawInterleaved(unsigned int nb_samples)
{
	return static_cast<const short*>(getSamplesRawInterleavedVoid(nb_samples));
}

void* Buffer::getSamplesRawInterleavedVoid(unsigned int nb_samples)
{
	bool anyChannelEnabled = false;
	if (Utils::getIioDeviceDirection(m_dev) != INPUT) {
		THROW_M2K_EXCEPTION("Device not input-buffer capable, so no buffer was created", libm2k::EXC_INVALID_PARAMETER);
		return nullptr;
	}

	for (auto chn : m_channel_list) {
		bool en  = chn->isEnabled(m_mask);
		anyChannelEnabled = en ? true : anyChannelEnabled;
	}

	if (!anyChannelEnabled) {
		THROW_M2K_EXCEPTION("Buffer: No channel enabled for RX buffer", libm2k::EXC_INVALID_PARAMETER);
		return nullptr;
	}

	initializeBuffer(nb_samples, false, false);

	if (!m_stream) {
		return nullptr;
	}
	const struct iio_block *block = iio_stream_get_next_block(m_stream);
	int ret = iio_err(block);
	if (ret) {
		destroy();
		THROW_M2K_EXCEPTION("Buffer: Cannot get next block", libm2k::EXC_RUNTIME_ERROR, ret);
		return nullptr;
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer refilled"));

	return m_channel_list.at(0)->getFirstVoid(block);
}

const double* Buffer::getSamplesInterleaved(unsigned int nb_samples,
				    const std::function<double(int16_t, unsigned int)> &process)
{
	const short* data_p = getSamplesRawInterleaved(nb_samples);

	std::vector<bool> channels_enabled;

	for (auto chn : m_channel_list) {
		bool en  = chn->isEnabled(m_mask);
		channels_enabled.push_back(en);
	}

	unsigned int nb_channels = m_channel_list.size();
	double *data_p_d = new double[nb_samples * channels_enabled.size()];
	unsigned int i;
	unsigned int i_d = 0;

	for (i = 0; i < nb_samples; i++) {
		for (unsigned int ch = 0; ch < nb_channels; ch++) {
			if (channels_enabled.at(ch)) {
				data_p_d[i_d] = process(data_p[i * nb_channels + ch], ch);
				i_d++;
			}
		}
	}

	return (const double *)data_p_d;
}

void Buffer::stop()
{
	if (Utils::getIioDeviceDirection(m_dev) != OUTPUT) {
		return;
	}

	if (m_buffer) {
		// TBD watchout for cancelling buffers
		iio_buffer_cancel(m_buffer);
	}

	destroy();
	destroyBuffer();
}

void Buffer::destroyBuffer()
{
	if (m_buffer) {
		iio_buffer_destroy(m_buffer);
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer destroyed"));
		m_buffer = nullptr;
		m_last_nb_samples = 0;
	}
}


void Buffer::destroy()
{
	if (m_block && !m_stream) {
		if (m_block_enqueued) {
			iio_block_dequeue(m_block, false);
			m_block_enqueued = false;
		}
		iio_block_destroy(m_block);
		m_block = nullptr;
		iio_buffer_disable(m_buffer);
	}

	if (m_stream) {
		iio_stream_destroy(m_stream);
		m_block = nullptr;
	}

	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Stream destroyed"));
	m_stream = nullptr;
	m_last_nb_samples = 0;
}

void Buffer::cancelBuffer()
{
	if (m_buffer) {
		iio_buffer_cancel(m_buffer);
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer canceled"));
	}
}

void Buffer::flushBuffer()
{
	destroy();
	m_last_nb_samples = 0;
}

void Buffer::setCyclic(bool enable)
{
	m_cyclic = enable;
}

unsigned int Buffer::getNbSamples() const
{
	return m_last_nb_samples;
}

unsigned int Buffer::getNbAttributes()
{
	if (!m_buffer)
		return 0;
	return iio_buffer_get_attrs_count(m_buffer);
}

std::string Buffer::getAttributeName(unsigned int idx)
{
	const char *attr_name = "";
	const struct iio_attr* attr = iio_buffer_get_attr(m_buffer, idx);
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Buffer: invalid attribute requested.", libm2k::EXC_INVALID_PARAMETER);
	}
	attr_name = iio_attr_get_name(attr);
	return attr_name;
}

int Buffer::getLongValue(std::string attr_name)
{
	long long value = 0;
	const struct iio_attr* attr = iio_buffer_find_attr(m_buffer, attr_name.c_str());
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Buffer: invalid attribute requested: " + attr_name,
				    libm2k::EXC_INVALID_PARAMETER);
	}
	int ret = iio_attr_read_longlong(attr, &value);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Buffer: Cannot read " + attr_name, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name, attr_name.c_str(), LIBM2K_ATTRIBUTE_READ}, std::to_string(value)));
	return static_cast<int>(value);
}

void Buffer::setLongValue(int value, std::string attr_name)
{
	const struct iio_attr* attr = iio_buffer_find_attr(m_buffer, attr_name.c_str());
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Buffer: invalid attribute requested: " + attr_name,
				    libm2k::EXC_INVALID_PARAMETER);
	}
	int ret = iio_attr_write_longlong(attr, value);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Buffer: Cannot write " + attr_name, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name, attr_name.c_str(), LIBM2K_ATTRIBUTE_WRITE}, std::to_string(value)));
}

std::string Buffer::getStringValue(std::string attr_name)
{
	char value[1024];
	const struct iio_attr* attr = iio_buffer_find_attr(m_buffer, attr_name.c_str());
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Buffer: invalid attribute requested: " + attr_name,
				    libm2k::EXC_INVALID_PARAMETER);
	}
	int ret = iio_attr_read_raw(attr, value, sizeof(value));
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Buffer: Cannot read " + attr_name, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name, attr_name.c_str(), LIBM2K_ATTRIBUTE_READ}, std::string(value)));
	return std::string(value);
}

void Buffer::setStringValue(std::string attr_name, std::string value)
{
	const struct iio_attr* attr = iio_buffer_find_attr(m_buffer, attr_name.c_str());
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Buffer: invalid attribute requested: " + attr_name,
				    libm2k::EXC_INVALID_PARAMETER);
	}
	int ret = iio_attr_write_raw(attr, value.c_str(), value.size());
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Buffer: Cannot write " + attr_name, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name, attr_name.c_str(), LIBM2K_ATTRIBUTE_READ}, std::string(value)));
}

struct iio_buffer* Buffer::getBuffer()
{
	return m_buffer;
}

struct iio_block* Buffer::getBlock()
{
	return m_block;
}

bool Buffer::hasBufferAttribute(std::string attr_name)
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Buffer not enabled, can't search attribute.", libm2k::EXC_INVALID_PARAMETER);
	}
	const struct iio_attr* attr = iio_buffer_find_attr(m_buffer, attr_name.c_str());
	return !!attr;
	// TBD ?
}
