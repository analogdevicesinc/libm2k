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

#include <libm2k/utils/devicegeneric.hpp>
#include <libm2k/utils/devicein.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>

#include <iostream>
#include <functional>
#include <thread>
#include <chrono>
#include <iio.h>
#include <string.h>

using namespace libm2k::analog;
using namespace libm2k::utils;
using namespace std;
using namespace std::placeholders;

#define HIGH_MAX 2.5
#define HIGH_MIN -2.5
#define LOW_MAX 25
#define LOW_MIN -25

class M2kAnalogIn::M2kAnalogInImpl : public DeviceIn {
public:
	M2kAnalogInImpl(iio_context * ctx, std::string adc_dev, bool sync, M2kHardwareTrigger *trigger) :
		DeviceIn(ctx, adc_dev),
		m_need_processing(false),
		m_trigger(trigger)
	{
		m_m2k_fabric = make_shared<DeviceGeneric>(ctx, "m2k-fabric");
		m_ad5625_dev = std::make_shared<DeviceGeneric>(ctx, "ad5625");

		/* Filters applied while decimating affect the
		/ amplitude of the received  data */
		m_filter_compensation_table[1E8] = 1.00;
		m_filter_compensation_table[1E7] = 1.05;
		m_filter_compensation_table[1E6] = 1.10;
		m_filter_compensation_table[1E5] = 1.15;
		m_filter_compensation_table[1E4] = 1.20;
		m_filter_compensation_table[1E3] = 1.26;

		m_samplerate = 1E8;

		for (unsigned int i = 0; i < getNbChannels(); i++) {
			m_input_range.push_back(PLUS_MINUS_25V);
			m_adc_calib_offset.push_back(2048);
			m_adc_calib_gain.push_back(1);
			m_adc_hw_vert_offset.push_back(0);
			m_adc_hw_vert_offset_raw.push_back(2048);
			m_trigger->setCalibParameters(i, getScalingFactor(i), m_adc_hw_vert_offset.at(i));
		}

		if (sync) {
			syncDevice();
		}

	}

	~M2kAnalogInImpl()
	{
	}

	void enableChannel(unsigned int chn_idx, bool enable)
	{
		DeviceGeneric::enableChannel(chn_idx, enable, false);
	}

	bool isChannelEnabled(unsigned int chn_idx)
	{
		return DeviceGeneric::isChannelEnabled(chn_idx, false);
	}

	unsigned int getNbChannels()
	{
		return DeviceGeneric::getNbChannels(false);
	}

	void init()
	{
		setOversamplingRatio(1);
		setSampleRate(1E8);

		for (unsigned int i = 0; i < getNbChannels(); i++) {
			enableChannel(i, true);

			ANALOG_IN_CHANNEL ch = static_cast<ANALOG_IN_CHANNEL>(i);
			m_trigger->setAnalogMode(ch, ALWAYS);

			setRange(ch, PLUS_MINUS_25V);
			m_adc_calib_offset.at(i) = 2048;
			m_adc_calib_gain.at(i) = 1;
			m_adc_hw_vert_offset.at(i) = 0;
			m_adc_hw_vert_offset_raw.at(i) = 2048;
			m_trigger->setCalibParameters(i, getScalingFactor(i), m_adc_hw_vert_offset.at(i));
		}
		setKernelBuffersCount(1);
	}

	void syncDevice()
	{
		for (unsigned int i = 0; i < getNbChannels(); i++) {
			//enabled???
			ANALOG_IN_CHANNEL ch = static_cast<ANALOG_IN_CHANNEL>(i);
			auto range = getRangeDevice(ch);
			m_input_range[i] = range;
			getVerticalOffset(ch);

			m_trigger->setCalibParameters(i, getScalingFactor(i), m_adc_hw_vert_offset.at(i));
			m_samplerate = getSampleRate();
		}
	}

	void setAdcCalibGain(ANALOG_IN_CHANNEL channel, double gain)
	{
		m_adc_calib_gain[channel] = gain;
		m_trigger->setCalibParameters(channel, getScalingFactor(channel), m_adc_hw_vert_offset.at(channel));
	}

