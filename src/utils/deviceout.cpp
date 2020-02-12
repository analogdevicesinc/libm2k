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

#include "private/deviceout_impl.cpp"

using namespace std;
using namespace libm2k;
using namespace libm2k::utils;
using namespace libm2k::contexts;

/*
 * Represents an iio_device
 */
DeviceOut::DeviceOut(struct iio_context* context, std::string dev_name) :
	DeviceGeneric(context, dev_name),
	m_pimpl(std::unique_ptr<DeviceOutImpl>(new DeviceOutImpl(context, dev_name)))
{
}

DeviceOut::~DeviceOut()
{
}

void DeviceOut::initializeBuffer(unsigned int size, bool cyclic)
{
	m_pimpl->initializeBuffer(size, cyclic);
}

void DeviceOut::push(std::vector<short> const &data, unsigned int channel,
                     bool cyclic, bool multiplex)
{
    m_pimpl->push(data, channel, cyclic, multiplex);
}

void DeviceOut::push(std::vector<unsigned short> const &data, unsigned int channel,
		  bool cyclic, bool multiplex)
{
	m_pimpl->push(data, channel, cyclic, multiplex);
}

void DeviceOut::push(unsigned short *data, unsigned int channel, unsigned int nb_samples,
		  bool cyclic, bool multiplex)
{
	m_pimpl->push(data, channel, nb_samples, cyclic, multiplex);
}

void DeviceOut::push(std::vector<double> const &data, unsigned int channel, bool cyclic)
{
	m_pimpl->push(data, channel, cyclic);
}

void DeviceOut::push(double *data, unsigned int channel, unsigned int nb_samples, bool cyclic)
{
	m_pimpl->push(data, channel, nb_samples, cyclic);
}

void DeviceOut::push(short *data, unsigned int channel, unsigned int nb_samples, bool cyclic)
{
	m_pimpl->push(data, channel, nb_samples, cyclic);
}

void DeviceOut::stop()
{
	m_pimpl->stop();
}

void DeviceOut::cancelBuffer()
{
	m_pimpl->cancelBuffer();
}

void DeviceOut::setKernelBuffersCount(unsigned int count)
{
	m_pimpl->setKernelBuffersCount(count);
}

IIO_OBJECTS DeviceOut::getIioObjects()
{
	return m_pimpl->getIioObjects();
}
