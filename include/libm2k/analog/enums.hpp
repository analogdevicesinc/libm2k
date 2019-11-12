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

/**
 * @file analog/enums.hpp
 * @brief Analogical enumerations
 */

namespace libm2k {
namespace analog {


	/**
	* @struct DMM_READING
	* @brief The structure of a DMM
	*
	*/
	struct DMM_READING {
		std::string name; ///< The name of the channel
		std::string id; ///< Channel's id
		double value; ///< The measured value
		std::string unit; ///< Unit of measurement
	};


	/**
	* @enum ANALOG_IN_CHANNEL
	* @brief Indexes of the channels
	*
	* Each channel can be accessed through its index
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


	/**
	* @enum M2K_TRIGGER_CONDITION_ANALOG for the analog side.
	* @brief Condition of triggering.
	*
	*/
	enum M2K_TRIGGER_CONDITION_ANALOG {
		RISING_EDGE_ANALOG = 0,
		FALLING_EDGE_ANALOG = 1,
		LOW_LEVEL_ANALOG = 2,
		HIGH_LEVEL_ANALOG = 3
	};

	/**
	* @enum M2K_TRIGGER_CONDITION_DIGITAL for the digital side.
	* @brief Condition of triggering.
	*
	*/
	enum M2K_TRIGGER_CONDITION_DIGITAL {
		RISING_EDGE_DIGITAL = 0,
		FALLING_EDGE_DIGITAL = 1,
		LOW_LEVEL_DIGITAL = 2,
		HIGH_LEVEL_DIGITAL = 3,
		ANY_EDGE_DIGITAL = 4,
		NO_TRIGGER_DIGITAL = 5,
	};

	/**
	* @enum M2K_TRIGGER_MODE
	* @brief Triggering mode
	*
	*/
	enum M2K_TRIGGER_MODE {
		ALWAYS = 0,
		ANALOG = 1,
		EXTERNAL = 2,
		DIGITAL_OR_ANALOG = 3,
		DIGITAL_AND_ANALOG = 4,
		DIGITAL_XOR_ANALOG = 5,
		N_DIGITAL_OR_ANALOG = 6,
		N_DIGITAL_AND_ANALOG = 7,
		N_DIGITAL_XOR_ANALOG = 8,
	};


	/**
	* @enum M2K_TRIGGER_ANALOG_SOURCE
	* @brief The source of triggering
	*
	*/
	enum M2K_TRIGGER_SOURCE_ANALOG {
		CHANNEL_1 = 0,
		CHANNEL_2 = 1,
		CHANNEL_1_OR_CHANNEL_2 = 2,
		CHANNEL_1_AND_CHANNEL_2 = 3,
		CHANNEL_1_XOR_CHANNEL_2 = 4,
		SRC_DIGITAL_IN = 5,
		CHANNEL_1_OR_SRC_LOGIC_ANALYZER = 6,
		CHANNEL_2_OR_SRC_LOGIC_ANALYZER = 7,
		CHANNEL_1_OR_CHANNEL_2_OR_SRC_LOGIC_ANALYZER = 8,
	};

	enum M2K_TRIGGER_SOURCE_DIGITAL {
		SRC_TRIGGER_IN = 0,
		SRC_ANALOG_IN = 1,
		SRC_NONE = 2
	};


	/**
	 * @enum M2K_TRIGGER_OUT_SELECT
	 * @brief select
	 */

	enum M2K_TRIGGER_OUT_SELECT {
		SELECT_NONE = 0,
		SELECT_TRIGGER_I_SAME_CHAN = 1,
		SELECT_TRIGGER_IN = 2,
		SELECT_ANALOG_IN = 3,
		SELECT_DIGITAL_IN = 4
	};


	/**
	* @struct SETTINGS
	* @brief Triggering system
	*
	*/
	struct SETTINGS {
		std::vector<M2K_TRIGGER_CONDITION_ANALOG> analog_condition; ///< Analogical trigger's condition
		std::vector<M2K_TRIGGER_CONDITION_DIGITAL> digital_condition; ///< Digital trigger's condition
		std::vector<int> raw_level; ///< Trigger's raw level
		std::vector<double> level; ///< Trigger's level
		std::vector<int> hysteresis; ///< Trigger's hysteresis
		std::vector<M2K_TRIGGER_MODE> mode; ///<Triggering mode
		M2K_TRIGGER_SOURCE_ANALOG trigger_source; ///< Triggering source
		int delay; ///< Trigger's delay
	};
}
}


#endif
