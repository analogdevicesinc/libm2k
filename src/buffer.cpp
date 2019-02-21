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
	if (dev_count < 0) {
		throw no_device_exception("Buffer: Device is not buffer capable, no buffer can be created");
	}
	m_buffer = nullptr;
}

Buffer::~Buffer()
{

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
void Buffer::push(std::vector<short> &data, unsigned int channel)
{
	size_t size = data.size();
	if (Utils::getIioDeviceDirection(m_dev) != Utils::OUTPUT) {
		throw no_device_exception("Device not found, so no buffer was created");
	}

	if (m_buffer) {
		iio_buffer_destroy(m_buffer);
		m_buffer = nullptr;
	}

	m_buffer = iio_device_create_buffer(m_dev, size, true);

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

//push on a certain channel
void Buffer::push(std::vector<double> &data, unsigned int channel)
{
	size_t size = data.size();
	if (Utils::getIioDeviceDirection(m_dev) != Utils::OUTPUT) {
		throw no_device_exception("Device not found, so no buffer was created");
	}

	if (m_buffer) {
		iio_buffer_destroy(m_buffer);
		m_buffer = nullptr;
	}

	m_buffer = iio_device_create_buffer(m_dev, size, true);

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

//void Buffer::refill()
//{

//}

void Buffer::stop()
{
	if (Utils::getIioDeviceDirection(m_dev) != Utils::OUTPUT) {
		return;
//		throw no_device_exception("Device not found, so no buffer was created");
	}

	if (m_buffer) {
		iio_buffer_destroy(m_buffer);
		m_buffer = nullptr;
	}
}