	void setAdcCalibOffset(ANALOG_IN_CHANNEL channel, int raw_offset)
	{
		m_adc_calib_offset[channel] = raw_offset;
		m_trigger->setCalibParameters(channel, getScalingFactor(channel), m_adc_hw_vert_offset.at(channel));
		getVerticalOffset(channel);
	}

	double convRawToVolts(short sample, double correctionGain,
					      double hw_gain, double filterCompensation, double offset)
	{
		// TO DO: explain this formula
		return ((sample * 0.78) / ((1 << 11) * 1.3 * hw_gain) *
			correctionGain * filterCompensation) + offset;
	}

	double convertRawToVolts(unsigned int channel, short sample)
	{
		return convRawToVolts(sample,
					 m_adc_calib_gain.at(channel),
					 getValueForRange(m_input_range.at(channel)),
					 getFilterCompensation(m_samplerate),
					 -m_adc_hw_vert_offset.at(channel));
	}

	short convVoltsToRaw(double voltage, double correctionGain,
					   double hw_gain, double filterCompensation, double offset)
	{
		// TO DO: explain this formula
		return (short)(((voltage - offset) / (correctionGain * filterCompensation) *
				(2048 * 1.3 * hw_gain) / 0.78));
	}

	short convertVoltsToRaw(unsigned int channel, double voltage)
	{
		return convVoltsToRaw(voltage,
					 m_adc_calib_gain.at(channel),
					 getValueForRange(m_input_range.at(channel)),
					 getFilterCompensation(m_samplerate),
					 -m_adc_hw_vert_offset.at(channel));
	}

	double getCalibscale(unsigned int index)
	{
		return getDoubleValue(index, "calibscale");
	}

	double setCalibscale(unsigned int index, double calibscale)
	{
		return setDoubleValue(index, calibscale, "calibscale");
	}

	M2kHardwareTrigger *getTrigger()
	{
		return m_trigger;
	}

	void flushBuffer()
	{
		DeviceIn::flushBuffer();
	}

	/**
	 * @brief Utility method to be used before and after ADC getSamples calls.
	 * For the m2k-adc, all the channels have to be enabled before refilling
	 */
	void handleChannelsEnableState(bool before_refill)
	{
		if (before_refill) {
			bool anyChannelEnabled = false;
			for (unsigned int i = 0; i < getNbChannels(); i++) {
				bool en  = isChannelEnabled(i);
				m_channels_enabled.push_back(en);
				anyChannelEnabled = en ? true : anyChannelEnabled;
			}
			if (!anyChannelEnabled) {
				throw_exception(EXC_INVALID_PARAMETER, "M2kAnalogIn: No channel enabled for RX buffer");
			}

			for (unsigned int i = 0; i < getNbChannels(); i++) {
				enableChannel(i, true);
			}
		} else {
			for (unsigned int i = 0; i < getNbChannels(); i++) {
				enableChannel(i, m_channels_enabled.at(i));
			}
			m_channels_enabled.clear();
		}

	}

	std::vector<std::vector<double>> getSamples(unsigned int nb_samples,
						bool processed = false)
	{
		if (processed) {
			m_need_processing = true;
		}
		m_samplerate = getSampleRate();
		handleChannelsEnableState(true);

		auto fp = std::bind(&M2kAnalogInImpl::processSample, this, _1, _2);
		auto samps = DeviceIn::getSamples(nb_samples, fp);

		handleChannelsEnableState(false);
		if (processed) {
			m_need_processing = false;
		}
		return samps;
	}

	const double *getSamplesInterleaved(unsigned int nb_samples,
						bool processed = false)
	{
		if (processed) {
			m_need_processing = true;
		}
		m_samplerate = getSampleRate();
		handleChannelsEnableState(true);

		auto fp = std::bind(&M2kAnalogInImpl::processSample, this, _1, _2);
		auto samps = (const double *)DeviceIn::getSamplesInterleaved(nb_samples, fp);

		handleChannelsEnableState(false);
		if (processed) {
			m_need_processing = false;
		}
		return samps;
	}

