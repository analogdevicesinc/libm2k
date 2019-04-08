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

#include "private/m2kpowersupply_impl.cpp"
using namespace libm2k::analog;

M2kPowerSupply::M2kPowerSupply(iio_context *ctx, std::string write_dev,
			       std::string read_dev) :
	Device(new M2kPowerSupplyImpl(ctx, write_dev, read_dev))
{
	m_pimpl = dynamic_pointer_cast<M2kPowerSupplyImpl>(Device::m_pimpl);
}

M2kPowerSupply::~M2kPowerSupply()
{

}

void M2kPowerSupply::powerDownDacs(bool powerdown)
{
	m_pimpl->powerDownDacs(powerdown);
}

void M2kPowerSupply::enableChannel(unsigned int chnIdx, bool en)
{
	m_pimpl->enableChannel(chnIdx, en);
}

bool M2kPowerSupply::anyChannelEnabled()
{
	return m_pimpl->anyChannelEnabled();
}

void M2kPowerSupply::enableAll(bool en)
{
	m_pimpl->enableAll(en);
}

double M2kPowerSupply::readChannel(unsigned int idx)
{
	return m_pimpl->readChannel(idx);
}

void M2kPowerSupply::pushChannel(unsigned int chnIdx, double value)
{
	m_pimpl->pushChannel(chnIdx, value);
}
