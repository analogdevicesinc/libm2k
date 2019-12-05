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

#include "private/devicein_impl.cpp"

using namespace std;
using namespace libm2k;
using namespace libm2k::utils;
using namespace libm2k::contexts;

/*
 * Represents an iio_device
 */
DeviceIn::DeviceIn(struct iio_context* context, std::string dev_name) :
	DeviceGeneric(context, dev_name),
	m_pimpl(std::unique_ptr<DeviceInImpl>(new DeviceInImpl(context, dev_name)))
{
}

DeviceIn::~DeviceIn()
{
}

std::vector<unsigned short> DeviceIn::getSamples(unsigned int nb_samples)
{
	return m_pimpl->getSamples(nb_samples);
}

const unsigned short *DeviceIn::getSamplesP(unsigned int nb_samples)
{
	return m_pimpl->getSamplesP(nb_samples);
}

std::vector<std::vector<double> > DeviceIn::getSamples(unsigned int nb_samples,
				std::function<double(int16_t, unsigned int)> process)
{
	return m_pimpl->getSamples(nb_samples, process);
}

const double *DeviceIn::getSamplesInterleaved(unsigned int nb_samples,
				std::function<double(int16_t, unsigned int)> process)
{
	return m_pimpl->getSamplesInterleaved(nb_samples, process);
}

const short *DeviceIn::getSamplesRawInterleaved(unsigned int nb_samples)
{
	return m_pimpl->getSamplesRawInterleaved(nb_samples);
}

void* DeviceIn::getSamplesRawInterleavedVoid(unsigned int nb_samples)
{
	return m_pimpl->getSamplesRawInterleavedVoid(nb_samples);
}

void DeviceIn::flushBuffer()
{
	m_pimpl->flushBuffer();
}

IIO_OBJECTS DeviceIn::getIioObjects()
{
	return m_pimpl->getIioObjects();
}
