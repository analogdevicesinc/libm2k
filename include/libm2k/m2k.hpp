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
#include "libm2k/device.hpp"
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

	libm2k::digital::M2kDigital* getDigital();
	libm2k::analog::M2kPowerSupply* getPowerSupply();
	libm2k::analog::M2kAnalogIn* getAnalogIn();
	libm2k::analog::M2kAnalogIn* getAnalogIn(std::string);
	libm2k::analog::M2kAnalogOut* getAnalogOut();
	std::vector<libm2k::analog::M2kAnalogIn*> getAllAnalogIn();
	std::vector<libm2k::analog::M2kAnalogOut*> getAllAnalogOut();
	int getDacBCalibrationOffset();
	int getDacACalibrationOffset();
	double getDacBCalibrationGain();
	double getDacACalibrationGain();
	int getAdcCalibrationOffset(unsigned int chn);
	double getAdcCalibrationGain(unsigned int chn);
private:
	M2kCalibration* m_calibration;
	std::vector<utils::Device*> m_instancesAnalogOut;
	std::vector<utils::Device*> m_instancesAnalogIn;
	void initialize();
};
}
}
#endif // M2K_H
