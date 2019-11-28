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

#ifndef ENUMS_DIGITAL_HPP
#define ENUMS_DIGITAL_HPP

#include <iio.h>

/**
 * @file digital/enums.hpp
 * @brief Digital enumerations
 */

namespace libm2k {
namespace digital {


	/**
	* @enum DIO_DIRECTION
	* @brief Direction of a digital channel
	*
	* Digital channels can be used both for transmitting and receiving digital signals.
	*
	*/
	enum DIO_DIRECTION {
		DIO_INPUT = 0,
		DIO_OUTPUT = 1,
	};


	/**
	* @enum DIO_LEVEL
	* @brief Logic voltage levels
	*
	* At any given time a digital signals can only take two possible values, namely low or high
	*
	*/
	enum DIO_LEVEL {
		LOW = 0,
		HIGH = 1,
	};


	/**
	* @enum DIO_CHANNEL
	* @brief Indexes of the channels
	*
	* Each channel can be accessed through its index
	*
	*/
	enum DIO_CHANNEL {
		DIO_CHANNEL_0 = 0,
		DIO_CHANNEL_1 = 1,
		DIO_CHANNEL_2 = 2,
		DIO_CHANNEL_3 = 3,
		DIO_CHANNEL_4 = 4,
		DIO_CHANNEL_5 = 5,
		DIO_CHANNEL_6 = 6,
		DIO_CHANNEL_7 = 7,
		DIO_CHANNEL_8 = 8,
		DIO_CHANNEL_9 = 9,
		DIO_CHANNEL_10 = 10,
		DIO_CHANNEL_11 = 11,
		DIO_CHANNEL_12 = 12,
		DIO_CHANNEL_13 = 13,
		DIO_CHANNEL_14 = 14,
		DIO_CHANNEL_15 = 15,
	};


	/**
	* @enum DIO_MODE
	* @brief Output mode for a digital channel
	*
	*/
	enum DIO_MODE {
		DIO_OPENDRAIN = 0,
		DIO_PUSHPULL = 1,
	};


	/**
	* @enum DIO_TRIGGER_MODE
	* @brief Triggering mode for digital channels
	*
	*/
	enum DIO_TRIGGER_MODE {
		DIO_OR = 0,
		DIO_AND = 1,
	};


	/**
	* @private
	*/
	struct channel {
		struct iio_channel* m_channel; ///< A pointer to an iio_channel structure
		DIO_DIRECTION m_direction; ///< The direction of the channel
	};
}
}


#endif
