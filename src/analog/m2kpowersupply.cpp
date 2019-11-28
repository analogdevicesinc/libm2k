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

#include "private/m2kpowersupply_impl.cpp"
using namespace libm2k::analog;

M2kPowerSupply::M2kPowerSupply(iio_context *ctx, std::string write_dev,
			       std::string read_dev, bool sync) :
	m_pimpl(std::unique_ptr<M2kPowerSupplyImpl>(new M2kPowerSupplyImpl(ctx, write_dev, read_dev, sync)))
{
}

M2kPowerSupply::~M2kPowerSupply()
{

}

void M2kPowerSupply::init()
{
	m_pimpl->init();
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
