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

#include <libm2k/utils/devicegeneric.hpp>
#include <libm2k/utils/devicein.hpp>
#include <libm2k/utils/deviceout.hpp>
#include <libm2k/analog/powersupply.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <iio.h>
#include <iostream>

using namespace libm2k::analog;
using namespace libm2k::utils;

class PowerSupply::PowerSupplyImpl : public DeviceGeneric {
public:
	PowerSupplyImpl(struct iio_context* ctx, std::string write_dev,
				 std::string read_dev) :
		DeviceGeneric(ctx, "")
	{
		if (write_dev != "") {
			m_dev_write = make_shared<DeviceOut>(ctx, write_dev);
			if (!m_dev_write) {
				m_dev_write = nullptr;
				throw_exception(EXC_INVALID_PARAMETER, "Power Supply: No device was found for writing");
			}
		}

		if (read_dev != "") {
			m_dev_read = make_shared<DeviceIn>(ctx, read_dev);
			if (!m_dev_read) {
				m_dev_read = nullptr;
				throw_exception(EXC_INVALID_PARAMETER, "Power Supply: No device was found for reading");
			}
		}
	}

	~PowerSupplyImpl()
	{
	}

private:
	std::shared_ptr<DeviceOut> m_dev_write;
	std::shared_ptr<DeviceIn> m_dev_read;
};
