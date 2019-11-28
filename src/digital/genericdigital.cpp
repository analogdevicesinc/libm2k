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

#include "private/genericdigital_impl.cpp"

using namespace libm2k::digital;
using namespace std;

GenericDigital::GenericDigital(iio_context *ctx, string logic_dev) :
	m_pimpl(std::unique_ptr<GenericDigitalImpl>(new GenericDigitalImpl(ctx, logic_dev)))
{
}

GenericDigital::~GenericDigital()
{
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

