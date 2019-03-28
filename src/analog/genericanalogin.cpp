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

#include <libm2k/analog/genericanalogin.hpp>
#include <libm2k/m2kexceptions.hpp>

#include "libm2k/utils.hpp"
#include <iio.h>
#include <iostream>
#include <algorithm>

using namespace libm2k::utils;
using namespace libm2k::analog;
using namespace std;

GenericAnalogIn::GenericAnalogIn(iio_context *ctx, std::string adc_dev) :
	Device(ctx, adc_dev)
{
}

GenericAnalogIn::~GenericAnalogIn()
{
//	m_data.clear();
}

std::vector<std::vector<double>> GenericAnalogIn::getSamples(int nb_samples,
							     bool processed)
{
	try {
		return Device::getSamples(nb_samples, processSample);
	} catch(std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
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
		return getDoubleValue("sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double GenericAnalogIn::getSampleRate(unsigned int chn_idx)
{
	try {
		return getDoubleValue(chn_idx, "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double GenericAnalogIn::setSampleRate(double sampleRate)
{
	try {
		return setDoubleValue(sampleRate, "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

double GenericAnalogIn::setSampleRate(unsigned int chn_idx, double sampleRate)
{
	try {
		return setDoubleValue(chn_idx, sampleRate, "sampling_frequency");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

std::vector<double> GenericAnalogIn::getAvailableSamplerates()
{
	return Device::getAvailableSamplerates();
}

string GenericAnalogIn::getDeviceName()
{
	return m_dev_name;
}

void GenericAnalogIn::enableChannel(unsigned int index, bool enable)
{
	try {
		Device::enableChannel(index, enable);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

