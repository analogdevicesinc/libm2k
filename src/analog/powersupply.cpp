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

#include "private/powersupply_impl.cpp"

using namespace libm2k::analog;

PowerSupply::PowerSupply(struct iio_context* ctx, std::string write_dev,
			 std::string read_dev) :
	m_pimpl(std::unique_ptr<PowerSupplyImpl>(new PowerSupplyImpl(ctx, write_dev, read_dev)))
{
}

PowerSupply::~PowerSupply()
{
}

void PowerSupply::enableChannel(unsigned int chnidx, bool enable)
{
	m_pimpl->enableChannel(chnidx, enable);
}

