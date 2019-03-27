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

#include "libm2k/m2kanalogin.hpp"
#include "libm2k/genericanalogin.hpp"
#include "libm2k/m2kexceptions.hpp"
#include "libm2k/utils.hpp"

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

M2kAnalogIn::M2kAnalogIn(iio_context * ctx,
			 std::string adc_dev) :
	Device(ctx, adc_dev),
	m_need_processing(false)
{
	try {
		m_ad9963 = make_shared<Device>(ctx, "ad9963");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(string(e.what()) + " ad9963");
	}

	setOversamplingRatio(1);

	try {
		m_m2k_fabric = make_shared<Device>(ctx, "m2k-fabric");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(string(e.what()) + " m2k-fabric");
	}

	try {
		m_ad5625 = make_shared<Device>(ctx, "ad5625");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(string(e.what()) + " ad5625");
	}

	try {
		m_trigger = new M2kHardwareTrigger(ctx);
	} catch (std::exception& e){
		throw no_device_exception("Disabling hardware trigger support.");
	}

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

std::vector<std::vector<double>> M2kAnalogIn::getSamples(int nb_samples,
							 bool processed)
{
	try {
		if (processed) {
			m_need_processing = true;
		}
		auto fp = std::bind(&M2kAnalogIn::processSample, this, _1, _2);
		auto samps = Device::getSamples(nb_samples, fp);
		if (processed) {
			m_need_processing = false;
		}
		return samps;
	} catch (std::runtime_error &e) {
		throw e;
	}
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
	M2kAnalogIn::ANALOG_IN_CHANNEL chn = static_cast<M2kAnalogIn::ANALOG_IN_CHANNEL>(ch);
	try {
		return getVoltageRaw(chn);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

uint16_t M2kAnalogIn::getVoltageRaw(M2kAnalogIn::ANALOG_IN_CHANNEL ch)
{
	size_t num_samples = 100;
	if (ch >= getNbChannels()) {
		throw invalid_parameter_exception("M2kAnalogIn: no such channel");
	}

	try {
		enableChannel(ch, true);
		auto samps = getSamples(num_samples);
		double avg = Utils::average(samps.at(ch).data(), num_samples);
		return (uint16_t)avg;
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

std::vector<uint16_t> M2kAnalogIn::getVoltageRaw()
{
	size_t num_samples = 100;
	std::vector<uint16_t> avgs;
	try {
		for (unsigned int i = 0; i < getNbChannels(); i++) {
			enableChannel(i, true);
		}
		auto samps = getSamples(num_samples);
		for (unsigned int i = 0; i < getNbChannels(); i++) {
			uint16_t avg = (uint16_t)(Utils::average(samps.at(i).data(), num_samples));
			avgs.push_back(avg);
		}
		return avgs;
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::getVoltage(unsigned int ch)
{
	M2kAnalogIn::ANALOG_IN_CHANNEL chn = static_cast<M2kAnalogIn::ANALOG_IN_CHANNEL>(ch);
	try {
		return getVoltage(chn);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::getVoltage(ANALOG_IN_CHANNEL ch)
{
	size_t num_samples = 100;
	if (ch >= getNbChannels()) {
		throw invalid_parameter_exception("M2kAnalogIn: no such channel");
	}

	try {
		enableChannel(ch, true);
		auto samps = getSamples(num_samples, true);
		double avg = Utils::average(samps.at(ch).data(), num_samples);
		return avg;
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

std::vector<double> M2kAnalogIn::getVoltage()
{
	size_t num_samples = 100;
	std::vector<double> avgs;
	try {
		for (unsigned int i = 0; i < getNbChannels(); i++) {
			enableChannel(i, true);
		}
		auto samps = getSamples(num_samples, true);
		for (unsigned int i = 0; i < getNbChannels(); i++) {
			avgs.push_back(Utils::average(samps.at(i).data(), num_samples));
		}
		return avgs;
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::getScalingFactor(M2kAnalogIn::ANALOG_IN_CHANNEL ch)
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

int M2kAnalogIn::getTriggerDelay()
{
	return m_trigger->getDelay();
}

void M2kAnalogIn::setTriggerDelay(int delay)
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

std::pair<double, double> M2kAnalogIn::getHysteresisRange(M2kAnalogIn::ANALOG_IN_CHANNEL chn)
{
	std::pair<double, double> m2k_range = getRangeLimits(getRange(chn));
	return std::pair<double, double>(0, m2k_range.second / 10);
}

M2kHardwareTrigger::condition M2kAnalogIn::getAnalogCondition(ANALOG_IN_CHANNEL chnIdx) const
{
	try {
		return m_trigger->getAnalogCondition(chnIdx);
	} catch (std::runtime_error &e) {
		throw std::runtime_error("M2K Analog In error: " +
					 std::string(e.what()));
	}
}

void M2kAnalogIn::setAnalogCondition(ANALOG_IN_CHANNEL chnIdx, M2kHardwareTrigger::condition cond)
{
	m_trigger->setAnalogCondition(chnIdx, cond);
}

M2kHardwareTrigger::condition M2kAnalogIn::getDigitalCondition(ANALOG_IN_CHANNEL chnIdx) const
{
	try {
		return m_trigger->getDigitalCondition(chnIdx);
	} catch (std::runtime_error &e) {
		throw std::runtime_error("M2K Analog In error: " +
					 std::string(e.what()));
	}
}

void M2kAnalogIn::setDigitalCondition(ANALOG_IN_CHANNEL chnIdx, M2kHardwareTrigger::condition cond)
{
	m_trigger->setDigitalCondition(chnIdx, cond);
}

M2kHardwareTrigger::source M2kAnalogIn::getSource() const
{
	try {
		return m_trigger->getSource();
	} catch (std::runtime_error &e) {
		throw std::runtime_error("M2K Analog In error: " +
					 std::string(e.what()));
	}
}

void M2kAnalogIn::setSource(M2kHardwareTrigger::source src)
{
	m_trigger->setSource(src);
}

void M2kAnalogIn::setSourceChannel(ANALOG_IN_CHANNEL channel)
{
	try {
		m_trigger->setSourceChannel(channel);
	} catch (std::runtime_error &e) {
		throw std::runtime_error("M2K Analog In error: " +
					 std::string(e.what()));
	}
}

M2kAnalogIn::ANALOG_IN_CHANNEL M2kAnalogIn::getSourceChannel()
{
	int sourceChannel = m_trigger->getSourceChannel();
	if (sourceChannel > 0) {
		return static_cast<ANALOG_IN_CHANNEL>(sourceChannel);
	} else {
		throw std::runtime_error("M2K Analog In error: "
					 " No trigger channel configured");
	}
}

void M2kAnalogIn::setTriggerMode(M2kAnalogIn::ANALOG_IN_CHANNEL channel,
				 M2kHardwareTrigger::mode mode)
{
	try {
		m_trigger->setTriggerMode(channel, mode);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

M2kHardwareTrigger::mode M2kAnalogIn::getTriggerMode(M2kAnalogIn::ANALOG_IN_CHANNEL channel)
{
	try {
		return m_trigger->getTriggerMode(channel);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kAnalogIn::setRange(ANALOG_IN_CHANNEL channel, M2K_RANGE range)
{
	const char *str_gain_mode;

	m_input_range[channel] = range;
	if (range == PLUS_MINUS_5V) {
		str_gain_mode = "high";
	} else {
		str_gain_mode = "low";
	}

	try {
		m_m2k_fabric->setStringValue(channel, "gain", std::string(str_gain_mode));
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kAnalogIn::setRange(ANALOG_IN_CHANNEL channel, double min, double max)
{

	if ((min >= -5) && (max <= 5)) {
		setRange(channel, PLUS_MINUS_5V);
	} else {
		setRange(channel, PLUS_MINUS_25V);
	}
}

M2kAnalogIn::M2K_RANGE M2kAnalogIn::getRange(ANALOG_IN_CHANNEL channel)
{
	return m_input_range[channel];
}

std::pair<double, double> M2kAnalogIn::getRangeLimits(M2kAnalogIn::M2K_RANGE range)
{
	if (range == PLUS_MINUS_25V) {
		return std::pair<double, double>(-25, 25);
	} else {
		return std::pair<double, double>(-5, 5);
	}
}

std::vector<M2kAnalogIn::M2K_RANGE> M2kAnalogIn::getAvailableRanges()
{
	std::vector<M2kAnalogIn::M2K_RANGE> ranges = {};

}

double M2kAnalogIn::getOversamplingRatio()
{
	try {
		return getDoubleValue("oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::getOversamplingRatio(unsigned int chn_idx)
{
	try {
		return getDoubleValue(chn_idx, "oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::setOversamplingRatio(double oversampling_ratio)
{
	try {
		return setDoubleValue(oversampling_ratio, "oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::setOversamplingRatio(unsigned int chn_idx, double oversampling_ratio)
{
	try {
		return setDoubleValue(chn_idx, oversampling_ratio, "oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}


double M2kAnalogIn::getSamplerate()
{
	try {
		return getDoubleValue("sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::getSamplerate(unsigned int chn_idx)
{
	try {
		return getDoubleValue(chn_idx, "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::setSamplerate(double samplerate)
{
	try {
		return setDoubleValue(samplerate, "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::setSamplerate(unsigned int chn_idx, double samplerate)
{
	try {
		return setDoubleValue(chn_idx, samplerate, "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::getFilterCompensation(double samplerate)
{
	return m_filter_compensation_table.at(samplerate);
}

double M2kAnalogIn::getValueForRange(M2K_RANGE range)
{
	if (range == PLUS_MINUS_25V) {
		return 0.02017;
	} else if (range == PLUS_MINUS_5V) {
		return 0.21229;
	} else {
		return 0;
	}
}
