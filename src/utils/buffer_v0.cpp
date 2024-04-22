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

#include "buffer.hpp"
#include "channel.hpp"
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/logger.hpp>
#include <libm2k/utils/utils.hpp>

#include <iostream>
#include <memory>
#include <algorithm>

using namespace std;
using namespace libm2k::utils;

Buffer::Buffer(struct iio_device *dev) {
	m_dev = dev;

	if (!m_dev) {
		m_dev = nullptr;
		THROW_M2K_EXCEPTION("Buffer: Device not found, so no buffer can be created", libm2k::EXC_INVALID_PARAMETER);
	}
	m_dev_name = iio_device_get_name(m_dev);

	unsigned int dev_count = iio_device_get_buffer_attrs_count(m_dev);
	if (dev_count <= 0) {
		THROW_M2K_EXCEPTION("Buffer: Device " + std::string(m_dev_name) + " is not buffer capable, no buffer can be created", libm2k::EXC_INVALID_PARAMETER);
	}
	m_buffer = nullptr;
	m_last_nb_samples = 0;
}

Buffer::~Buffer() {
	stop();
	destroy();
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
		m_buffer = iio_device_create_buffer(m_dev, size, cyclic);
		if (!m_buffer) {
			if (output) {
				if (errno == ETIMEDOUT) {
					THROW_M2K_EXCEPTION("Buffer: Cannot create the TX buffer", libm2k::EXC_TIMEOUT, -ETIMEDOUT);
				}
				THROW_M2K_EXCEPTION("Buffer: Cannot create the TX buffer", libm2k::EXC_RUNTIME_ERROR, errno);
			} else {
				if (errno == ETIMEDOUT) {
					THROW_M2K_EXCEPTION("Buffer: Cannot create the RX buffer", libm2k::EXC_TIMEOUT, -ETIMEDOUT);
				}
				THROW_M2K_EXCEPTION("Buffer: Cannot create the RX buffer", libm2k::EXC_RUNTIME_ERROR, errno);
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
	//if (m_buffer) //this means the output is running, should we change the channels now?
	//{
	//throw exception
	//}
	m_channel_list = channels;
}

void Buffer::push(unsigned short *data, unsigned int channel, unsigned int nb_samples,
	  bool cyclic, bool multiplex, bool enableFlag)
{
	if (Utils::getIioDeviceDirection(m_dev) != OUTPUT) {
		THROW_M2K_EXCEPTION("Device not output buffer capable, so no buffer was created", libm2k::EXC_INVALID_PARAMETER);
	}

	/* If the data vector is empty, then it means we want
	 * to remove what was pushed earlier to the device, so
	 * we destroy the buffer */
	if (nb_samples == 0) {
		return;
	}

	initializeBuffer(nb_samples, cyclic, true, enableFlag);

	if (channel < m_channel_list.size() ) {
		if (!multiplex) {
			m_channel_list.at(channel)->write(m_buffer, data, nb_samples);
		} else {
			short *p_dat;
			int i = 0;

			for (p_dat = (short *)iio_buffer_start(m_buffer); (p_dat < iio_buffer_end(m_buffer));
			     (unsigned short*)p_dat++, i++) {
				*p_dat = data[i];
			}

		}
		ssize_t ret = iio_buffer_push(m_buffer);
		if (ret < 0) {
			destroy();
			// timeout error code
			if (ret == -ETIMEDOUT) {
				THROW_M2K_EXCEPTION("Buffer: Push timeout occurred", libm2k::EXC_TIMEOUT, ret);
			}
			THROW_M2K_EXCEPTION("Buffer: Cannot push TX buffer", libm2k::EXC_RUNTIME_ERROR, ret);
		}
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer pushed"));
	} else {
		THROW_M2K_EXCEPTION("Buffer: Please setup channels before pushing data", libm2k::EXC_INVALID_PARAMETER);
	}
}

//push on a certain channel
void Buffer::push(std::vector<short> const &data, unsigned int channel,
	  bool cyclic, bool multiplex, bool enableFlag)
{
    size_t size = data.size();
	if (Utils::getIioDeviceDirection(m_dev) != OUTPUT) {
		THROW_M2K_EXCEPTION("Device not output buffer capable, so no buffer was created", libm2k::EXC_INVALID_PARAMETER);
	}

	/* If the data vector is empty, then it means we want
	* to remove what was pushed earlier to the device, so
	* we destroy the buffer */
	if (data.size() == 0) {
		return;
	}

	initializeBuffer(size, cyclic, true, enableFlag);

	if (channel < m_channel_list.size() ) {
		if (!multiplex) {
			m_channel_list.at(channel)->write(m_buffer, data);
		} else {
			short *p_dat;
			int i = 0;

			for (p_dat = (short *)iio_buffer_start(m_buffer); (p_dat < iio_buffer_end(m_buffer));
			     (short*)p_dat++, i++) {
				*p_dat = data[i];
			}

		}
		ssize_t ret = iio_buffer_push(m_buffer);
		if (ret < 0) {
			destroy();
			// timeout error code
			if (ret == -ETIMEDOUT) {
				THROW_M2K_EXCEPTION("Buffer: Push timeout occurred", libm2k::EXC_TIMEOUT, ret);
			}
			THROW_M2K_EXCEPTION("Buffer: Cannot push TX buffer", libm2k::EXC_RUNTIME_ERROR, ret);
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
	if (Utils::getIioDeviceDirection(m_dev) != OUTPUT) {
		THROW_M2K_EXCEPTION("Device not output buffer capable, so no buffer was created", libm2k::EXC_INVALID_PARAMETER);
	}

	/* If the data vector is empty, then it means we want
	 * to remove what was pushed earlier to the device, so
	 * we destroy the buffer */
	if (data.size() == 0) {
		return;
	}

	initializeBuffer(size, cyclic, true, enableFlag);

	if (channel < m_channel_list.size() ) {
		if (!multiplex) {
			m_channel_list.at(channel)->write(m_buffer, data);
		} else {
			unsigned short *p_dat;
			int i = 0;

			for (p_dat = (unsigned short *)iio_buffer_start(m_buffer); (p_dat < iio_buffer_end(m_buffer));
			     (unsigned short*)p_dat++, i++) {
				*p_dat = data[i];
			}

		}
		ssize_t ret = iio_buffer_push(m_buffer);
		if (ret < 0) {
			destroy();
			// timeout error code
			if (ret == -ETIMEDOUT) {
				THROW_M2K_EXCEPTION("Buffer: Push timeout occurred", libm2k::EXC_TIMEOUT, ret);
			}
			THROW_M2K_EXCEPTION("Buffer: Cannot push TX buffer", libm2k::EXC_RUNTIME_ERROR, ret);
		}
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer pushed"));
	} else {
		THROW_M2K_EXCEPTION("Buffer: Please setup channels before pushing data", libm2k::EXC_INVALID_PARAMETER);

	}
}

void Buffer::push(std::vector<double> const &data, unsigned int channel, bool cyclic, bool enableFlag)
{
	size_t size = data.size();
	if (Utils::getIioDeviceDirection(m_dev) == INPUT) {
		THROW_M2K_EXCEPTION("Device not output buffer capable, so no buffer was created", libm2k::EXC_INVALID_PARAMETER);
	}

	/* If the data vector is empty, then it means we want
	 * to remove what was pushed earlier to the device, so
	 * we destroy the buffer */
	if (data.size() == 0) {
		return;
	}

	initializeBuffer(size, cyclic, true, enableFlag);

	if (channel < m_channel_list.size() ) {
		m_channel_list.at(channel)->write(m_buffer, data);
		ssize_t ret = iio_buffer_push(m_buffer);
		if (ret < 0) {
			destroy();
			// timeout error code
			if (ret == -ETIMEDOUT) {
				THROW_M2K_EXCEPTION("Buffer: Push timeout occurred", libm2k::EXC_TIMEOUT, ret);
			}
			THROW_M2K_EXCEPTION("Buffer: Cannot push TX buffer", libm2k::EXC_RUNTIME_ERROR, ret);
		}
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer pushed"));
	} else {
		THROW_M2K_EXCEPTION("Buffer: Please setup channels before pushing data", libm2k::EXC_INVALID_PARAMETER);
	}
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
	if (Utils::getIioDeviceDirection(m_dev) == INPUT) {
		THROW_M2K_EXCEPTION("Device not output buffer capable, so no buffer was created", libm2k::EXC_INVALID_PARAMETER);
	}

	/* If the data vector is empty, then it means we want
	 * to remove what was pushed earlier to the device, so
	 * we destroy the buffer */
	if (nb_samples == 0) {
		return;
	}

	initializeBuffer(nb_samples, cyclic, true, enableFlag);

	if (channel < m_channel_list.size() ) {
		m_channel_list.at(channel)->write(m_buffer, data, nb_samples);
		ssize_t ret = iio_buffer_push(m_buffer);
		if (ret < 0) {
			destroy();
			// timeout error code
			if (ret == -ETIMEDOUT) {
				THROW_M2K_EXCEPTION("Buffer: Push timeout occurred", libm2k::EXC_TIMEOUT, ret);
			}
			THROW_M2K_EXCEPTION("Buffer: Cannot push TX buffer", libm2k::EXC_RUNTIME_ERROR, ret);
		}
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer pushed"));
	} else {
		THROW_M2K_EXCEPTION("Buffer: Please setup channels before pushing data", libm2k::EXC_INVALID_PARAMETER);
	}
}

void Buffer::push(short *data, unsigned int channel, unsigned int nb_samples, bool cyclic, bool enableFlag)
{
	if (Utils::getIioDeviceDirection(m_dev) == INPUT) {
		THROW_M2K_EXCEPTION("Device not output buffer capable, so no buffer was created", libm2k::EXC_INVALID_PARAMETER);
	}

	/* If the data vector is empty, then it means we want
	 * to remove what was pushed earlier to the device, so
	 * we destroy the buffer */
	if (nb_samples == 0) {
		return;
	}

	initializeBuffer(nb_samples, cyclic, true, enableFlag);

	if (channel < m_channel_list.size() ) {
		m_channel_list.at(channel)->write(m_buffer, data, nb_samples);
		ssize_t ret = iio_buffer_push(m_buffer);
		if (ret < 0) {
			destroy();
			// timeout error code
			if (ret == -ETIMEDOUT) {
				THROW_M2K_EXCEPTION("Buffer: Push timeout occurred", libm2k::EXC_TIMEOUT, ret);
			}
			THROW_M2K_EXCEPTION("Buffer: Cannot push TX buffer", libm2k::EXC_RUNTIME_ERROR, ret);
		}
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer pushed"));
	} else {
		THROW_M2K_EXCEPTION("Buffer: Please setup channels before pushing data", libm2k::EXC_INVALID_PARAMETER);
	}
}

void Buffer::getSamples(std::vector<unsigned short> &data, unsigned int nb_samples)
{
	if (Utils::getIioDeviceDirection(m_dev) == OUTPUT) {
		THROW_M2K_EXCEPTION("Device not input-buffer capable, so no buffer was created", libm2k::EXC_RUNTIME_ERROR);
	}

	data.clear();

	initializeBuffer(nb_samples, false, false);

	ssize_t ret = iio_buffer_refill(m_buffer);


	if (ret < 0) {
		destroy();
		// timeout error code
		if (ret == -ETIMEDOUT) {
			THROW_M2K_EXCEPTION("Buffer: Refill timeout occurred", libm2k::EXC_TIMEOUT, ret);
		}
		THROW_M2K_EXCEPTION("Buffer: Cannot refill RX buffer", libm2k::EXC_RUNTIME_ERROR, ret);
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer refilled"));

	unsigned short* d_ptr = (unsigned short*)iio_buffer_start(m_buffer);
	for (unsigned int i = 0; i < nb_samples; i++) {
		data.push_back(d_ptr[i]);
	}
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

	ssize_t ret = iio_buffer_refill(m_buffer);


	if (ret < 0) {
		destroy();
		// timeout error code
		if (ret == -ETIMEDOUT) {
			THROW_M2K_EXCEPTION("Buffer: Refill timeout occurred", libm2k::EXC_TIMEOUT, ret);
		}
		THROW_M2K_EXCEPTION("Buffer: Cannot refill RX buffer", libm2k::EXC_RUNTIME_ERROR, ret);
		return nullptr;
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer refilled"));

	const unsigned short* data = (const unsigned short*)iio_buffer_start(m_buffer);
	return data;
}


void Buffer::getSamples(std::vector<std::vector<double>> &data, unsigned int nb_samples,
				const std::function<double(int16_t, unsigned int)> &process)
{
	short* data_p = (short *) getSamplesRawInterleaved(nb_samples);

	std::vector<bool> channels_enabled;
	data.clear();

	for (auto chn : m_channel_list) {
		bool en  = chn->isEnabled();
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
		bool en  = chn->isEnabled();
		anyChannelEnabled = en ? true : anyChannelEnabled;
	}

	if (!anyChannelEnabled) {
		THROW_M2K_EXCEPTION("Buffer: No channel enabled for RX buffer", libm2k::EXC_INVALID_PARAMETER);
		return nullptr;
	}

	initializeBuffer(nb_samples, false, false);

	ssize_t ret = iio_buffer_refill(m_buffer);
	if (ret < 0) {
		destroy();
		// timeout error code
		if (ret == -ETIMEDOUT) {
			THROW_M2K_EXCEPTION("Buffer: Refill timeout occurred", libm2k::EXC_TIMEOUT, ret);
		}
		THROW_M2K_EXCEPTION("Buffer: Cannot refill RX buffer", libm2k::EXC_RUNTIME_ERROR, ret);
		return nullptr;
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer refilled"));

	return m_channel_list.at(0)->getFirstVoid(m_buffer);
}

const double* Buffer::getSamplesInterleaved(unsigned int nb_samples,
				    const std::function<double(int16_t, unsigned int)> &process)
{
	const short* data_p = getSamplesRawInterleaved(nb_samples);

	std::vector<bool> channels_enabled;

	for (auto chn : m_channel_list) {
		bool en  = chn->isEnabled();
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
		iio_buffer_cancel(m_buffer);
	}

	destroy();
}

void Buffer::destroy()
{
	if (m_buffer) {
		iio_buffer_destroy(m_buffer);
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name}, "Buffer destroyed"));
		m_buffer = nullptr;
		m_last_nb_samples = 0;
	}
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

struct iio_buffer* Buffer::getBuffer()
{
	return m_buffer;
}
