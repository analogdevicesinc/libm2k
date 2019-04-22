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

#include <libm2k/m2kglobal.hpp>
#include <libm2k/analog/enums.hpp>
#include <libm2k/analog/genericanalogin.hpp>
#include <libm2k/analog/m2khardwaretrigger.hpp>
#include <vector>
#include <map>
#include <memory>

using namespace libm2k::utils;
namespace libm2k {
namespace analog {
class LIBM2K_API M2kAnalogIn : public Device
{
public:	
	M2kAnalogIn(struct iio_context*, std::string adc_dev);
	virtual ~M2kAnalogIn();

	std::vector<std::vector<double>> getSamples(unsigned int nb_samples,
						    bool processed=false);
	void openAnalogIn();
	void closeAnalogIn();

	double processSample(int16_t sample, unsigned int channel);

	uint16_t getVoltageRaw(unsigned int ch);
	double getVoltage(unsigned int ch);

	uint16_t getVoltageRaw(ANALOG_IN_CHANNEL ch);
	double getVoltage(ANALOG_IN_CHANNEL ch);

	std::vector<uint16_t> getVoltageRaw();
	std::vector<double> getVoltage();

	double getScalingFactor(ANALOG_IN_CHANNEL ch);

	void setRange(ANALOG_IN_CHANNEL channel, M2K_RANGE range);
	void setRange(ANALOG_IN_CHANNEL channel, double min, double max);
	M2K_RANGE getRange(ANALOG_IN_CHANNEL channel);
	std::pair<double, double> getRangeLimits(M2K_RANGE);
	std::vector<M2K_RANGE>  getAvailableRanges();

	double getOversamplingRatio();
	double getOversamplingRatio(unsigned int);
	double setOversamplingRatio(double oversampling);
	double setOversamplingRatio(unsigned int chn_idx, double oversampling);

	double getSamplerate();
	double getSamplerate(unsigned int);
	double setSamplerate(double samplerate);
	double setSamplerate(unsigned int chn_idx, double samplerate);

	int getLevel(ANALOG_IN_CHANNEL chnIdx) const;
	void setLevel(ANALOG_IN_CHANNEL chnIdx, int level);

	int getHysteresis(ANALOG_IN_CHANNEL chnIdx) const;
	void setHysteresis(ANALOG_IN_CHANNEL chnIdx, int histeresis);
	std::pair<double, double> getHysteresisRange(ANALOG_IN_CHANNEL);


	M2K_TRIGGER_CONDITION getAnalogCondition(ANALOG_IN_CHANNEL chnIdx) const;
	void setAnalogCondition(ANALOG_IN_CHANNEL chnIdx, M2K_TRIGGER_CONDITION cond);

	M2K_TRIGGER_CONDITION getDigitalCondition(ANALOG_IN_CHANNEL chnIdx) const;
	void setDigitalCondition(ANALOG_IN_CHANNEL chnIdx, M2K_TRIGGER_CONDITION cond);

	M2K_TRIGGER_MODE getTriggerMode(ANALOG_IN_CHANNEL chnIdx);
	void setTriggerMode(ANALOG_IN_CHANNEL chnIdx, M2K_TRIGGER_MODE mode);

	M2K_TRIGGER_SOURCE getSource() const;
	void setSource(M2K_TRIGGER_SOURCE src);

	ANALOG_IN_CHANNEL getSourceChannel();
	void setSourceChannel(ANALOG_IN_CHANNEL chnIdx);

	int getDelay();
	void setDelay(int delay);

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

	void setStreamingFlag(bool);
	bool getStreamingFlag();

	M2kHardwareTrigger* getTrigger();
private:
	class M2kAnalogInImpl;
	std::shared_ptr<M2kAnalogInImpl> m_pimpl;
};
}
}
#endif //M2KANALOGIN_HPP
