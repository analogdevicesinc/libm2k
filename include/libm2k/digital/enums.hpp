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

#ifndef ENUMS_DIGITAL_HPP
#define ENUMS_DIGITAL_HPP

#include <iio.h>

namespace libm2k {
namespace digital {
	enum DIO_DIRECTION {
		DIO_INPUT = 0,
		DIO_OUTPUT = 1,
	};

	enum DIO_LEVEL {
		LOW = 0,
		HIGH = 1,
	};

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

	enum DIO_MODE {
		DIO_OPENDRAIN = 0,
		DIO_PUSHPULL = 1,
	};

	enum DIO_TRIGGER_MODE {
		DIO_OR = 0,
		DIO_AND = 1,
	};

	struct channel {
		struct iio_channel* m_channel;
		DIO_DIRECTION m_direction;
	};
}
}


#endif
