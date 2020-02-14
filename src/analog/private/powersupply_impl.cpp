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

#include "utils/deviceout.hpp"
#include "utils/devicein.hpp"
#include "utils/devicegeneric.hpp"
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
