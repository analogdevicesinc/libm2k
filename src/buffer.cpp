/*
 * Copyright 2016 Analog Devices, Inc.
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


#include "libm2k/buffer.hpp"
#include "libm2k/channel.hpp"
#include "libm2k/m2kexceptions.hpp"
#include "libm2k/utils.hpp"

#include <iostream>
#include <algorithm>

using namespace std;
using namespace libm2k::utils;

Buffer::Buffer(struct iio_device *dev)
{
	m_dev = dev;

	if (!m_dev) {
		m_dev = nullptr;
		throw no_device_exception("Buffer: Device not found, so no buffer can be created");
	}

	unsigned int dev_count = iio_device_get_buffer_attrs_count(m_dev);
	if (dev_count <= 0) {
		throw no_device_exception("Buffer: Device is not buffer capable, no buffer can be created");
	}
	m_buffer = nullptr;
}

Buffer::~Buffer()
{
	m_data.clear();
	m_data_short.clear();
}

/* push on multiple channels
 * should call setChannels(channels) before using the
 * push methods
 */
void Buffer::push(std::vector<std::vector<short>> &data)
{
	size_t data_ch_nb = data.size();

	if (data_ch_nb > m_channel_list.size()) {
		throw invalid_parameter_exception("Buffer: Please setup channels before pushing data");
	}

	for (unsigned int i = 0; i < data_ch_nb; i++) {
		push(data.at(i), i);
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

//push on a certain channel
void Buffer::push(std::vector<short> &data, unsigned int channel,
		  bool cyclic, bool multiplex)
{
	size_t size = data.size();
	if (Utils::getIioDeviceDirection(m_dev) != Utils::OUTPUT) {
		throw no_device_exception("Device not found, so no buffer was created");
	}

	destroy();

	m_buffer = iio_device_create_buffer(m_dev, size, cyclic);

	if (!m_buffer) {
		throw invalid_parameter_exception("Buffer: Can't create the TX buffer");
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
		throw invalid_parameter_exception("Buffer: Please setup channels before pushing data");
	}
}

//push on a certain channel
void Buffer::push(std::vector<double> &data, unsigned int channel, bool cyclic)
{
	size_t size = data.size();
	if (Utils::getIioDeviceDirection(m_dev) == Utils::INPUT) {
		throw no_device_exception("Device not found, so no buffer was created");
	}

	destroy();

	m_buffer = iio_device_create_buffer(m_dev, size, cyclic);

	if (!m_buffer) {
		throw invalid_parameter_exception("Buffer: Can't create the TX buffer");
	}

	if (channel < m_channel_list.size() ) {
		m_channel_list.at(channel)->write(m_buffer, data);
		iio_buffer_push(m_buffer);
	} else {
		throw invalid_parameter_exception("Buffer: Please setup channels before pushing data");
	}
}

std::vector<unsigned short> Buffer::getSamples(int nb_samples)
{
	if (Utils::getIioDeviceDirection(m_dev) == Utils::OUTPUT) {
		throw no_device_exception("Device not found, so no buffer was created");
	}

	if (m_buffer) {
		iio_buffer_destroy(m_buffer);
		m_buffer = nullptr;
	}

	m_buffer = iio_device_create_buffer(m_dev, nb_samples, false);
	if (!m_buffer) {
		throw invalid_parameter_exception("Buffer: Can't create the RX buffer");
	}

	int ret = iio_buffer_refill(m_buffer);

	if (ret < 0) {
		destroy();
		throw instrument_already_in_use_exception(
			"Buffer: Cannot refill RX buffer");
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

std::vector<std::vector<double>> Buffer::getSamples(int nb_samples,
				std::function<double(int16_t, unsigned int)> process)
{
	std::vector<bool> channels_enabled;
//	std::vector<std::vector<int16_t>> ch_data;
	if (Utils::getIioDeviceDirection(m_dev) != Utils::INPUT) {
		throw no_device_exception("Device not found, so no buffer was created");
	}

	if (m_buffer) {
		iio_buffer_destroy(m_buffer);
		m_buffer = nullptr;
	}

	m_buffer = iio_device_create_buffer(m_dev, nb_samples, false);
	if (!m_buffer) {
		throw invalid_parameter_exception("Buffer: Can't create the RX buffer");
	}

	int ret = iio_buffer_refill(m_buffer);

	if (ret < 0) {
		destroy();

		throw instrument_already_in_use_exception(
			"Buffer: Cannot refill RX buffer");
	}

	m_data.clear();

	for (auto chn : m_channel_list) {
		bool en  = chn->isEnabled();
		channels_enabled.push_back(en);
		std::vector<double> data {};
		for (int j = 0; j < nb_samples; j++) {
			data.push_back(0);
		}
		m_data.push_back(data);
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


//	// Restore channels enable states
//	for (unsigned int i = 0; i < m_nb_channels; i++) {
//		enableChannel(i, channels_enabled.at(i));
//	}
	return m_data;
}

void Buffer::stop()
{
	if (Utils::getIioDeviceDirection(m_dev) != Utils::OUTPUT) {
		return;
//		throw no_device_exception("Device not found, so no buffer was created");
	}

	destroy();
}

void Buffer::destroy()
{
	if (m_buffer) {
		iio_buffer_destroy(m_buffer);
		m_buffer = nullptr;
	}
}
