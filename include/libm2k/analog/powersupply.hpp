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

#ifndef POWERSUPPLY_HPP
#define POWERSUPPLY_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/utils/device.hpp>
#include <vector>
#include <string>
#include <memory>

using namespace libm2k::utils;

namespace libm2k {
namespace analog {
class LIBM2K_API PowerSupply : public Device
{
public:
	PowerSupply(struct iio_context* ctx, std::string write_dev = "",
		     std::string read_dev = "");
	virtual ~PowerSupply();

private:
	class PowerSupplyImpl;
	std::shared_ptr<PowerSupplyImpl> m_pimpl;
};
}
}

#endif //POWERSUPPLY_HPP
