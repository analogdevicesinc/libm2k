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

#include "libm2k/genericdigital.hpp"
#include "libm2k/m2kexceptions.hpp"

#include "utils.hpp"
#include <iio.h>
#include <iostream>
#include <algorithm>

using namespace libm2k::utils;
using namespace libm2k::digital;
using namespace std;



GenericDigital::GenericDigital(iio_context *ctx, string logic_dev) :
	m_ctx(ctx)
{
	if (m_ctx) {
		m_dev = iio_context_find_device(m_ctx, logic_dev.c_str());
	}

	if (!m_dev) {
		m_dev = nullptr;
		throw no_device_exception("Digital In device not found");
	}
	m_dev_name = logic_dev;
	m_nb_channels = iio_device_get_channels_count(m_dev);
	for (unsigned int i = 0; i < m_nb_channels; i++) {
		struct channel chn;
		chn.m_channel = iio_device_get_channel(m_dev, i);
		chn.m_direction = GenericDigital::DIO_INPUT;
		m_channel_list.push_back(chn);
	}

	if (m_channel_list.size() != m_nb_channels) {
		m_nb_channels = m_channel_list.size();
	}
}

GenericDigital::~GenericDigital()
{
	m_data.clear();
}

double GenericDigital::getSampleRate()
{
	try {
		return Utils::getDoubleValue(m_dev, "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double GenericDigital::setSampleRate(double sampleRate)
{
	try {
		return Utils::setDoubleValue(m_dev, sampleRate,
					     "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void GenericDigital::enableChannel(unsigned int index, bool enable)
{
	if (index < m_nb_channels) {
		if (enable) {
			iio_channel_enable(m_channel_list.at(index).m_channel);
		} else {
			iio_channel_disable(m_channel_list.at(index).m_channel);
		}
	} else {
		throw invalid_parameter_exception("Cannot enable digital channel.");
	}
}

unsigned int GenericDigital::nbChannels()
{
	return m_nb_channels;
}

iio_context *GenericDigital::ctx()
{
	return m_ctx;
}

