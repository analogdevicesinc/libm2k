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

#include "private/deviceout_impl.cpp"

using namespace std;
using namespace libm2k::utils;
using namespace libm2k::devices;

/*
 * Represents an iio_device
 */
DeviceOut::DeviceOut(struct iio_context* context, std::string dev_name, bool input) :
	DeviceGeneric(context, dev_name, input),
	m_pimpl(std::unique_ptr<DeviceOutImpl>(new DeviceOutImpl(context, dev_name, input)))
{
}

DeviceOut::~DeviceOut()
{
}

void DeviceOut::push(std::vector<int> const &data, unsigned int channel,
		  bool cyclic, bool multiplex)
{
	m_pimpl->push(data, channel, cyclic, multiplex);
}

void DeviceOut::push(std::vector<double> const &data, unsigned int channel, bool cyclic)
{
	m_pimpl->push(data, channel, cyclic);
}

void DeviceOut::stop()
{
	m_pimpl->stop();
}