	const short *getSamplesRawInterleaved(unsigned int nb_samples)
	{
		m_samplerate = getSampleRate();
		handleChannelsEnableState(true);
		auto samps = DeviceIn::getSamplesRawInterleaved(nb_samples);
		handleChannelsEnableState(false);
		return samps;
	}

	double processSample(int16_t sample, unsigned int channel)
	{
		if (m_need_processing) {
			return convRawToVolts(sample,
						 m_adc_calib_gain.at(channel),
						 getValueForRange(m_input_range.at(channel)),
						 getFilterCompensation(m_samplerate),
						 -m_adc_hw_vert_offset.at(channel));
		} else {
			return (double)sample;
		}
	}

	short getVoltageRaw(unsigned int ch)
	{
		ANALOG_IN_CHANNEL chn = static_cast<ANALOG_IN_CHANNEL>(ch);
		return getVoltageRaw(chn);
	}

	short getVoltageRaw(ANALOG_IN_CHANNEL ch)
	{
		unsigned int num_samples = 100;
		M2K_TRIGGER_MODE mode;
		bool enabled;

		if (ch >= getNbChannels()) {
			throw_exception(EXC_INVALID_PARAMETER, "M2kAnalogIn: no such channel");
		}

		mode = m_trigger->getAnalogMode(ch);
		m_trigger->setAnalogMode(ch, ALWAYS);

		enabled = isChannelEnabled(ch);
		enableChannel(ch, true);

		auto samps = getSamples(num_samples, false);
		double avg = Utils::average(samps.at(ch).data(), num_samples);

		m_trigger->setAnalogMode(ch, mode);
		enableChannel(ch, enabled);
		return (short)avg;
	}

	std::vector<short> getVoltageRaw()
	{
		unsigned int num_samples = 100;
		std::vector<short> avgs;
		std::vector<M2K_TRIGGER_MODE> modes = {};
		std::vector<bool> enabled = {};

		for (unsigned int i = 0; i < getNbChannels(); i++) {
			enabled.push_back(isChannelEnabled(i));
			enableChannel(i, true);
			modes.push_back(m_trigger->getAnalogMode(i));
			m_trigger->setAnalogMode(i, ALWAYS);
		}
		auto samps = getSamples(num_samples, false);
		for (unsigned int i = 0; i < getNbChannels(); i++) {
			short avg = (short)(Utils::average(samps.at(i).data(), num_samples));
			avgs.push_back(avg);
			m_trigger->setAnalogMode(i, modes.at(i));
			enableChannel(i, enabled.at(i));
		}
		return avgs;
	}

	short *getVoltageRawP()
	{
		std::vector<short> avgs = getVoltageRaw();
		return avgs.data();
	}

	double getVoltage(unsigned int ch)
	{
		ANALOG_IN_CHANNEL chn = static_cast<ANALOG_IN_CHANNEL>(ch);
		return getVoltage(chn);
	}

	double getVoltage(ANALOG_IN_CHANNEL ch)
	{
		unsigned int num_samples = 100;
		M2K_TRIGGER_MODE mode;
		bool enabled;

		if (ch >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "M2kAnalogIn: no such channel");
		}
		mode = m_trigger->getAnalogMode(ch);
		m_trigger->setAnalogMode(ch, ALWAYS);

		enabled = isChannelEnabled(ch);
		enableChannel(ch, true);

		auto samps = getSamples(num_samples, true);
		double avg = Utils::average(samps.at(ch).data(), num_samples);

