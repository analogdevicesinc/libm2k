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
#include <string.h>

using namespace libm2k::analog;
using namespace libm2k::utils;
using namespace std;

M2kAnalogIn::M2kAnalogIn(iio_context * ctx,
			 std::__cxx11::string adc_dev) :
	GenericAnalogIn(ctx, adc_dev)
{
	applyM2kFixes();

	iio_device_attr_write_longlong(m_dev, "oversampling_ratio", 1);
	m_m2k_fabric = iio_context_find_device(m_ctx, "m2k-fabric");
	if (m_m2k_fabric) {
		auto chn0 = iio_device_find_channel(m_m2k_fabric, "voltage0", false);
		auto chn1 = iio_device_find_channel(m_m2k_fabric, "voltage1", false);
		if (chn0 && chn1) {
			m_m2k_fabric_channels.push_back(chn0);
			m_m2k_fabric_channels.push_back(chn1);
			iio_channel_attr_write_bool(chn0, "powerdown", false);
			iio_channel_attr_write_bool(chn1, "powerdown", false);
		}
		iio_device_attr_write_bool(m_m2k_fabric, "clk_powerdown", false);
	}

	try {
		m_trigger = new M2kHardwareTrigger(m_ctx);
	} catch (std::exception& e){
		throw no_device_exception("Disabling hardware trigger support.");
	}

	for (unsigned int i = 0; i < m_nb_channels; i++) {
		m_input_range.push_back(PLUS_MINUS_25V);
	}
}

M2kAnalogIn::~M2kAnalogIn()
{

}

double M2kAnalogIn::convertRawToVolts(int sample, float correctionGain,
		float filterCompensation, float offset, float hw_gain)
{
	// TO DO: explain this formula
	return ((sample * 0.78) / ((1 << 11) * 1.3 * hw_gain) *
		correctionGain * filterCompensation) + offset;
}

int M2kAnalogIn::convertVoltsToRaw(double voltage, float correctionGain,
		float filterCompensation, float offset, float hw_gain)
{
	// TO DO: explain this formula
	return ((voltage - offset) / (correctionGain * filterCompensation) *
		(2048 * 1.3 * hw_gain) / 0.78);
}

double* M2kAnalogIn::getSamples(int nb_samples)
{
	try {
		return GenericAnalogIn::getSamples(nb_samples);
	} catch (std::runtime_error &e) {
		throw e;
	}
}

void M2kAnalogIn::openAnalogIn()
{
	std::cout << "Opened analog in for " << m_dev_name << "\n";
}

void M2kAnalogIn::closeAnalogIn()
{
	std::cout << "Opened analog in for " << m_dev_name << "\n";
}

void M2kAnalogIn::calibrate(bool async)
{

}

void M2kAnalogIn::applyM2kFixes()
{
	std::string hw_rev = Utils::getHardwareRevision(m_ctx);

	struct iio_device *dev = iio_context_find_device(m_ctx, "ad9963");

	int config1 = 0x05;
	int config2 = 0x05;

	if (hw_rev == "A") {
		config1 = 0x1B; // IGAIN1 +-6db  0.25db steps
		config2 = 0x1B;
	}

	/* Configure TX path */
	iio_device_reg_write(dev, 0x68, config1);
	iio_device_reg_write(dev, 0x6B, config2);
	iio_device_reg_write(dev, 0x69, 0x1C);  // IGAIN2 +-2.5%
	iio_device_reg_write(dev, 0x6C, 0x1C);
	iio_device_reg_write(dev, 0x6A, 0x20);  // IRSET +-20%
	iio_device_reg_write(dev, 0x6D, 0x20);
}


int M2kAnalogIn::getTriggerDelay()
{
	return m_trigger->delay();
}

void M2kAnalogIn::setTriggerDelay(int delay)
{
	m_trigger->setDelay(delay);
}

void M2kAnalogIn::setTriggerDelay(double percent)
{
	//percent of what?:-??
}

double M2kAnalogIn::getTriggerOffset()
{

}

void M2kAnalogIn::setTriggerOffset(double volt)
{

}

void M2kAnalogIn::setEnableTrigger(bool en)
{

}

bool M2kAnalogIn::getEnableTrigger()
{

}

void M2kAnalogIn::setTriggerChannel(ANALOG_IN_CHANNEL channel)
{
	if (channel < m_nb_channels) {
		m_trigger->setSourceChannel(channel);
	} else {
		throw invalid_parameter_exception("No such channel");
	}
}

M2kAnalogIn::ANALOG_IN_CHANNEL M2kAnalogIn::getTriggerChannel()
{
	int sourceChannel = m_trigger->sourceChannel();
	if (sourceChannel == 0) {
		return ANALOG_IN_CHANNEL_1;
	} else if (sourceChannel == 1) {
		return ANALOG_IN_CHANNEL_2;
	} else {
		throw invalid_parameter_exception("No trigger channel configured");
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
