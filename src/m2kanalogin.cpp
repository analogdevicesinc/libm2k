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
#include "utils.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <iio.h>
#include <string.h>

using namespace libm2k::analog;
using namespace libm2k::utils;
using namespace std;

M2kAnalogIn::M2kAnalogIn(iio_context * ctx,
			 std::__cxx11::string adc_dev) :
	GenericAnalogIn(ctx, adc_dev),
	m_need_processing(false)
{
	applyM2kFixes();

	setOversamplingRatio(1);
	m_m2k_fabric = iio_context_find_device(m_ctx, "m2k-fabric");
	if (m_m2k_fabric) {
		auto chn0 = iio_device_find_channel(m_m2k_fabric, "voltage0", false);
		auto chn1 = iio_device_find_channel(m_m2k_fabric, "voltage1", false);
		if (chn0 && chn1) {
			m_m2k_fabric_channels.push_back(chn0);
			m_m2k_fabric_channels.push_back(chn1);
		}
	}

	m_ad5625 = iio_context_find_device(m_ctx, "ad5625");
	if (m_ad5625) {
		auto chn0 = iio_device_find_channel(m_ad5625, "voltage2", true);
		auto chn1 = iio_device_find_channel(m_ad5625, "voltage3", true);
		if (chn0 && chn1) {
			m_ad5625_channels.push_back(chn0);
			m_ad5625_channels.push_back(chn1);
		}
	}

	try {
		m_trigger = new M2kHardwareTrigger(m_ctx);
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

	for (unsigned int i = 0; i < m_nb_channels; i++) {
		m_channel_list.push_back(iio_device_find_channel(
			m_dev, "voltage" + i, false));
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

std::vector<std::vector<double>> M2kAnalogIn::getSamples(int nb_samples,
							 bool processed)
{
	try {
		if (processed) {
			m_need_processing = true;
		}
		return GenericAnalogIn::getSamples(nb_samples);
		if (processed) {
			m_need_processing = false;
		}
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
					 getFilterCompensation(getSampleRate()),
					 m_adc_hw_offset.at(channel));
	} else {
		return (double)sample;
	}
}

double M2kAnalogIn::getScalingFactor(M2kAnalogIn::ANALOG_IN_CHANNEL ch)
{
	return (0.78 / ((1 << 11) * 1.3 *
		getValueForRange(m_input_range.at(ch))) *
		m_adc_calib_gain.at(ch) *
		getFilterCompensation(getSampleRate()));
}

void M2kAnalogIn::openAnalogIn()
{
	std::cout << "Opened analog in for " << m_dev_name << "\n";
}

void M2kAnalogIn::closeAnalogIn()
{
	std::cout << "Opened analog in for " << m_dev_name << "\n";
}

void M2kAnalogIn::applyM2kFixes()
{
//	std::string hw_rev = Utils::getHardwareRevision(m_ctx);

//	struct iio_device *dev = iio_context_find_device(m_ctx, "ad9963");

//	int config1 = 0x05;
//	int config2 = 0x05;

//	if (hw_rev == "A") {
//		config1 = 0x1B; // IGAIN1 +-6db  0.25db steps
//		config2 = 0x1B;
//	}

//	/* Configure TX path */
//	iio_device_reg_write(dev, 0x68, config1);
//	iio_device_reg_write(dev, 0x6B, config2);
//	iio_device_reg_write(dev, 0x69, 0x1C);  // IGAIN2 +-2.5%
//	iio_device_reg_write(dev, 0x6C, 0x1C);
//	iio_device_reg_write(dev, 0x6A, 0x20);  // IRSET +-20%
//	iio_device_reg_write(dev, 0x6D, 0x20);
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

unsigned int M2kAnalogIn::numChannels() const
{
	return m_nb_channels;
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

	iio_channel_attr_write_raw(m_m2k_fabric_channels[channel], "gain",
				   str_gain_mode, strlen(str_gain_mode));
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

std::vector<M2kAnalogIn::M2K_RANGE> M2kAnalogIn::getAvailableRanges()
{
	std::vector<M2kAnalogIn::M2K_RANGE> ranges = {};

}

double M2kAnalogIn::getOversamplingRatio()
{
	try {
		return Utils::getDoubleValue(m_dev, "oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::getOversamplingRatio(unsigned int chn_idx)
{
	try {
		return Utils::getDoubleValue(m_dev, chn_idx,
					     "oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::setOversamplingRatio(double oversampling_ratio)
{
	try {
		return Utils::setDoubleValue(m_dev, oversampling_ratio,
					    "oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogIn::setOversamplingRatio(unsigned int chn_idx, double oversampling_ratio)
{
	try {
		return Utils::setDoubleValue(m_dev, chn_idx,
					    oversampling_ratio,
					    "oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

iio_channel *M2kAnalogIn::getChannel(M2kAnalogIn::ANALOG_IN_CHANNEL chn_idx)
{
	if (chn_idx < m_channel_list.size()) {
		return m_channel_list.at(chn_idx);
	} else {
		throw invalid_parameter_exception("No such ADC channel");
	}
}

iio_channel *M2kAnalogIn::getAuxChannel(unsigned int chn_idx)
{
	if (chn_idx < m_ad5625_channels.size()) {
		return m_ad5625_channels.at(chn_idx);
	} else {
		throw invalid_parameter_exception("No such ad5625 channel");
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
