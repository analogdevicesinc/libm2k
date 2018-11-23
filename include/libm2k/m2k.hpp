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

#ifndef M2K_H
#define M2K_H

#include "libm2k/m2kglobal.hpp"
#include "libm2k/genericdevice.hpp"
#include <iostream>

extern "C" {
	struct iio_context;
}

namespace libm2k {
namespace analog {
class M2kAnalogIn;
class M2kAnalogOut;
class M2kPowerSupply;
}

namespace digital {
class M2kDigital;
}

class M2kCalibration;
namespace devices {

class LIBM2K_API M2K : public libm2k::devices::GenericDevice
{
public:
	M2K(std::string uri, iio_context* ctx, std::string name);
	~M2K();
	void scanAllAnalogIn();
	void scanAllAnalogOut();
	void scanAllPowerSupply();
	void scanAllDigital();

	void calibrate();
	bool calibrateADC();
	bool calibrateDAC();
	bool resetCalibration();

	std::shared_ptr<libm2k::digital::M2kDigital> getDigital();
	std::shared_ptr<libm2k::analog::M2kPowerSupply> getPowerSupply();
	std::shared_ptr<libm2k::analog::M2kAnalogIn> getAnalogIn(unsigned int index);
	std::shared_ptr<libm2k::analog::M2kAnalogIn> getAnalogIn(std::string);
	std::shared_ptr<libm2k::analog::M2kAnalogOut> getAnalogOut(std::string dev_name);
	std::vector<std::shared_ptr<libm2k::analog::M2kAnalogIn>> getAllAnalogIn();
	std::vector<std::shared_ptr<libm2k::analog::M2kAnalogOut>> getAllAnalogOut();
	int getDacBCalibrationOffset();
	int getDacACalibrationOffset();
	double getDacBCalibrationGain();
	double getDacACalibrationGain();
	int getAdcCalibrationOffset(unsigned int chn);
	double getAdcCalibrationGain(unsigned int chn);
private:
	M2kCalibration* m_calibration;
	void initialize();
};
}
}
#endif // M2K_H
