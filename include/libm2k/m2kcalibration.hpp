/*
 * Copyright 2016 Analog Devices, Inc.
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

#ifndef M2KCALIBRATION_HPP
#define M2KCALIBRATION_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/analog/enums.hpp>
#include <libm2k/analog/m2khardwaretrigger.hpp>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <memory>
#include <vector>

namespace libm2k {

class LIBM2K_API M2kCalibration
{
public:
	M2kCalibration(struct iio_context* ctx, libm2k::analog::M2kAnalogIn* analogIn,
		       libm2k::analog::M2kAnalogOut* analogOut);
	virtual ~M2kCalibration();

	bool initialize();
	bool isInitialized() const;

	void setAdcInCalibMode();
	void setDacInCalibMode();
	void restoreAdcFromCalibMode();
	void restoreDacFromCalibMode();

	bool calibrateAll();
	bool calibrateADC();
	bool calibrateADCoffset();
	bool calibrateADCgain();
	bool calibrateDAC();
	bool calibrateDACoffset();
	bool calibrateDACgain();
	void cancelCalibration();

	int adcOffsetChannel0() const;
	int adcOffsetChannel1() const;
	int dacAoffset() const;
	int dacBoffset() const;
	double adcGainChannel0() const;
	double adcGainChannel1() const;
	double dacAvlsb() const;
	double dacBvlsb() const;

	bool resetCalibration();
	void updateAdcCorrections();
	void updateDacCorrections();

	bool setCalibrationMode(int);

protected:
	class M2kCalibrationImpl;
	std::unique_ptr<M2kCalibrationImpl> m_pimpl;
};

}

#endif /* M2KCALIBRATION_HPP */
