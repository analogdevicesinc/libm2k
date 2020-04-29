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

#ifndef M2KCALIBRATION_HPP
#define M2KCALIBRATION_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/m2khardwaretrigger.hpp>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <memory>
#include <vector>

namespace libm2k {

class LIBM2K_API M2kCalibration
{
public:
	virtual ~M2kCalibration() {}

	virtual bool initialize() = 0;
	virtual bool isInitialized() const = 0;

	virtual bool calibrateAll() = 0;
	virtual bool calibrateADC() = 0;
	virtual bool calibrateDAC() = 0;
	virtual void cancelCalibration() = 0;

	virtual int getAdcOffset(unsigned int channel) = 0;
	virtual int getDacOffset(unsigned int channel) = 0;
	virtual double getAdcGain(unsigned int channel) = 0;
	virtual double getDacGain(unsigned int channel) = 0;


	virtual bool resetCalibration() = 0;

	virtual void setDacGain(unsigned int chn, double gain) = 0;
	virtual void setDacOffset(unsigned int chn, int offset) = 0;
	virtual void setAdcOffset(unsigned int chn, int offset) = 0;
	virtual void setAdcGain(unsigned int chn, double gain) = 0;
};

}

#endif /* M2KCALIBRATION_HPP */
