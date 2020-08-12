/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "utils/devicein.hpp"
#include "genericanalogin_impl.hpp"
#include <libm2k/m2kexceptions.hpp>
#include "libm2k/utils/utils.hpp"
#include <iio.h>
#include <iostream>
#include <algorithm>

using namespace libm2k::analog;
using namespace libm2k::utils;

GenericAnalogInImpl::GenericAnalogInImpl(iio_context *ctx, std::string adc_dev)
{
	m_dev_name = adc_dev;
	m_devices_in.push_back(make_shared<DeviceIn>(ctx, adc_dev));
}

GenericAnalogInImpl::~GenericAnalogInImpl()
{
	m_devices_in.clear();
}

std::shared_ptr<DeviceIn> GenericAnalogInImpl::getAdcDevice(unsigned int index)
{
	if (index >= m_devices_in.size()) {
		throw_exception(m2k_exception::make("Generic Analog In: No such ADC device").type(libm2k::EXC_OUT_OF_RANGE).build());
		return nullptr;
	}
	return m_devices_in.at(index);
}

double GenericAnalogInImpl::processSample(int16_t sample, unsigned int channel)
{
	return sample;
}

const double *GenericAnalogInImpl::getSamplesInterleaved(unsigned int nb_samples)
{
	return getAdcDevice(0)->getSamplesInterleaved(nb_samples, processSample);
}

const short *GenericAnalogInImpl::getSamplesRawInterleaved(unsigned int nb_samples)
{
	return getAdcDevice(0)->getSamplesRawInterleaved(nb_samples);
}

double GenericAnalogInImpl::getSampleRate()
{
	return getAdcDevice(0)->getDoubleValue("sampling_frequency");
}

double GenericAnalogInImpl::getSampleRate(unsigned int chn_idx)
{
	return getAdcDevice(0)->getDoubleValue(chn_idx, "sampling_frequency");
}

double GenericAnalogInImpl::setSampleRate(double sampleRate)
{
	return getAdcDevice(0)->setDoubleValue(sampleRate, "sampling_frequency");
}

double GenericAnalogInImpl::setSampleRate(unsigned int chn_idx, double sampleRate)
{
	return getAdcDevice(0)->setDoubleValue(chn_idx, sampleRate, "sampling_frequency");
}

std::vector<double> GenericAnalogInImpl::getAvailableSampleRates()
{
	std::vector<std::string> stringValues;
	std::vector<double> values;

	stringValues = getAdcDevice(0)->getAvailableAttributeValues("sampling_frequency");
	std::transform(stringValues.begin(), stringValues.end(), std::back_inserter(values),
		       [] (std::string &s) -> double { return std::stod(s); });
	return values;
}

std::vector<double> GenericAnalogInImpl::getAvailableSampleRates(unsigned int chn_idx)
{
	std::vector<std::string> stringValues;
	std::vector<double> values;

	stringValues = getAdcDevice(0)->getAvailableAttributeValues(chn_idx, "sampling_frequency");
	std::transform(stringValues.begin(), stringValues.end(), std::back_inserter(values),
		       [] (std::string &s) -> double { return std::stod(s); });
	return values;
}


string GenericAnalogInImpl::getDeviceName()
{
	return m_dev_name;
}

libm2k::IIO_OBJECTS GenericAnalogInImpl::getIioObjects()
{
	return getAdcDevice(0)->getIioObjects();
}

unsigned int GenericAnalogInImpl::getNbChannels()
{
	return getAdcDevice(0)->getNbChannels(false);
}

double GenericAnalogInImpl::getMaximumSamplerate()
{
	auto values = getAvailableSampleRates();
	return *(max_element(values.begin(), values.end()));
}

double GenericAnalogInImpl::getMaximumSamplerate(unsigned int chn_idx)
{
	auto values = getAvailableSampleRates(chn_idx);
	return *(max_element(values.begin(), values.end()));
}

void GenericAnalogInImpl::enableChannel(unsigned int index, bool enable)
{
	getAdcDevice(0)->enableChannel(index, enable, false);
}

void GenericAnalogInImpl::setKernelBuffersCount(unsigned int count)
{
	getAdcDevice(0)->setKernelBuffersCount(count);
}
