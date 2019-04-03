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

#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/genericanalogin.hpp>
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

M2kAnalogIn::M2kAnalogIn(iio_context * ctx,
			 std::string adc_dev) :
	Device(ctx, adc_dev),
	m_need_processing(false),
	m_trigger(nullptr)
{
	m_ad9963 = make_shared<Device>(ctx, "ad9963");
	setOversamplingRatio(1);

	m_m2k_fabric = make_shared<Device>(ctx, "m2k-fabric");
	m_ad5625 = make_shared<Device>(ctx, "ad5625");
	m_trigger = new M2kHardwareTrigger(ctx);

	/* Filters applied while decimating affect the
	/ amplitude of the received  data */
	m_filter_compensation_table[1E8] = 1.00;
	m_filter_compensation_table[1E7] = 1.05;
	m_filter_compensation_table[1E6] = 1.10;
	m_filter_compensation_table[1E5] = 1.15;
	m_filter_compensation_table[1E4] = 1.20;
	m_filter_compensation_table[1E3] = 1.26;

	for (unsigned int i = 0; i < getNbChannels(); i++) {
		m_input_range.push_back(PLUS_MINUS_25V);
		m_adc_calib_offset.push_back(0);
		m_adc_calib_gain.push_back(1);
		m_adc_hw_offset.push_back(0);
	}
}

M2kAnalogIn::~M2kAnalogIn()
{
	delete m_trigger;
}

void M2kAnalogIn::setAdcCalibGain(ANALOG_IN_CHANNEL channel, double gain)
{
	m_adc_calib_gain[channel] = gain;
}

double M2kAnalogIn::convertRawToVolts(int sample, double correctionGain,
		double hw_gain, double filterCompensation, double offset)
{
	// TO DO: explain this formula
	return ((sample * 0.78) / ((1 << 11) * 1.3 * hw_gain) *
		correctionGain * filterCompensation) + offset;
}

int M2kAnalogIn::convertVoltsToRaw(double voltage, double correctionGain,
		double hw_gain, double filterCompensation, double offset)
{
	// TO DO: explain this formula
	return ((voltage - offset) / (correctionGain * filterCompensation) *
		(2048 * 1.3 * hw_gain) / 0.78);
}

void M2kAnalogIn::setStreamingFlag(bool en)
{
	m_trigger->setStreamingFlag(en);
}

bool M2kAnalogIn::getStreamingFlag()
{
	return m_trigger->getStreamingFlag();
}

M2kHardwareTrigger *M2kAnalogIn::getTrigger()
{
	return m_trigger;
}

std::vector<std::vector<double>> M2kAnalogIn::getSamples(int nb_samples,
							 bool processed)
{
	if (processed) {
		m_need_processing = true;
	}
	auto fp = std::bind(&M2kAnalogIn::processSample, this, _1, _2);
	auto samps = Device::getSamples(nb_samples, fp);
	if (processed) {
		m_need_processing = false;
	}
	return samps;
}

double M2kAnalogIn::processSample(int16_t sample, unsigned int channel)
{
	if (m_need_processing) {
		return convertRawToVolts(sample,
					 m_adc_calib_gain.at(channel),
					 getValueForRange(m_input_range.at(channel)),
					 getFilterCompensation(getSamplerate()),
					 m_adc_hw_offset.at(channel));
	} else {
		return (double)sample;
	}
}

uint16_t M2kAnalogIn::getVoltageRaw(unsigned int ch)
{
	ANALOG_IN_CHANNEL chn = static_cast<ANALOG_IN_CHANNEL>(ch);
	return getVoltageRaw(chn);
}

uint16_t M2kAnalogIn::getVoltageRaw(ANALOG_IN_CHANNEL ch)
{
	size_t num_samples = 100;
	if (ch >= getNbChannels()) {
		throw_exception(EXC_INVALID_PARAMETER, "M2kAnalogIn: no such channel");
	}

	enableChannel(ch, true);
	auto samps = getSamples(num_samples);
	double avg = Utils::average(samps.at(ch).data(), num_samples);
	return (uint16_t)avg;
}

std::vector<uint16_t> M2kAnalogIn::getVoltageRaw()
{
	size_t num_samples = 100;
	std::vector<uint16_t> avgs;
	for (unsigned int i = 0; i < getNbChannels(); i++) {
		enableChannel(i, true);
	}
	auto samps = getSamples(num_samples);
	for (unsigned int i = 0; i < getNbChannels(); i++) {
		uint16_t avg = (uint16_t)(Utils::average(samps.at(i).data(), num_samples));
		avgs.push_back(avg);
	}
	return avgs;
}