		m_trigger->setAnalogMode(ch, mode);
		enableChannel(ch, enabled);
		return avg;
	}

	std::vector<double> getVoltage()
	{
		size_t num_samples = 100;
		std::vector<double> avgs;
		std::vector<M2K_TRIGGER_MODE> modes = {};
		std::vector<bool> enabled = {};

		for (unsigned int i = 0; i < getNbChannels(); i++) {
			enabled.push_back(isChannelEnabled(i));
			enableChannel(i, true);
			modes.push_back(m_trigger->getAnalogMode(i));
			m_trigger->setAnalogMode(i, ALWAYS);
		}
		auto samps = getSamples(num_samples, true);
		for (unsigned int i = 0; i < getNbChannels(); i++) {
			avgs.push_back(Utils::average(samps.at(i).data(), num_samples));
			m_trigger->setAnalogMode(i, modes.at(i));
			enableChannel(i, enabled.at(i));
		}
		return avgs;
	}

	double *getVoltageP()
	{
		std::vector<double> avgs = getVoltage();
		return avgs.data();
	}

	double getScalingFactor(ANALOG_IN_CHANNEL ch)
	{
		return (0.78 / ((1 << 11) * 1.3 *
				getValueForRange(m_input_range.at(ch))) *
			m_adc_calib_gain.at(ch) *
			getFilterCompensation(getSampleRate()));
	}

	double getScalingFactor(unsigned int ch)
	{
		ANALOG_IN_CHANNEL channel = static_cast<ANALOG_IN_CHANNEL>(ch);
		return getScalingFactor(channel);
	}

	std::pair<double, double> getHysteresisRange(ANALOG_IN_CHANNEL chn)
	{
		std::pair<double, double> m2k_range = getRangeLimits(getRange(chn));
		return std::pair<double, double>(0, m2k_range.second / 10);
	}

	void setRange(ANALOG_IN_CHANNEL channel, M2K_RANGE range)
	{
		const char *str_gain_mode = "";

		m_input_range[channel] = range;
		if (range == PLUS_MINUS_2_5V) {
			str_gain_mode = "high";
		} else {
			str_gain_mode = "low";
		}

		m_m2k_fabric->setStringValue(channel, "gain", std::string(str_gain_mode));
		m_trigger->setCalibParameters(channel, getScalingFactor(channel), m_adc_hw_vert_offset.at(channel));
	}

	void setRange(ANALOG_IN_CHANNEL channel, double min, double max)
	{

		if ((min >= HIGH_MIN) && (max <= HIGH_MAX)) {
			setRange(channel, PLUS_MINUS_2_5V);
		} else {
			setRange(channel, PLUS_MINUS_25V);
		}
	}

	M2K_RANGE getRange(ANALOG_IN_CHANNEL channel)
	{
		return m_input_range[channel];
	}

	M2K_RANGE getRangeDevice(ANALOG_IN_CHANNEL channel)
	{
		M2K_RANGE range = PLUS_MINUS_25V;
		auto gain = m_m2k_fabric->getStringValue(channel, "gain");
		if (gain == "high") {
			range = PLUS_MINUS_2_5V;
		} else {
			range = PLUS_MINUS_25V;
		}
		return range;
	}

	std::pair<double, double> getRangeLimits(M2K_RANGE range)
	{
		if (range == PLUS_MINUS_25V) {
			return std::pair<double, double>(LOW_MIN, LOW_MAX);
		} else {
			return std::pair<double, double>(HIGH_MIN, HIGH_MAX);
		}
	}

	std::vector<std::pair<std::string, std::pair<double, double>>> getAvailableRanges()
	{
		std::vector<std::pair<std::string, std::pair <double, double>>> ranges;
		std::pair<std::string, std::pair <double, double>> p;
		p.first = "PLUS_MINUS_25V";
		p.second = getRangeLimits(PLUS_MINUS_25V);
		ranges.push_back(p);

		p.first = "PLUS_MINUS_2_5V";
		p.second = getRangeLimits(PLUS_MINUS_2_5V);
		ranges.push_back(p);

		return ranges;
	}


	void setVerticalOffset(ANALOG_IN_CHANNEL channel, double vertOffset)
	{
		double gain = 1.3;
		double vref = 1.2;
		int channel_number = 2 + channel;
		double hw_range_gain = getValueForRange(m_input_range.at(channel));
		int raw_vert_offset = (int) (vertOffset * (1 << 12) * hw_range_gain *
				gain / 2.693 / vref) + m_adc_calib_offset.at(channel);

		m_ad5625_dev->setLongValue(channel_number, raw_vert_offset, "raw", true);
		m_adc_hw_vert_offset_raw.at(channel) = raw_vert_offset;
		m_adc_hw_vert_offset.at(channel) = vertOffset;

		m_trigger->setCalibParameters(channel, getScalingFactor(channel), m_adc_hw_vert_offset.at(channel));
	}

	double getVerticalOffset(ANALOG_IN_CHANNEL channel)
	{
		double gain = 1.3;
		double vref = 1.2;
		int channel_number = 2 + channel;
		double hw_range_gain = getValueForRange(m_input_range.at(channel));
		m_adc_hw_vert_offset_raw.at(channel) = m_ad5625_dev->getLongValue(channel_number, "raw", true);

		double vert_offset = (m_adc_hw_vert_offset_raw.at(channel) - m_adc_calib_offset.at(channel)) *
				2.693 * vref / ((1 << 12) * hw_range_gain * gain);
		m_adc_hw_vert_offset.at(channel) = vert_offset;
		return vert_offset;
	}

	int getOversamplingRatio()
	{
		return getLongValue("oversampling_ratio");
	}

	int getOversamplingRatio(unsigned int chn_idx)
	{
		return getLongValue(chn_idx, "oversampling_ratio");
	}

	int setOversamplingRatio(int oversampling_ratio)
	{
		return setLongValue(oversampling_ratio, "oversampling_ratio");
	}

	int setOversamplingRatio(unsigned int chn_idx, int oversampling_ratio)
	{
		return setLongValue(chn_idx, oversampling_ratio, "oversampling_ratio");
	}


	double getSampleRate()
	{
		return getDoubleValue("sampling_frequency");
	}

	double getSampleRate(unsigned int chn_idx)
	{
		return getDoubleValue(chn_idx, "sampling_frequency");
	}

	double setSampleRate(double samplerate)
	{
		return setDoubleValue(samplerate, "sampling_frequency");
		m_trigger->setCalibParameters(0, getScalingFactor(0), m_adc_hw_vert_offset.at(0));
		m_trigger->setCalibParameters(1, getScalingFactor(1), m_adc_hw_vert_offset.at(1));
	}

	double setSampleRate(unsigned int chn_idx, double samplerate)
	{
		return setDoubleValue(chn_idx, samplerate, "sampling_frequency");
		m_trigger->setCalibParameters(0, getScalingFactor(0), m_adc_hw_vert_offset.at(0));
		m_trigger->setCalibParameters(1, getScalingFactor(1), m_adc_hw_vert_offset.at(1));
	}

	double getFilterCompensation(double samplerate)
	{
		double compensation = 0.0;
		if(m_filter_compensation_table.find(samplerate) != m_filter_compensation_table.end()) {
			compensation = m_filter_compensation_table.at(samplerate);
		} else {
			throw invalid_parameter_exception("Cannot get compensation value for the given samplerate.");
		}
		return compensation;
	}

	double getValueForRange(M2K_RANGE range)
	{
		if (range == PLUS_MINUS_25V) {
			return 0.02017;
		} else if (range == PLUS_MINUS_2_5V) {
			return 0.21229;
		} else {
			return 0;
		}
	}

	struct IIO_OBJECTS getIioObjects()
	{
		return DeviceIn::getIioObjects();
	}

private:
	std::shared_ptr<DeviceGeneric> m_ad5625_dev;
	std::shared_ptr<DeviceGeneric> m_m2k_fabric;
	bool m_need_processing;

	double m_samplerate;
	libm2k::M2kHardwareTrigger *m_trigger;
	std::vector<M2K_RANGE> m_input_range;

	std::vector<double> m_adc_calib_gain;
	std::vector<double> m_adc_calib_offset;
	std::vector<double> m_adc_hw_vert_offset_raw;
	std::vector<double> m_adc_hw_vert_offset;
	std::map<double, double> m_filter_compensation_table;
	std::vector<bool> m_channels_enabled;
};
