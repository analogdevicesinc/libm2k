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

#include <libm2k/m2kglobal.hpp>
#include <libm2k/context.hpp>
#include <iostream>

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

class LIBM2K_API M2k : public Context
{
public:
	M2k(std::string uri, iio_context* ctx, std::string name, bool sync);
	virtual ~M2k();
	void init();

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

	void setTimeout(unsigned int timeout);
	void setLed(bool on);
	bool getLed();
private:
	class M2kImpl;
	std::shared_ptr<M2kImpl> m_pimpl;
};
}
}
#endif // M2K_H