double M2kAnalogIn::getVoltage(unsigned int ch)
{
	ANALOG_IN_CHANNEL chn = static_cast<ANALOG_IN_CHANNEL>(ch);
	return getVoltage(chn);
}

double M2kAnalogIn::getVoltage(ANALOG_IN_CHANNEL ch)
{
	size_t num_samples = 100;
	if (ch >= getNbChannels()) {
		throw_exception(EXC_OUT_OF_RANGE, "M2kAnalogIn: no such channel");
	}

	enableChannel(ch, true);
	auto samps = getSamples(num_samples, true);
	double avg = Utils::average(samps.at(ch).data(), num_samples);
	return avg;
}

std::vector<double> M2kAnalogIn::getVoltage()
{
	size_t num_samples = 100;
	std::vector<double> avgs;

	for (unsigned int i = 0; i < getNbChannels(); i++) {
		enableChannel(i, true);
	}
	auto samps = getSamples(num_samples, true);
	for (unsigned int i = 0; i < getNbChannels(); i++) {
		avgs.push_back(Utils::average(samps.at(i).data(), num_samples));
	}
	return avgs;
}

double M2kAnalogIn::getScalingFactor(ANALOG_IN_CHANNEL ch)
{
	return (0.78 / ((1 << 11) * 1.3 *
		getValueForRange(m_input_range.at(ch))) *
		m_adc_calib_gain.at(ch) *
		getFilterCompensation(getSamplerate()));
}

void M2kAnalogIn::openAnalogIn()
{
	std::cout << "Opened analog in for " << getName() << "\n";
}

void M2kAnalogIn::closeAnalogIn()
{
	std::cout << "Opened analog in for " << getName() << "\n";
}

int M2kAnalogIn::getDelay()
{
	return m_trigger->getDelay();
}

void M2kAnalogIn::setDelay(int delay)
{
	m_trigger->setDelay(delay);
}

int M2kAnalogIn::getLevel(ANALOG_IN_CHANNEL chnIdx) const
{
	return m_trigger->getLevel(chnIdx);
}

void M2kAnalogIn::setLevel(ANALOG_IN_CHANNEL chnIdx, int level)
{
	m_trigger->setLevel(chnIdx, level);
}

int M2kAnalogIn::getHysteresis(ANALOG_IN_CHANNEL chnIdx) const
{
	return m_trigger->getHysteresis(chnIdx);
}

void M2kAnalogIn::setHysteresis(ANALOG_IN_CHANNEL chnIdx, int hysteresis)
{
	m_trigger->setHysteresis(chnIdx, hysteresis);
}

std::pair<double, double> M2kAnalogIn::getHysteresisRange(ANALOG_IN_CHANNEL chn)
{
	std::pair<double, double> m2k_range = getRangeLimits(getRange(chn));
	return std::pair<double, double>(0, m2k_range.second / 10);
}

M2K_TRIGGER_CONDITION M2kAnalogIn::getAnalogCondition(ANALOG_IN_CHANNEL chnIdx) const
{
	__try {
		return m_trigger->getAnalogCondition(chnIdx);
	} __catch (exception_type &e) {
		throw_exception(EXC_RUNTIME_ERROR, "M2KAnalogIn trigger condition error: "  +
				string(e.what()));
	}
}

void M2kAnalogIn::setAnalogCondition(ANALOG_IN_CHANNEL chnIdx, M2K_TRIGGER_CONDITION cond)
{
	m_trigger->setAnalogCondition(chnIdx, cond);
}

M2K_TRIGGER_CONDITION M2kAnalogIn::getDigitalCondition(ANALOG_IN_CHANNEL chnIdx) const
{
	__try {
		return m_trigger->getDigitalCondition(chnIdx);
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, "M2KAnalogIn trigger condition error: "  +
				string(e.what()));
	}
}

void M2kAnalogIn::setDigitalCondition(ANALOG_IN_CHANNEL chnIdx, M2K_TRIGGER_CONDITION cond)
{
	m_trigger->setDigitalCondition(chnIdx, cond);
}

M2K_TRIGGER_SOURCE M2kAnalogIn::getSource() const
{
	__try {
		return m_trigger->getSource();
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, "M2KAnalogIn trigger source error: "  +
				string(e.what()));
	}
}

