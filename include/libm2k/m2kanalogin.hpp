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
#include "m2khardwaretrigger.hpp"
#include <vector>
#include <map>

extern "C" {
	struct iio_context;
	struct iio_device;
	struct iio_channel;
}

namespace libm2k {
namespace analog {
class LIBM2K_API M2kAnalogIn : public GenericAnalogIn
{
public:	
	enum ANALOG_IN_CHANNEL {
		ANALOG_IN_CHANNEL_1 = 0,
		ANALOG_IN_CHANNEL_2 = 1
	};

	enum M2K_RANGE {
		PLUS_MINUS_25V = 0,
		PLUS_MINUS_5V = 1 // should be 2.5 instead of 5V?
	};


	M2kAnalogIn(struct iio_context*, std::string adc_dev);
	~M2kAnalogIn();

	std::vector<std::vector<double>> getSamples(int nb_samples,
						    bool processed=false);
//	std::vector<std::vector<double>> getProcessedSamples(int nb_samples);
	void openAnalogIn();
	void closeAnalogIn();

	double processSample(int16_t sample, unsigned int channel);

	uint16_t getVoltageRaw(ANALOG_IN_CHANNEL ch);
	double getVoltage(ANALOG_IN_CHANNEL ch);

	double getScalingFactor(ANALOG_IN_CHANNEL ch);

	void setRange(ANALOG_IN_CHANNEL channel, M2K_RANGE range);
	void setRange(ANALOG_IN_CHANNEL channel, double min, double max);
	M2K_RANGE getRange(ANALOG_IN_CHANNEL channel);
	std::vector<M2K_RANGE>  getAvailableRanges();

	double getOversamplingRatio();
	double getOversamplingRatio(unsigned int);
	double setOversamplingRatio(double oversampling);
	double setOversamplingRatio(unsigned int chn_idx, double oversampling);

	struct iio_channel* getChannel(ANALOG_IN_CHANNEL);
	struct iio_channel* getAuxChannel(unsigned int);

	//trigger channel?
	int getTriggerDelay();
	void setTriggerDelay(int delay);
	void setTriggerDelay(double percent);

	double getTriggerOffset();
	void setTriggerOffset(double volt);

	void setEnableTrigger(bool en);
	bool getEnableTrigger();

	void setTriggerChannel(ANALOG_IN_CHANNEL channel);
	ANALOG_IN_CHANNEL getTriggerChannel();

	void setTriggerMode(ANALOG_IN_CHANNEL,
		libm2k::analog::M2kHardwareTrigger::mode);
	libm2k::analog::M2kHardwareTrigger::mode getTriggerMode(ANALOG_IN_CHANNEL);


	void setAdcCalibGain(ANALOG_IN_CHANNEL channel, double gain);
	double getFilterCompensation(double samplerate);
	double getValueForRange(M2K_RANGE);

	double convertRawToVolts(int sample, double correctionGain = 1,
				 double hw_gain = 0.02,
				 double filterCompensation = 1,
				 double offset = 0);
	int convertVoltsToRaw(double voltage, double correctionGain = 1,
			      double hw_gain = 0.02,
			      double filterCompensation = 1,
			      double offset = 0);

private:
	void applyM2kFixes();
	bool m_need_processing;

	libm2k::analog::M2kHardwareTrigger *m_trigger;
	std::vector<M2K_RANGE> m_input_range;

	struct iio_device* m_m2k_fabric;
	struct iio_device* m_ad5625;
	std::vector<struct iio_channel*> m_m2k_fabric_channels;
	std::vector<struct iio_channel*> m_ad5625_channels;
	std::vector<double> m_adc_calib_gain;
	std::vector<double> m_adc_calib_offset;
	std::vector<double> m_adc_hw_offset;

	std::map<double, double> m_filter_compensation_table;
};
}
}
#endif //M2KANALOGIN_HPP
