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

#ifndef ENUMS_ANALOG_HPP
#define ENUMS_ANALOG_HPP

#include <vector>
#include <string>
#include <memory>

/**
 * @file analog/enums.hpp
 * @brief Analogical enumerations
 */

namespace libm2k {
namespace analog {


	/**
	* @struct DMM_READING enums.hpp libm2k/analog/enums.hpp
	* @brief The structure of a DMM
	*
	*/
	struct DMM_READING {
		std::string name; ///< The name of the channel
		std::string id; ///< Channel's id
		double value; ///< The measured value
		std::string unit_name; ///< Unit of measurement
		std::string unit_symbol; ///< Unit symbol
	};


	/**
	* @enum ANALOG_IN_CHANNEL
	* @brief Indexes of the channels
	*
	* @note Each channel can be accessed through its index
	*
	*/
	enum ANALOG_IN_CHANNEL {
		ANALOG_IN_CHANNEL_1 = 0,
		ANALOG_IN_CHANNEL_2 = 1
	};


	/**
	* @enum M2K_RANGE
	* @brief Range of the signal's amplitude
	*
	*/
	enum M2K_RANGE {
		PLUS_MINUS_25V = 0,
		PLUS_MINUS_2_5V = 1
	};
}
}


#endif
