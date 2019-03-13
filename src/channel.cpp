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

#include "libm2k/channel.hpp"
#include "libm2k/m2kexceptions.hpp"
#include "libm2k/utils.hpp"

using namespace libm2k::utils;

Channel::Channel(iio_device *device, unsigned int channel)
{
	m_device = device;
	if (m_device) {
		m_channel = iio_device_get_channel(m_device, channel);
	}

	if (!m_channel) {
		m_channel = nullptr;
		throw no_device_exception("Channel: Channel not found for this device");
	}
}

Channel::Channel(iio_device *device, std::string channel_name, bool output)
{
	m_device = device;
	if (m_device) {
		m_channel = iio_device_find_channel(device, channel_name.c_str(), output);
	}

	if (!m_channel) {
		m_channel = nullptr;
		throw no_device_exception("Channel: Channel not found for this device");
	}

}

Channel::~Channel()
{

}

std::string Channel::getName()
{
	if (!m_channel) {
		throw no_device_exception("Channel: Can not find associated channel");
	}
	return iio_channel_get_name(m_channel);
}

std::string Channel::getId()
{
	if (!m_channel) {
		throw no_device_exception("Channel: Can not find associated channel");
	}
	return iio_channel_get_id(m_channel);
}

unsigned int Channel::getIndex()
{
	if (!m_channel) {
		throw no_device_exception("Channel: Can not find associated channel");
	}
	return iio_channel_get_index(m_channel);
}

bool Channel::isOutput()
{
	if (!m_channel) {
		throw no_device_exception("Channel: Can not find associated channel");
	}
	return iio_channel_is_output(m_channel);
}

void Channel::enableChannel(bool enable)
{
	if (!m_channel) {
		throw no_device_exception("Channel: Can not find associated channel");
	}

	if (enable) {
		iio_channel_enable(m_channel);
	} else {
		iio_channel_disable(m_channel);
	}
}

void Channel::write(struct iio_buffer* buffer, std::vector<short> &data)
{
	if (!m_channel) {
		throw no_device_exception("Channel: Can not find associated channel");
	}

	size_t size = data.size();
	size_t ret = iio_channel_write(m_channel, buffer, data.data(),
				       size * sizeof(short));
}

void Channel::write(struct iio_buffer* buffer, std::vector<double> &data)
{
	if (!m_channel) {
		throw no_device_exception("Channel: Can not find associated channel");
	}

	size_t size = data.size();
	size_t ret = iio_channel_write(m_channel, buffer, data.data(),
				       size * sizeof(double));
}
