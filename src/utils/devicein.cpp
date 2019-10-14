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

#include "private/devicein_impl.cpp"

using namespace std;
using namespace libm2k::utils;
using namespace libm2k::contexts;

/*
 * Represents an iio_device
 */
DeviceIn::DeviceIn(struct iio_context* context, std::string dev_name, bool input) :
	DeviceGeneric(context, dev_name, input),
	m_pimpl(std::unique_ptr<DeviceInImpl>(new DeviceInImpl(context, dev_name, input)))
{
}

DeviceIn::~DeviceIn()
{
}

std::vector<unsigned short> DeviceIn::getSamples(unsigned int nb_samples)
{
	return m_pimpl->getSamples(nb_samples);
}

unsigned short *DeviceIn::getSamplesP(unsigned int nb_samples)
{
	return m_pimpl->getSamplesP(nb_samples);
}

std::vector<std::vector<double> > DeviceIn::getSamples(unsigned int nb_samples,
				std::function<double(int16_t, unsigned int)> process)
{
	return m_pimpl->getSamples(nb_samples, process);
}

double *DeviceIn::getSamplesInterleaved(unsigned int nb_samples,
				std::function<double(int16_t, unsigned int)> process)
{
	return m_pimpl->getSamplesInterleaved(nb_samples, process);
}

short *DeviceIn::getSamplesRawInterleaved(unsigned int nb_samples)
{
	return m_pimpl->getSamplesRawInterleaved(nb_samples);
}

void DeviceIn::flushBuffer()
{
	m_pimpl->flushBuffer();
}
