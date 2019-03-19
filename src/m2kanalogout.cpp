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
#include "libm2k/utils.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <iio.h>
#include <string.h>

using namespace libm2k::analog;
using namespace libm2k::utils;
using namespace std;

M2kAnalogOut::M2kAnalogOut(iio_context *ctx, std::vector<std::string> dac_devs):
	Device(ctx, ""),
	m_sync_start(true)
{
	m_dac_devices.push_back(new Device(ctx, dac_devs.at(0)));
	m_dac_devices.push_back(new Device(ctx, dac_devs.at(1)));

	m_m2k_fabric = make_shared<Device>(ctx, "m2k-fabric");
	if (!m_m2k_fabric) {
		throw invalid_parameter_exception("Analog out: Can not find m2k fabric device");
	}
	m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
	m_m2k_fabric->setBoolValue(1, false, "powerdown", true);

	m_calib_vlsb.push_back(10.0 / ((double)( 1 << 12 )));
	m_calib_vlsb.push_back(10.0 / ((double)( 1 << 12 )));
	m_filter_compensation_table[75E6] = 1.00;
	m_filter_compensation_table[75E5] = 1.525879;
	m_filter_compensation_table[75E4] = 1.164153;
	m_filter_compensation_table[75E3] = 1.776357;
	m_filter_compensation_table[75E2] = 1.355253;
	m_filter_compensation_table[75E1] = 1.033976;

	for (unsigned int i = 0; i < m_dac_devices.size(); i++) {
		m_cyclic.push_back(true);
	}
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

std::vector<double> M2kAnalogOut::getOversamplingRatio()
{
	try {
		std::vector<double> values = {};
		for (int i = 0; i < m_dac_devices.size(); i++) {
			double val = m_dac_devices.at(i)->getDoubleValue("oversampling_ratio");
			values.push_back(val);
		}
		return values;
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogOut::getOversamplingRatio(unsigned int chn_idx)
{
	try {
		return m_dac_devices.at(chn_idx)->getDoubleValue("oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

std::vector<double> M2kAnalogOut::setOversamplingRatio(std::vector<double> oversampling_ratio)
{
	try {
		std::vector<double> values = {};
		for (int i = 0; i < oversampling_ratio.size(); i++) {
			double val = m_dac_devices.at(i)->setDoubleValue(oversampling_ratio.at(i),
									 "oversampling_ratio");
			values.push_back(val);
		}
		return values;
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogOut::setOversamplingRatio(unsigned int chn_idx, double oversampling_ratio)
{
	try {
		return m_dac_devices.at(chn_idx)->setDoubleValue(oversampling_ratio,
								 "oversampling_ratio");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

std::vector<double> M2kAnalogOut::getSamplerate()
{
	try {
		std::vector<double> values = {};
		for (int i = 0; i < m_dac_devices.size(); i++) {
			double val = m_dac_devices.at(i)->getDoubleValue("sampling_frequency");
			values.push_back(val);
		}
		return values;
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogOut::getSamplerate(unsigned int chn_idx)
{
	try {
		return m_dac_devices.at(chn_idx)->getDoubleValue("sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

std::vector<double> M2kAnalogOut::setSamplerate(std::vector<double> samplerates)
{
	try {
		std::vector<double> values = {};
		for (int i = 0; i < samplerates.size(); i++) {
			double val = m_dac_devices.at(i)->setDoubleValue(samplerates.at(i),
									 "sampling_frequency");
			values.push_back(val);
		}
		return values;
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double M2kAnalogOut::setSamplerate(unsigned int chn_idx, double samplerate)
{
	try {
		return m_dac_devices.at(chn_idx)->setDoubleValue(samplerate,
								 "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kAnalogOut::setSyncedDma(bool en, int chn)
{
	if (chn < 0) {
		for (auto dac : m_dac_devices) {
			dac->setBoolValue(en, "dma_sync");
		}
	} else {
		if (chn >= m_dac_devices.size()) {
			throw invalid_parameter_exception("Analog Out: No such channel");
		}
		m_dac_devices.at(chn)->setBoolValue(en, "dma_sync");
	}
}

bool M2kAnalogOut::getSyncedDma(int chn)
{
	if (chn >= m_dac_devices.size()) {
		throw invalid_parameter_exception("Analog Out: No such channel");
	}

	return m_dac_devices.at(chn)->getBoolValue("dma_sync");
}

void M2kAnalogOut::setCyclic(bool en)
{
	for (unsigned int i = 0; i < m_dac_devices.size(); i++) {
		m_cyclic.at(i) = en;
	}
}

void M2kAnalogOut::setCyclic(bool en, unsigned int chn)
{
	if (chn >= m_dac_devices.size()) {
		throw invalid_parameter_exception("Analog Out: No such channel");
	}
	m_cyclic.at(chn) = en;
}

bool M2kAnalogOut::getCyclic(unsigned int chn)
{
	if (chn >= m_dac_devices.size()) {
		throw invalid_parameter_exception("Analog Out: No such channel");
	}
	return m_cyclic.at(chn);
}

int M2kAnalogOut::convertVoltsToRaw(double voltage, double vlsb,
				       double filterCompensation)
{
	// TO DO: explain this formula....
	return voltage * ((-1 * (1 / vlsb) * 16) / filterCompensation);
}


void M2kAnalogOut::setDacCalibVlsb(unsigned int chn_idx, double vlsb)
{
	m_calib_vlsb[chn_idx] = vlsb;
}

void M2kAnalogOut::push(std::vector<std::vector<short>> &data)
{
	//check data size
	std::vector<std::vector<short>> data_buffers;
	setSyncedDma(true);
	for (int chn = 0; chn < data.size(); chn++) {
		size_t size = data.at(chn).size();
		std::vector<short> raw_data_buffer;
		for (int i = 0; i < size; i++) {
			raw_data_buffer.push_back(processSample(data.at(chn).at(i), chn, true));
		}
		m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
	}
	setSyncedDma(false);
	m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
	m_m2k_fabric->setBoolValue(1, false, "powerdown", true);
}

void M2kAnalogOut::push(std::vector<std::vector<double>> &data)
{
	std::vector<std::vector<short>> data_buffers;
	setSyncedDma(true);
	for (int chn = 0; chn < data.size(); chn++) {
		size_t size = data.at(chn).size();
		std::vector<short> raw_data_buffer;
		for (int i = 0; i < size; i++) {
			raw_data_buffer.push_back(processSample(data.at(chn).at(i), chn, false));
		}
		m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
		data_buffers.push_back(raw_data_buffer);
	}
	setSyncedDma(false);
	m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
	m_m2k_fabric->setBoolValue(1, false, "powerdown", true);
}

double M2kAnalogOut::getScalingFactor(unsigned int chn)
{
	if (chn >= m_calib_vlsb.size()) {
		throw invalid_parameter_exception("No such channel");
	}
	return (-1 * (1 / m_calib_vlsb.at(chn)) * 16) /
			getFilterCompensation(getSamplerate(chn));
}

double M2kAnalogOut::getFilterCompensation(double samplerate)
{
	return m_filter_compensation_table.at(samplerate);
}

void M2kAnalogOut::stopOutput()
{
	m_m2k_fabric->setBoolValue(0, true, "powerdown", true);
	m_m2k_fabric->setBoolValue(1, true, "powerdown", true);

	for (Device* dev : m_dac_devices) {
		dev->stop();
	}
}

short M2kAnalogOut::processSample(double value, unsigned int channel, bool raw)
{
	if (raw) {
		short raw_value = value;
		raw_value = (-raw_value) << 4;

		// This should go away once channel type gets
		// changed from 'le:S16/16>>0' to 'le:S12/16>>4'
		return raw_value;
	} else {
		return convertVoltsToRaw(value, m_calib_vlsb.at(channel),
					 getFilterCompensation(
						 getSamplerate(channel)));
	}
}

void M2kAnalogOut::enableChannel(unsigned int chnIdx, bool enable)
{
	if (chnIdx >= m_dac_devices.size()) {
		throw invalid_parameter_exception("No such channel");
	}

	m_dac_devices.at(chnIdx)->enableChannel(0, enable);
}
