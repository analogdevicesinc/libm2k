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

#include "libm2k/m2kanalogout.hpp"
#include "libm2k/genericanalogout.hpp"
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

M2kAnalogOut::M2kAnalogOut(iio_context *ctx, string dac_dev):
	GenericAnalogOut(ctx, dac_dev)
{
	for (unsigned int i = 0; i < m_nb_channels; i++) {
		m_channel_list.push_back(iio_device_find_channel(
			m_dev, "voltage" + i, true));
	}

	m_m2k_fabric = iio_context_find_device(m_ctx, "m2k-fabric");
	if (m_m2k_fabric) {
		auto chn0 = iio_device_find_channel(m_m2k_fabric, "voltage0", true);
		auto chn1 = iio_device_find_channel(m_m2k_fabric, "voltage1", true);
		if (chn0 && chn1) {
			m_m2k_fabric_channels.push_back(chn0);
			m_m2k_fabric_channels.push_back(chn1);
			iio_channel_attr_write_bool(chn0, "powerdown", false);
			iio_channel_attr_write_bool(chn1, "powerdown", false);
		}
	}

	m_ad5625 = iio_context_find_device(m_ctx, "ad5625");
	if (m_ad5625) {
		auto chn0 = iio_device_find_channel(m_ad5625, "voltage0", true);
		auto chn1 = iio_device_find_channel(m_ad5625, "voltage1", true);
		if (chn0 && chn1) {
			m_ad5625_channels.push_back(chn0);
			m_ad5625_channels.push_back(chn1);
		}
	}

	m_buffer = nullptr;
	m_dac_calib_vlsb = 10.0 / ((double)( 1 << 12 ));
	m_filter_compensation_table[75E6] = 1.00;
	m_filter_compensation_table[75E5] = 1.525879;
	m_filter_compensation_table[75E4] = 1.164153;
	m_filter_compensation_table[75E3] = 1.776357;
	m_filter_compensation_table[75E2] = 1.355253;
	m_filter_compensation_table[75E1] = 1.033976;
}

M2kAnalogOut::~M2kAnalogOut()
{
	stopOutput();
}

void M2kAnalogOut::openAnalogOut()
{

}

void M2kAnalogOut::closeAnalogOut()
{

}

double M2kAnalogOut::getOversamplingRatio()
{
	try {
		return Utils::getDoubleValue(m_dev, "oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogOut::getOversamplingRatio(unsigned int chn_idx)
{
	try {
		return Utils::getDoubleValue(m_dev, chn_idx,
					     "oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogOut::setOversamplingRatio(double oversampling_ratio)
{
	try {
		return Utils::setDoubleValue(m_dev, oversampling_ratio,
					    "oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogOut::setOversamplingRatio(unsigned int chn_idx, double oversampling_ratio)
{
	try {
		return Utils::setDoubleValue(m_dev, chn_idx,
					    oversampling_ratio,
					    "oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

iio_channel *M2kAnalogOut::getChannel()
{
	if (m_channel_list.size() > 0) {
		return m_channel_list.at(0);
	} else {
		throw invalid_parameter_exception("No such DAC channel");
	}
}

iio_channel *M2kAnalogOut::getAuxChannel(unsigned int chn_idx)
{
	if (chn_idx < m_ad5625_channels.size()) {
		return m_ad5625_channels.at(chn_idx);
	} else {
		throw invalid_parameter_exception("No such ad5625 channel");
	}
}

iio_channel *M2kAnalogOut::getFabricChannel(unsigned int chn_idx)
{
	if (chn_idx < m_m2k_fabric_channels.size()) {
		return m_m2k_fabric_channels.at(chn_idx);
	} else {
		throw invalid_parameter_exception("No such fabric channel");
	}
}

int M2kAnalogOut::convertVoltsToRaw(double voltage, double vlsb,
				       double filterCompensation)
{
	// TO DO: explain this formula....
	return voltage * ((-1 * (1 / vlsb) * 16) / filterCompensation);
}


void M2kAnalogOut::setDacCalibVlsb(double vlsb)
{
	m_dac_calib_vlsb = vlsb;
}

double M2kAnalogOut::getScalingFactor()
{
	return (-1 * (1 / m_dac_calib_vlsb) * 16) /
			getFilterCompensation(getSampleRate());
}

double M2kAnalogOut::getFilterCompensation(double samplerate)
{
	return m_filter_compensation_table.at(samplerate);
}

void M2kAnalogOut::sendConstant(double value, bool raw,
				size_t size, bool cyclic)
{
	short data[size];
	short raw_value = 0;

	if (m_buffer) {
		iio_buffer_destroy(m_buffer);
	}

	enableChannel(0, true);

	if (raw) {
		raw_value = value;

		// This should go away once channel type gets
		// changed from 'le:S16/16>>0' to 'le:S12/16>>4'
		raw_value = (-raw_value) << 4;
	} else {
		raw_value = convertVoltsToRaw(value, m_dac_calib_vlsb,
			      getFilterCompensation(
				      getSampleRate()));
	}

	std::fill_n(data, size, raw_value);

	iio_device_attr_write_bool(m_dev, "dma_sync", true);
	m_buffer = iio_device_create_buffer(m_dev,
					size,
					cyclic);
	size_t ret = iio_channel_write(m_channel_list.at(0), m_buffer, data,
		  size * sizeof(short));

	iio_buffer_push(m_buffer);
	iio_device_attr_write_bool(m_dev, "dma_sync", false);

	for (auto fabric_chn : m_m2k_fabric_channels) {
		iio_channel_attr_write_bool(fabric_chn, "powerdown", false);
	}
}

void M2kAnalogOut::stopOutput()
{
	enableChannel(0, true);
	for (auto fabric_chn : m_m2k_fabric_channels) {
		iio_channel_attr_write_bool(fabric_chn, "powerdown", true);
	}
}

