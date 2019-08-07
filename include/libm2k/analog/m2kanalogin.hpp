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
#include <libm2k/analog/m2khardwaretrigger.hpp>
#include <vector>
#include <map>
#include <memory>

namespace libm2k {
namespace analog {
class LIBM2K_API M2kAnalogIn
{
public:	
	M2kAnalogIn(struct iio_context*, std::string adc_dev, bool sync);
	virtual ~M2kAnalogIn();
	void init();

	std::vector<std::vector<double>> getSamples(unsigned int nb_samples);
	std::vector<std::vector<double>> getSamplesRaw(unsigned int nb_samples);

	double processSample(int16_t sample, unsigned int channel);

	short getVoltageRaw(unsigned int ch);
	double getVoltage(unsigned int ch);

	short getVoltageRaw(libm2k::analog::ANALOG_IN_CHANNEL ch);
	double getVoltage(libm2k::analog::ANALOG_IN_CHANNEL ch);

	std::vector<short> getVoltageRaw();
	std::vector<double> getVoltage();

	double getScalingFactor(libm2k::analog::ANALOG_IN_CHANNEL ch);

	void setRange(ANALOG_IN_CHANNEL channel, M2K_RANGE range);
	void setRange(ANALOG_IN_CHANNEL channel, double min, double max);
	libm2k::analog::M2K_RANGE getRange(libm2k::analog::ANALOG_IN_CHANNEL channel);
	std::pair<double, double> getRangeLimits(libm2k::analog::M2K_RANGE);
	std::vector<libm2k::analog::M2K_RANGE>  getAvailableRanges();

	double getOversamplingRatio();
	double getOversamplingRatio(unsigned int);
	double setOversamplingRatio(double oversampling);
	double setOversamplingRatio(unsigned int chn_idx, double oversampling);

	double getSampleRate();
	double getSampleRate(unsigned int);
	double setSampleRate(double samplerate);
	double setSampleRate(unsigned int chn_idx, double samplerate);

	std::pair<double, double> getHysteresisRange(ANALOG_IN_CHANNEL);

	double setCalibscale(unsigned int index, double calibscale);
	double getCalibscale(unsigned int index);

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

	unsigned int getNbChannels();
	std::string getName();
	void enableChannel(unsigned int chnIdx, bool enable);
	bool isChannelEnabled(unsigned int chnIdx);

	void convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src);
	void convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src);

	libm2k::analog::M2kHardwareTrigger* getTrigger();
private:
	class M2kAnalogInImpl;
	std::unique_ptr<M2kAnalogInImpl> m_pimpl;
};
}
}
#endif //M2KANALOGIN_HPP
