/*
 * Copyright 2019 Analog Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file LICENSE.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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
	}

	~BufferImpl() {
		destroy();
		m_data.clear();
		m_data_short.clear();
	}

	void setChannels(std::vector<Channel*> channels)
	{
		//if (m_buffer) //this means the output is running, should we change the channels now?
		//{
		//throw exception
		//}
		m_channel_list = channels;
	}

	//push on a certain channel
	void push(std::vector<short> &data, unsigned int channel = 0,
		  bool cyclic = true, bool multiplex = false)
	{
		size_t size = data.size();
		if (Utils::getIioDeviceDirection(m_dev) != OUTPUT) {
			throw_exception(EXC_INVALID_PARAMETER, "Device not output buffer capable, so no buffer was created");
		}

		destroy();

		/* If the data vector is empty, then it means we want
		 * to remove what was pushed earlier to the device, so
		 * we destroy the buffer */
		if (data.size() == 0) {
			return;
		}

		m_buffer = iio_device_create_buffer(m_dev, size, cyclic);

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
				     (unsigned short*)p_dat++, i++) {
					*p_dat = data[i];
				}

			}
			iio_buffer_push(m_buffer);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Please setup channels before pushing data");

		}
	}

	void push(std::vector<double> &data, unsigned int channel = 0, bool cyclic = true)
	{
		size_t size = data.size();
		if (Utils::getIioDeviceDirection(m_dev) == INPUT) {
			throw_exception(EXC_INVALID_PARAMETER, "Device not output buffer capable, so no buffer was created");
		}

		destroy();

		/* If the data vector is empty, then it means we want
		 * to remove what was pushed earlier to the device, so
		 * we destroy the buffer */
		if (data.size() == 0) {
			return;
		}

		m_buffer = iio_device_create_buffer(m_dev, size, cyclic);

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

	void push(std::vector<std::vector<short>> &data)
	{
		size_t data_ch_nb = data.size();

		if (data_ch_nb > m_channel_list.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Buffer: Please setup channels before pushing data");
		}

		for (unsigned int i = 0; i < data_ch_nb; i++) {
			push(data.at(i), i);
		}
	}

	std::vector<unsigned short> getSamples(int nb_samples)
	{
		if (Utils::getIioDeviceDirection(m_dev) == OUTPUT) {
			throw_exception(EXC_RUNTIME_ERROR, "Device not input-buffer capable, so no buffer was created");
		}

		if (m_buffer) {
			iio_buffer_destroy(m_buffer);
			m_buffer = nullptr;
		}

		m_buffer = iio_device_create_buffer(m_dev, nb_samples, false);
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Cannot create the RX buffer");
		}

		int ret = iio_buffer_refill(m_buffer);

		if (ret < 0) {
			destroy();
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Cannot refill RX buffer");
		}

		m_data_short.clear();

		for (int j = 0; j < nb_samples; j++) {
			m_data_short.push_back(0);
		}

		unsigned short* data = (unsigned short*)iio_buffer_start(m_buffer);
		for (int i = 0; i < nb_samples; i++) {
			m_data_short[i] = data[i];
		}

		destroy();
		return m_data_short;
	}

	std::vector<std::vector<double>> getSamples(int nb_samples,
					std::function<double(int16_t, unsigned int)> process)
	{
		bool anyChannelEnabled = false;
		std::vector<bool> channels_enabled;
		if (Utils::getIioDeviceDirection(m_dev) != INPUT) {
			throw_exception(EXC_INVALID_PARAMETER, "Device not found, so no buffer was created");
		}

		m_data.clear();

		for (auto chn : m_channel_list) {
			bool en  = chn->isEnabled();
			channels_enabled.push_back(en);
			anyChannelEnabled = en ? true : anyChannelEnabled;
			std::vector<double> data {};
			for (int j = 0; j < nb_samples; j++) {
				data.push_back(0);
			}
			m_data.push_back(data);
		}

		if (!anyChannelEnabled) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: No channel enabled for RX buffer");
		}

		if (m_buffer) {
			iio_buffer_destroy(m_buffer);
			m_buffer = nullptr;
		}

		m_buffer = iio_device_create_buffer(m_dev, nb_samples, false);
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Can't create the RX buffer");
		}

		int ret = iio_buffer_refill(m_buffer);

		if (ret < 0) {
			destroy();
			throw_exception(EXC_INVALID_PARAMETER, "Buffer: Cannot refill RX buffer");
		}

		ptrdiff_t p_inc = iio_buffer_step(m_buffer);
		uintptr_t p_dat;
		uintptr_t p_end = (uintptr_t)iio_buffer_end(m_buffer);
		unsigned int i;
		for (i = 0, p_dat = m_channel_list.at(0)->getFirst(m_buffer);
				p_dat < p_end; p_dat += p_inc, i++)
		{
			for (int ch = 0; ch < m_data.size(); ch++) {
				m_data[ch][i] = process(((int16_t*)p_dat)[ch], ch);
			}
		}

		destroy();
		return m_data;
	}

	void stop()
	{
		if (Utils::getIioDeviceDirection(m_dev) != OUTPUT) {
			return;
		}

		destroy();
	}

	void destroy()
	{
		if (m_buffer) {
			iio_buffer_destroy(m_buffer);
			m_buffer = nullptr;
		}
	}
private:
	struct iio_device* m_dev;
	struct iio_buffer* m_buffer;
	std::vector<Channel*> m_channel_list;
	std::vector<std::vector<double>> m_data;
	std::vector<unsigned short> m_data_short;

};

