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

#ifndef ENUMS_HPP
#define ENUMS_HPP

#include <string>
#include <vector>

namespace libm2k {
	enum CALIBRATION_MODE {
		ADC_REF1,
		ADC_REF2,
		ADC_GND,
		DAC,
		NONE
	};

	enum GAIN_MODE {
		LOW_GAIN,
		HIGH_GAIN
	};

	enum M2K_EXCEPTION {
		EXC_OUT_OF_RANGE,
		EXC_RUNTIME_ERROR,
		EXC_INVALID_PARAMETER,
		EXC_TIMEOUT
	};
}


#endif
