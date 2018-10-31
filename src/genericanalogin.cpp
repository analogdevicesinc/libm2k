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

#include "libm2k/genericanalogin.hpp"
#include "libm2k/m2kexceptions.hpp"

#include "utils.hpp"
#include <iio.h>
#include <iostream>
#include <algorithm>

using namespace libm2k::utils;
using namespace libm2k::analog;
using namespace std;

GenericAnalogIn::GenericAnalogIn(iio_context *ctx, std::__cxx11::string adc_dev) :
	m_ctx(ctx)
{
	if (m_ctx) {
		m_dev = iio_context_find_device(m_ctx, adc_dev.c_str());
	}

	if (!m_dev) {
		m_dev = nullptr;
		throw no_device_exception("Analog In device not found");
	}
	m_dev_name = adc_dev;
	m_nb_channels = iio_device_get_channels_count(m_dev);
	for (unsigned int i = 0; i < m_nb_channels; i++) {
		m_channel_list.push_back(iio_device_get_channel(m_dev, i));
	}

	if (m_channel_list.size() != m_nb_channels) {
		m_nb_channels = m_channel_list.size();
	}

	iio_context_set_timeout(m_ctx, 0);
}

GenericAnalogIn::~GenericAnalogIn()
{
	m_data.clear();
}

std::vector<std::vector<double>> GenericAnalogIn::getSamples(int nb_samples,
							     bool processed)
{
	std::vector<bool> channels_enabled;
//	std::vector<std::vector<int16_t>> ch_data;
	m_data.clear();

	if (!m_dev) {
		throw no_device_exception("No such device: " + m_dev_name);
	}

	// Store channels enable state
	for (unsigned int i = 0; i < m_nb_channels; i++) {
		bool en = iio_channel_is_enabled(
				iio_device_get_channel(m_dev, i));
		channels_enabled.push_back(en);
		std::vector<double> data {};
		for (int j = 0; j < nb_samples; j++) {
			data.push_back(0);
		}
		m_data.push_back(data);
	}

	// Enable the required channels
	for (int i = 0; i < m_data.size(); i++) {
		enableChannel(i, true);
	}

	struct iio_buffer *buffer = iio_device_create_buffer(m_dev,
		nb_samples, false);

	if (!buffer) {
		for (unsigned int i = 0; i < m_nb_channels; i++) {
			enableChannel(i, channels_enabled.at(i));
		}
		throw instrument_already_in_use_exception(
			"Cannot create buffer for " + m_dev_name);
	}

	int ret = iio_buffer_refill(buffer);

	if (ret < 0) {
		iio_buffer_destroy(buffer);
		for (unsigned int i = 0; i < m_nb_channels; i++) {
			enableChannel(i, channels_enabled.at(i));
		}

		throw instrument_already_in_use_exception(
			"Cannot refill buffer for " + m_dev_name);
	}

	ptrdiff_t p_inc = iio_buffer_step(buffer);
	uintptr_t p_dat;
	uintptr_t p_end = (uintptr_t)iio_buffer_end(buffer);
	unsigned int i;
	for (i = 0, p_dat = (uintptr_t)iio_buffer_first(buffer, m_channel_list.at(0));
			p_dat < p_end; p_dat += p_inc, i++)
	{
		for (int ch = 0; ch < m_data.size(); ch++) {
//			m_data[ch][i] = ((int16_t*)p_dat)[ch];
//			std::cout << "before processing " << i << " " << ch << std::endl;
			m_data[ch][i] = processSample(((int16_t*)p_dat)[ch], ch);
		}
	}

	iio_buffer_destroy(buffer);

	// Restore channels enable states
	for (unsigned int i = 0; i < m_nb_channels; i++) {
		enableChannel(i, channels_enabled.at(i));
	}
	return m_data;
}

struct iio_context* GenericAnalogIn::getContext()
{
	return m_ctx;
}

double GenericAnalogIn::processSample(int16_t sample, unsigned int ch)
{
	return (double)sample;
}

void GenericAnalogIn::openAnalogIn()
{
	std::cout << "Opened analog in for " << m_dev_name << "\n";
}

void GenericAnalogIn::closeAnalogIn()
{
	std::cout << "Closed analog in for " << m_dev_name << "\n";
}

double GenericAnalogIn::getSampleRate()
{
	try {
		return Utils::getDoubleValue(m_dev, "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double GenericAnalogIn::getSampleRate(unsigned int chn_idx)
{
	try {
		return Utils::getDoubleValue(m_dev, chn_idx,
					     "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double GenericAnalogIn::setSampleRate(double sampleRate)
{
	try {
		return Utils::setDoubleValue(m_dev, sampleRate,
					    "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double GenericAnalogIn::setSampleRate(unsigned int chn_idx, double sampleRate)
{
	try {
		return Utils::setDoubleValue(m_dev, chn_idx,
					    sampleRate, "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

std::vector<unsigned long> GenericAnalogIn::getAvailableSamplerates()
{
	return Utils::getAvailableSamplerates(m_dev);
}

string GenericAnalogIn::getDeviceName()
{
	return m_dev_name;
}

void GenericAnalogIn::enableChannel(unsigned int index, bool enable)
{
	if (index < m_nb_channels) {
		if (enable) {
			iio_channel_enable(m_channel_list.at(index));
		} else {
			iio_channel_disable(m_channel_list.at(index));
		}
	} else {
		throw invalid_parameter_exception("Cannot enable channel");
	}
}

