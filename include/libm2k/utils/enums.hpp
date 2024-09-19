/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software; you can redistribute it and/or modify
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

#ifndef ENUMS_UTILS_HPP
#define ENUMS_UTILS_HPP

#include <string>
#include <vector>

namespace libm2k {
namespace utils {
	enum DEVICE_TYPE {
		ANALOG_DEV = 1,
		DIGITAL_DEV = 2,
		NO_DEV = 3
	};

	enum DEVICE_DIRECTION {
		INPUT = 1,
		OUTPUT = 2,
		BOTH = 3,
		NO_DIRECTION = 4
	};

	struct ini_device_struct {
		std::string hw_name;
		std::vector< std::pair< std::string,std::vector<std::string>>>
			key_val_pairs;
	};
}
}


#endif
