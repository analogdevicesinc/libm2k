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

#ifndef ENUMS_ANALOG_HPP
#define ENUMS_ANALOG_HPP

#include <vector>
#include <string>
#include <memory>

namespace libm2k {
namespace analog {
	struct DMM_READING {
		std::string name;
		std::string id;
		double value;
		std::string unit;
	};

	enum ANALOG_IN_CHANNEL {
		ANALOG_IN_CHANNEL_1 = 0,
		ANALOG_IN_CHANNEL_2 = 1
	};

	enum M2K_RANGE {
		PLUS_MINUS_25V = 0,
		PLUS_MINUS_2_5V = 1
	};

	enum M2K_TRIGGER_CONDITION {
		RISING_EDGE = 0,
		FALLING_EDGE = 1,
		LOW_LEVEL = 3,
		HIGH_LEVEL = 4,
		ANY_EDGE = 5,
		NO_TRIGGER = 6,
	};

	enum M2K_TRIGGER_MODE {
		ALWAYS = 0,
		ANALOG = 1,
		DIGITAL = 2,
		DIGITAL_OR_ANALOG = 3,
		DIGITAL_AND_ANALOG = 4,
		DIGITAL_XOR_ANALOG = 5,
		N_DIGITAL_OR_ANALOG = 6,
		N_DIGITAL_AND_ANALOG = 7,
		N_DIGITAL_XOR_ANALOG = 8,
	};

	enum M2K_TRIGGER_SOURCE {
		CHANNEL_1 = 0,
		CHANNEL_2 = 1,
		CHANNEL_1_OR_CHANNEL_2 = 2,
		CHANNEL_1_AND_CHANNEL_2 = 3,
		CHANNEL_1_XOR_CHANNEL_2 = 4,
	};

	struct SETTINGS {
		std::vector<M2K_TRIGGER_CONDITION> analog_condition;
		std::vector<M2K_TRIGGER_CONDITION> digital_condition;
		std::vector<int> raw_level;
		std::vector<double> level;
		std::vector<int> hysteresis;
		std::vector<M2K_TRIGGER_MODE> mode;
		M2K_TRIGGER_SOURCE trigger_source;
		int delay;
	};

	typedef std::shared_ptr<SETTINGS> settings_uptr;
}
}


#endif
