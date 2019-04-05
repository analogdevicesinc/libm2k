/*
 * Copyright 2018 Analog Devices, Inc.
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

#include "private/genericdigital_impl.cpp"

using namespace libm2k::utils;
using namespace libm2k::digital;
using namespace std;

GenericDigital::GenericDigital(iio_context *ctx, string logic_dev) :
	Device(new GenericDigitalImpl(ctx, logic_dev))
{
	m_pimpl = std::unique_ptr<GenericDigitalImpl>(
			static_cast<GenericDigitalImpl*>(Device::m_pimpl.get()));
}

GenericDigital::~GenericDigital()
{
	m_pimpl.reset();
}

double GenericDigital::getSampleRate()
{
	return m_pimpl->getSampleRate();
}

double GenericDigital::setSampleRate(double sampleRate)
{
	return m_pimpl->setSampleRate(sampleRate);
}

void GenericDigital::setCyclic(bool cyclic)
{
	m_pimpl->setCyclic(cyclic);
}

bool GenericDigital::getCyclic()
{
	return m_pimpl->getCyclic();
}

void GenericDigital::enableChannel(unsigned int index, bool enable)
{
	m_pimpl->enableChannel(index, enable);
}

