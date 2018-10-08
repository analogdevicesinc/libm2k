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
#include <iostream>

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

	iio_context_set_timeout(m_ctx, 0);
}

GenericAnalogIn::~GenericAnalogIn()
{
	m_data.clear();
}

double* GenericAnalogIn::getSamples(int nb_samples)
{
	double* samples;
	if (!m_dev) {
		throw no_device_exception("No such device: " + m_dev_name);
	}

	unsigned int i;
	for (i = 0; i < m_nb_channels; i++) {
		enableChannel(i, true);
	}

	struct iio_buffer* buffer = iio_device_create_buffer(m_dev, nb_samples, false);
	if (!buffer) {
		throw instrument_already_in_use_exception(
			"Cannot create buffer for " + m_dev_name);
	}

	ssize_t ret = iio_buffer_refill(buffer);
	if (ret < 0) {
		iio_buffer_destroy(buffer);
		throw instrument_already_in_use_exception(
			"Cannot refill buffer for " + m_dev_name);
	}

	samples = static_cast<double*> (iio_buffer_start(buffer));

	iio_buffer_destroy(buffer);
	return samples;
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
	double sampleRate = 0;

	if (Utils::iioDevHasAttribute(m_dev, "sampling_frequency")) {
		iio_device_attr_read_double(m_dev, "sampling_frequency",
			&sampleRate);
	} else {
		throw invalid_parameter_exception(m_dev_name +
				" has no sampling frequency attribute");
	}
	return sampleRate;
}

double GenericAnalogIn::getSampleRate(unsigned int chn_idx)
{
	double sampleRate = 0;

	if (chn_idx >= m_nb_channels) {
		throw invalid_parameter_exception(m_dev_name +
				" has no such channel");
	}

	auto chn = iio_device_get_channel(m_dev, chn_idx);
	if (Utils::iioChannelHasAttribute(chn, "sampling_frequency")) {
		iio_channel_attr_read_double(chn, "sampling_frequency",
			&sampleRate);
	} else {
		throw invalid_parameter_exception(m_dev_name +
				" has no sampling frequency attribute for the selected channel");
	}
	return sampleRate;
}

double GenericAnalogIn::setSampleRate(double sampleRate)
{
	if (Utils::iioDevHasAttribute(m_dev, "sampling_frequency")) {
		iio_device_attr_write_double(m_dev, "sampling_frequency",
			sampleRate);
	} else {
		throw invalid_parameter_exception(m_dev_name +
				" has no sampling frequency attribute");
	}
	return getSampleRate();
}

double GenericAnalogIn::setSampleRate(unsigned int chn_idx, double sampleRate)
{
	if (chn_idx >= m_nb_channels) {
		throw invalid_parameter_exception(m_dev_name +
				" has no such channel");
	}

	auto chn = iio_device_get_channel(m_dev, chn_idx);
	if (Utils::iioChannelHasAttribute(chn, "sampling_frequency")) {
		iio_channel_attr_write_double(chn, "sampling_frequency",
			sampleRate);
	} else {
		throw invalid_parameter_exception(m_dev_name +
				" has no sampling frequency attribute for the selected channel");
	}
	return getSampleRate(chn_idx);
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

