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
