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

std::string GenericAnalogOut::getDeviceName()
{
	return m_dev_name;
}

void libm2k::analog::GenericAnalogOut::openAnalogOut()
{

}

void libm2k::analog::GenericAnalogOut::closeAnalogOut()
{

}

