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

#include "libm2k/genericanalogout.hpp"
#include "libm2k/m2kexceptions.hpp"
#include "utils.hpp"
#include <iio.h>
#include <iostream>

using namespace std;
using namespace libm2k::analog;
using namespace libm2k::utils;

GenericAnalogOut::GenericAnalogOut(iio_context *ctx, std::string dac_dev):
	m_ctx(ctx)
{
	if (m_ctx) {
		m_dev = iio_context_find_device(m_ctx, dac_dev.c_str());
	}

	if (!m_dev) {
		m_dev = nullptr;
		throw no_device_exception("Analog Out device not found");
	}
	m_dev_name = dac_dev;
	m_nb_channels = iio_device_get_channels_count(m_dev);
	for (unsigned int i = 0; i < m_nb_channels; i++) {
		m_channel_list.push_back(iio_device_get_channel(m_dev, i));
	}

	if (m_channel_list.size() != m_nb_channels) {
		m_nb_channels = m_channel_list.size();
	}
	m_buffer = nullptr;
}

GenericAnalogOut::~GenericAnalogOut()
{

}
double GenericAnalogOut::getSampleRate()
{
	try {
		return Utils::getDoubleValue(m_dev, "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double GenericAnalogOut::getSampleRate(unsigned int chn_idx)
{
	try {
		return Utils::getDoubleValue(m_dev, chn_idx, "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double GenericAnalogOut::setSampleRate(double sampleRate)
{
	try {
		return Utils::setDoubleValue(m_dev, sampleRate, "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double GenericAnalogOut::setSampleRate(unsigned int chn_idx, double sampleRate)
{
	try {
		return Utils::setDoubleValue(m_dev, chn_idx, sampleRate,
					    "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

std::vector<unsigned long> GenericAnalogOut::getAvailableSamplerates()
{
	return Utils::getAvailableSamplerates(m_dev);
}

iio_context *GenericAnalogOut::getContext()
{
	return m_ctx;
}

void GenericAnalogOut::enableChannel(unsigned int index, bool enable)
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

bool GenericAnalogOut::isChannelEnabled(unsigned int index)
{
	if (index < m_channel_list.size()) {
		return iio_channel_is_enabled(m_channel_list.at(index));
	} else {
		throw invalid_parameter_exception("No such channel for the AnalogOut");
	}
}

std::string GenericAnalogOut::getDeviceName()
{
	return m_dev_name;
}

void GenericAnalogOut::push(std::vector<short> &data, bool cyclic,
			    unsigned int chn_idx)
{
	size_t size = data.size();
//	short raw_value = 0;

	if (m_buffer) {
		iio_buffer_destroy(m_buffer);
	}

	try {
		if (!isChannelEnabled(chn_idx)) {
			throw invalid_parameter_exception("The AnalogOut channel "
							  "is not enabled");
			// or should we check if we have at least one en channel?
		}

		setupBeforeBuffer();

		m_buffer = iio_device_create_buffer(m_dev,
						size,
						cyclic);
		if (!m_buffer) {
			throw invalid_parameter_exception("Can't create the TX buffer");
		}

		// on which channel do we write this?
		size_t ret = iio_channel_write(m_channel_list.at(chn_idx), m_buffer, data.data(),
			  size * sizeof(short));

		iio_buffer_push(m_buffer);

		setupAfterBuffer();
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void libm2k::analog::GenericAnalogOut::openAnalogOut()
{

}

void libm2k::analog::GenericAnalogOut::closeAnalogOut()
{

}

void GenericAnalogOut::stopOutput()
{
	if (m_buffer) {
		iio_buffer_destroy(m_buffer);
		m_buffer = nullptr;
	}
}

void GenericAnalogOut::setupBeforeBuffer()
{
//	write some attr if needed
}

void GenericAnalogOut::setupAfterBuffer()
{
//	write some attrs if needed
}


short GenericAnalogOut::processSample(double value, bool raw)
{
	return (short)value;
}