void M2kAnalogIn::setSource(M2K_TRIGGER_SOURCE src)
{
	m_trigger->setSource(src);
}

void M2kAnalogIn::setSourceChannel(ANALOG_IN_CHANNEL channel)
{
	__try {
		m_trigger->setSourceChannel(channel);
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, "M2KAnalogIn set trigger source error: "  +
				string(e.what()));
	}
}

ANALOG_IN_CHANNEL M2kAnalogIn::getSourceChannel()
{
	int sourceChannel = m_trigger->getSourceChannel();
	if (sourceChannel > 0) {
		return static_cast<ANALOG_IN_CHANNEL>(sourceChannel);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, "M2KAnalogIn trigger source channel error: "  +
				string(e.what()));
	}
}

void M2kAnalogIn::setTriggerMode(ANALOG_IN_CHANNEL channel,
				 M2K_TRIGGER_MODE mode)
{
	__try {
		m_trigger->setTriggerMode(channel, mode);
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, "M2KAnalogIn set trigger mode error: "  +
				string(e.what()));
	}
}

M2K_TRIGGER_MODE M2kAnalogIn::getTriggerMode(ANALOG_IN_CHANNEL channel)
{
	__try {
		return m_trigger->getTriggerMode(channel);
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, "M2KAnalogIn trigger mode error: "  +
				string(e.what()));
	}
}

void M2kAnalogIn::setRange(ANALOG_IN_CHANNEL channel, M2K_RANGE range)
{
	const char *str_gain_mode = "";

	m_input_range[channel] = range;
	if (range == PLUS_MINUS_2_5V) {
		str_gain_mode = "high";
	} else {
		str_gain_mode = "low";
	}

	m_m2k_fabric->setStringValue(channel, "gain", std::string(str_gain_mode));
}

void M2kAnalogIn::setRange(ANALOG_IN_CHANNEL channel, double min, double max)
{

	if ((min >= HIGH_MIN) && (max <= HIGH_MAX)) {
		setRange(channel, PLUS_MINUS_2_5V);
	} else {
		setRange(channel, PLUS_MINUS_25V);
	}
}

M2K_RANGE M2kAnalogIn::getRange(ANALOG_IN_CHANNEL channel)
{
	return m_input_range[channel];
}

std::pair<double, double> M2kAnalogIn::getRangeLimits(M2K_RANGE range)
{
	if (range == PLUS_MINUS_25V) {
		return std::pair<double, double>(LOW_MIN, LOW_MAX);
	} else {
		return std::pair<double, double>(HIGH_MIN, HIGH_MAX);
	}
}

std::vector<M2K_RANGE> M2kAnalogIn::getAvailableRanges()
{
	std::vector<M2K_RANGE> ranges = {};

}

double M2kAnalogIn::getOversamplingRatio()
{
	return getDoubleValue("oversampling_ratio");
}

double M2kAnalogIn::getOversamplingRatio(unsigned int chn_idx)
{
	return getDoubleValue(chn_idx, "oversampling_ratio");
}

double M2kAnalogIn::setOversamplingRatio(double oversampling_ratio)
{
	return setDoubleValue(oversampling_ratio, "oversampling_ratio");
}

double M2kAnalogIn::setOversamplingRatio(unsigned int chn_idx, double oversampling_ratio)
{
	return setDoubleValue(chn_idx, oversampling_ratio, "oversampling_ratio");
}


double M2kAnalogIn::getSamplerate()
{
	return getDoubleValue("sampling_frequency");
}

double M2kAnalogIn::getSamplerate(unsigned int chn_idx)
{
	return getDoubleValue(chn_idx, "sampling_frequency");
}

double M2kAnalogIn::setSamplerate(double samplerate)
{
	return setDoubleValue(samplerate, "sampling_frequency");
}

double M2kAnalogIn::setSamplerate(unsigned int chn_idx, double samplerate)
{
	return setDoubleValue(chn_idx, samplerate, "sampling_frequency");
}

double M2kAnalogIn::getFilterCompensation(double samplerate)
{
	return m_filter_compensation_table.at(samplerate);
}

double M2kAnalogIn::getValueForRange(M2K_RANGE range)
{
	if (range == PLUS_MINUS_25V) {
		return 0.02017;
	} else if (range == PLUS_MINUS_2_5V) {
		return 0.21229;
	} else {
		return 0;
	}
}
