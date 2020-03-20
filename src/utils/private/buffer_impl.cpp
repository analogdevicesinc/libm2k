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

#include <libm2k/utils/buffer.hpp>
#include <libm2k/utils/channel.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>

#include <iostream>
#include <memory>
#include <algorithm>

using namespace std;
using namespace libm2k::utils;

class Buffer::BufferImpl {
public:
	BufferImpl(struct iio_device *dev) {
		m_dev = dev;

		if (!m_dev) {
			m_dev = nullptr;
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Device not found, so no buffer can be created");
		}

		unsigned int dev_count = iio_device_get_buffer_attrs_count(m_dev);
		if (dev_count <= 0) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Device is not buffer capable, no buffer can be created");
		}
		m_buffer = nullptr;
		m_last_nb_samples = 0;
	}

	~BufferImpl() {
		stop();
		destroy();
		m_data.clear();
		m_data_short.clear();
	}

	void initializeBuffer(unsigned int size, bool cyclic)
	{
		/* In non-cyclic mode pushing samples will fill the internal buffers, creating the possibility of continuous
		* data transferring; the buffer must be destroy when its size is changed
		*
		* In cyclic mode the very first buffer pushed will be repeated; in order to push any other buffer the
		* old buffer must be destroyed and a new one must be created*/
		if (size != m_last_nb_samples || cyclic) {
			destroy();
			m_last_nb_samples = size;
			m_buffer = iio_device_create_buffer(m_dev, size, cyclic);
		}
	}

	void setChannels(std::vector<Channel*> channels)
	{
		//if (m_buffer) //this means the output is running, should we change the channels now?
		//{
		//throw exception
		//}
		m_channel_list = channels;
	}

	void push(unsigned short *data, unsigned int channel, unsigned int nb_samples,
			  bool cyclic = true, bool multiplex = false)
	{
		if (Utils::getIioDeviceDirection(m_dev) != OUTPUT) {
			throw_exception(EXC_INVALID_PARAMETER, "Device not output buffer capable, so no buffer was created");
		}

		initializeBuffer(nb_samples, cyclic);

		/* If the data vector is empty, then it means we want
		 * to remove what was pushed earlier to the device, so
		 * we destroy the buffer */
		if (nb_samples == 0) {
			return;
		}

		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Can't create the TX buffer");
		}

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
			iio_buffer_push(m_buffer);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Please setup channels before pushing data");

		}
	}

	//push on a certain channel
	void push(std::vector<short> const &data, unsigned int channel = 0,
		bool cyclic = true, bool multiplex = false)
	{
		size_t size = data.size();
		if (Utils::getIioDeviceDirection(m_dev) != OUTPUT) {
		throw_exception(EXC_INVALID_PARAMETER, "Device not output buffer capable, so no buffer was created");
		}

		initializeBuffer(size, cyclic);

		/* If the data vector is empty, then it means we want
		* to remove what was pushed earlier to the device, so
		* we destroy the buffer */
		if (data.size() == 0) {
			return;
		}

		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Can't create the TX buffer");
		}

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
			iio_buffer_push(m_buffer);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Please setup channels before pushing data");

		}
	}

	void push(std::vector<unsigned short> const &data, unsigned int channel = 0,
		bool cyclic = true, bool multiplex = false)
	{
		size_t size = data.size();
		if (Utils::getIioDeviceDirection(m_dev) != OUTPUT) {
			throw_exception(EXC_INVALID_PARAMETER, "Device not output buffer capable, so no buffer was created");
		}

		initializeBuffer(size, cyclic);

		/* If the data vector is empty, then it means we want
		 * to remove what was pushed earlier to the device, so
		 * we destroy the buffer */
		if (data.size() == 0) {
			return;
		}

		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Can't create the TX buffer");
		}

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
			iio_buffer_push(m_buffer);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Please setup channels before pushing data");

		}
	}

	void push(std::vector<double> const &data, unsigned int channel = 0, bool cyclic = true)
	{
		size_t size = data.size();
		if (Utils::getIioDeviceDirection(m_dev) == INPUT) {
			throw_exception(EXC_INVALID_PARAMETER, "Device not output buffer capable, so no buffer was created");
		}

		initializeBuffer(size, cyclic);

		/* If the data vector is empty, then it means we want
		 * to remove what was pushed earlier to the device, so
		 * we destroy the buffer */
		if (data.size() == 0) {
			return;
		}

		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Can't create the TX buffer");
		}

		if (channel < m_channel_list.size() ) {
			m_channel_list.at(channel)->write(m_buffer, data);
			iio_buffer_push(m_buffer);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Please setup channels before pushing data");
		}
	}

	void push(std::vector<std::vector<short>> const &data)
	{
		size_t data_ch_nb = data.size();

		if (data_ch_nb > m_channel_list.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Buffer: Please setup channels before pushing data");
		}

		for (unsigned int i = 0; i < data_ch_nb; i++) {
			push(data.at(i), i);
		}
	}

	void push(double *data, unsigned int channel, unsigned int nb_samples, bool cyclic)
	{
		if (Utils::getIioDeviceDirection(m_dev) == INPUT) {
			throw_exception(EXC_INVALID_PARAMETER, "Device not output buffer capable, so no buffer was created");
		}

		initializeBuffer(nb_samples, cyclic);

		/* If the data vector is empty, then it means we want
		 * to remove what was pushed earlier to the device, so
		 * we destroy the buffer */
		if (nb_samples == 0) {
			return;
		}


		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Can't create the TX buffer");
		}

		if (channel < m_channel_list.size() ) {
			m_channel_list.at(channel)->write(m_buffer, data, nb_samples);
			iio_buffer_push(m_buffer);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Please setup channels before pushing data");
		}
	}

	void push(short *data, unsigned int channel, unsigned int nb_samples, bool cyclic)
	{
		if (Utils::getIioDeviceDirection(m_dev) == INPUT) {
			throw_exception(EXC_INVALID_PARAMETER, "Device not output buffer capable, so no buffer was created");
		}

		initializeBuffer(nb_samples, cyclic);

		/* If the data vector is empty, then it means we want
		 * to remove what was pushed earlier to the device, so
		 * we destroy the buffer */
		if (nb_samples == 0) {
			return;
		}

		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Can't create the TX buffer");
		}

		if (channel < m_channel_list.size() ) {
			m_channel_list.at(channel)->write(m_buffer, data, nb_samples);
			iio_buffer_push(m_buffer);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Please setup channels before pushing data");
		}
	}

	void getSamples(std::vector<unsigned short> &data, int nb_samples)
	{
		if (Utils::getIioDeviceDirection(m_dev) == OUTPUT) {
			throw_exception(EXC_RUNTIME_ERROR, "Device not input-buffer capable, so no buffer was created");
		}

		data.clear();

		bool new_buffer = (nb_samples != m_last_nb_samples);
		if (new_buffer) {
			destroy();
			m_buffer = iio_device_create_buffer(m_dev, nb_samples, false);
			m_last_nb_samples = nb_samples;
		}

		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Cannot create the RX buffer");
		}

		int ret = iio_buffer_refill(m_buffer);


		if (ret < 0) {
			destroy();
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Cannot refill RX buffer");
		}

		unsigned short* d_ptr = (unsigned short*)iio_buffer_start(m_buffer);
		for (int i = 0; i < nb_samples; i++) {
			data.push_back(d_ptr[i]);
		}
	}

	std::vector<unsigned short> getSamples(int nb_samples)
	{
		m_data_short.clear();
		getSamples(m_data_short, nb_samples);
		return m_data_short;
	}

	const unsigned short *getSamplesP(int nb_samples)
	{
		if (Utils::getIioDeviceDirection(m_dev) == OUTPUT) {
			throw_exception(EXC_RUNTIME_ERROR, "Device not input-buffer capable, so no buffer was created");
		}

		bool new_buffer = (nb_samples != m_last_nb_samples);
		if (new_buffer) {
			destroy();
			m_buffer = iio_device_create_buffer(m_dev, nb_samples, false);
			m_last_nb_samples = nb_samples;
		}

		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Cannot create the RX buffer");
		}

		int ret = iio_buffer_refill(m_buffer);


		if (ret < 0) {
			destroy();
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Cannot refill RX buffer");
		}

		const unsigned short* data = (const unsigned short*)iio_buffer_start(m_buffer);
		return data;
	}


	const short* getSamplesRawInterleaved(int nb_samples)
	{
		return static_cast<const short*>(getSamplesRawInterleavedVoid(nb_samples));
	}

	void *getSamplesRawInterleavedVoid(int nb_samples)
	{
		bool anyChannelEnabled = false;
		if (Utils::getIioDeviceDirection(m_dev) != INPUT) {
			throw_exception(EXC_INVALID_PARAMETER, "Device not found, so no buffer was created");
		}

		for (auto chn : m_channel_list) {
			bool en  = chn->isEnabled();
			anyChannelEnabled = en ? true : anyChannelEnabled;
		}

		if (!anyChannelEnabled) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: No channel enabled for RX buffer");
		}

		bool new_buffer = (nb_samples != m_last_nb_samples);
		if (new_buffer) {
			destroy();
			m_buffer = iio_device_create_buffer(m_dev, nb_samples, false);
			m_last_nb_samples = nb_samples;
		}

		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Can't create the RX buffer");
		}

		ssize_t ret = iio_buffer_refill(m_buffer);
		if (ret < 0) {
			destroy();
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Cannot refill RX buffer");
		}

		return m_channel_list.at(0)->getFirstVoid(m_buffer);
	}

	const double* getSamplesInterleaved(int nb_samples,
				      std::function<double(int16_t, unsigned int)> process)
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


	std::vector<std::vector<double>> getSamples(int nb_samples,
					std::function<double(int16_t, unsigned int)> process)
	{
		m_data.clear();
		getSamples(m_data, nb_samples, process);
		return m_data;
	}


	void getSamples(std::vector<std::vector<double>> &data, int nb_samples,
					std::function<double(int16_t, unsigned int)> process)
	{
		short* data_p = (short *) getSamplesRawInterleaved(nb_samples);

		std::vector<bool> channels_enabled;
		data.clear();

		for (auto chn : m_channel_list) {
			bool en  = chn->isEnabled();
			channels_enabled.push_back(en);
			std::vector<double> ch_data {};
			data.push_back(ch_data);
		}

		unsigned int i;
		unsigned int nb_channels = m_channel_list.size();

		for (i = 0; i < nb_samples; i++) {
			for (unsigned int ch = 0; ch < nb_channels; ch++) {
				if (channels_enabled.at(ch)) {
					data.at(ch).push_back(process(data_p[i * nb_channels + ch], ch));
				}
			}
		}
	}

	void stop()
	{
		if (Utils::getIioDeviceDirection(m_dev) != OUTPUT) {
			return;
		}

		if (m_buffer) {
			iio_buffer_cancel(m_buffer);
		}
		destroy();
	}

	void destroy()
	{
		if (m_buffer) {
			iio_buffer_destroy(m_buffer);
			m_buffer = nullptr;
			m_last_nb_samples = 0;
		}
	}

	void cancelBuffer()
	{
		if (m_buffer) {
			iio_buffer_cancel(m_buffer);
		}
	}

	void flushBuffer()
	{
		destroy();
		m_last_nb_samples = 0;
	}

	void setCyclic(bool enable)
	{
		m_cyclic = enable;
	}

	struct iio_buffer* getBuffer()
	{
		return m_buffer;
	}

private:
	struct iio_device* m_dev;
	struct iio_buffer* m_buffer;
	int m_last_nb_samples;
	bool m_cyclic;
	std::vector<Channel*> m_channel_list;
	std::vector<std::vector<double>> m_data;
	std::vector<unsigned short> m_data_short;

};

