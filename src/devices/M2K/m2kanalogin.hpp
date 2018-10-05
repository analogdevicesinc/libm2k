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

#ifndef M2KANALOGIN_HPP
#define M2KANALOGIN_HPP

#include "libm2k/m2kglobal.hpp"
#include "libm2k/genericanalogin.hpp"
#include <iio.h>
#include <vector>

namespace libm2k {
namespace analog {
class LIBM2K_API M2kAnalogIn : public GenericAnalogIn
{
public:	
	enum ANALOG_IN_CHANNEL {
		ANALOG_IN_CHANNEL_1 = 1,
		ANALOG_IN_CHANNEL_2 = 2
	};

	enum GAIN_CONFIG {
		LOW_GAIN = 0,
		HIGH_GAIN = 1
	};

	enum M2K_RANGES {
		PLUS_MINUS_5V = 1,
		PLUS_MINUS_25V = 0
	};


	M2kAnalogIn(struct iio_context*, std::string adc_dev);
	~M2kAnalogIn();

	double* getSamples(int nb_samples);
	void openAnalogIn();
	void closeAnalogIn();

	void calibrate(bool async = true);
	uint16_t getVoltageRaw(ANALOG_IN_CHANNEL ch);
	double getVoltage(ANALOG_IN_CHANNEL ch);
	double getScalingFactor(ANALOG_IN_CHANNEL ch);
	void setGain(ANALOG_IN_CHANNEL ch, GAIN_CONFIG gain);
	GAIN_CONFIG getGain(ANALOG_IN_CHANNEL ch);
	void setRange(M2K_RANGES range);
	void setRange(double min, double max);
	M2K_RANGES getRange();

	//trigger channel?
//	double getSampleRate();
//	double setSampleRate(double sampleRate);
	double getTimeTrigger();
	void setTimeTrigger(double percent);
	double getVoltTrigger();
	void setVoltTrigger(double volt);
	void setEnableTrigger(bool en);
	bool getEnableTrigger();

private:
	double convertRawToVolts(int);
	int convertVoltsToRaw(double);
};
}
}
#endif //M2KANALOGIN_HPP
