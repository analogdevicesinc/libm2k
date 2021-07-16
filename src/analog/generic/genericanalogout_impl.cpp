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

#include "utils/deviceout.hpp"
#include <utils/channel.hpp>
#include "genericanalogout_impl.hpp"
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace libm2k::analog;
using namespace libm2k::utils;

GenericAnalogOutImpl::GenericAnalogOutImpl(iio_context *ctx, std::string dac_dev)
{
	m_devices_out.push_back(make_shared<DeviceOut>(ctx, dac_dev));
	for (auto dac : m_devices_out) {
		m_cyclic.push_back(false);
	}
}

GenericAnalogOutImpl::~GenericAnalogOutImpl()
{
	m_devices_out.clear();
}

std::shared_ptr<libm2k::utils::DeviceOut> GenericAnalogOutImpl::getDacDevice(unsigned int index)
{
	if (index >= m_devices_out.size()) {
		THROW_M2K_EXCEPTION("No such DAC device", libm2k::EXC_OUT_OF_RANGE);
		return nullptr;
	}
	return m_devices_out.at(index);
}

double GenericAnalogOutImpl::getSampleRate()
{
	return getDacDevice(0)->getDoubleValue("sampling_frequency");
}

double GenericAnalogOutImpl::getSampleRate(unsigned int chn_idx)
{
	return getDacDevice(0)->getDoubleValue(chn_idx, "sampling_frequency", true);
}

double GenericAnalogOutImpl::setSampleRate(double sampleRate)
{
	return getDacDevice(0)->setDoubleValue(sampleRate, "sampling_frequency");
}

double GenericAnalogOutImpl::setSampleRate(unsigned int chn_idx, double sampleRate)
{
	return getDacDevice(0)->setDoubleValue(chn_idx, sampleRate,
			      "sampling_frequency", true);
}

std::vector<double> GenericAnalogOutImpl::getAvailableSampleRates()
{
	std::vector<std::string> stringValues;
	std::vector<double> values;

	stringValues = getDacDevice(0)->getAvailableAttributeValues("sampling_frequency");
	std::transform(stringValues.begin(), stringValues.end(), std::back_inserter(values),
	      [] (std::string &s) -> double {
		try {
			auto value = std::stod(s);
			return value;
		} catch (std::exception&) {
			THROW_M2K_EXCEPTION("Can't determine available sampling frequencies.", libm2k::EXC_RUNTIME_ERROR);
			return {};
		}
	});
	return values;
}

std::vector<double> GenericAnalogOutImpl::getAvailableSampleRates(unsigned int chn_idx)
{
	std::vector<std::string> stringValues;
	std::vector<double> values;

	stringValues = getDacDevice(0)->getAvailableAttributeValues(chn_idx, "sampling_frequency");
	std::transform(stringValues.begin(), stringValues.end(), std::back_inserter(values),
		       [] (std::string &s) -> double {
		try {
			auto value = std::stod(s);
			return value;
		} catch (std::exception&) {
			THROW_M2K_EXCEPTION("Can't determine available sampling frequencies.", libm2k::EXC_RUNTIME_ERROR);
			return {};
		}
	});
	return values;
}

void GenericAnalogOutImpl::setCyclic(bool en)
{
	m_cyclic.at(0) = en;
	getDacDevice(0)->setCyclic(en);
}

bool GenericAnalogOutImpl::getCyclic()
{
	return m_cyclic.at(0);
}

void GenericAnalogOutImpl::pushRaw(unsigned int chn_idx, const void* data, unsigned int nb_samples)
{
	getDacDevice(0)->push(data, chn_idx, nb_samples, getCyclic(), true);
}

void GenericAnalogOutImpl::pushRawInterleaved(const void* data, unsigned int nb_samples_per_chn)
{
	getDacDevice(0)->pushInterleaved(data, nb_samples_per_chn, true);
}

int8_t* GenericAnalogOutImpl::convertVoltsToRaw(double voltage, double scale, int offset)
{
	voltage = voltage * scale + offset;

}

//void GenericAnalogOutImpl::pushRaw(unsigned int chn_idx, short *data, unsigned int nb_samples)
//{
//	getDacDevice(0)->push(data, chn_idx, nb_samples, getCyclic());
//}

//void GenericAnalogOutImpl::pushRaw(unsigned int chn_idx, std::vector<short> const &data)
//{
//	getDacDevice(0)->push(data, chn_idx, getCyclic(), true);
//}

void GenericAnalogOutImpl::stop()
{
	getDacDevice(0)->stop();
}

string GenericAnalogOutImpl::getName()
{
	return getDacDevice(0)->getName();
}

libm2k::IIO_OBJECTS GenericAnalogOutImpl::getIioObjects()
{
	return getDacDevice(0)->getIioObjects();
}

unsigned int GenericAnalogOutImpl::getNbBufferedChannels()
{
	return getDacDevice(0)->getNbBufferedChannels();
}

unsigned int GenericAnalogOutImpl::getNbDdsChannels()
{
	return getDacDevice(0)->getNbDdsChannels();
}

double GenericAnalogOutImpl::getMaximumSamplerate()
{
	auto values = getAvailableSampleRates();
	return *(max_element(values.begin(), values.end()));
}

double GenericAnalogOutImpl::getMaximumSamplerate(unsigned int chn_idx)
{
	auto values = getAvailableSampleRates(chn_idx);
	return *(max_element(values.begin(), values.end()));
}

void GenericAnalogOutImpl::enableBufferedChannel(unsigned int chnIdx, bool enable)
{
	getDacDevice(0)->enableBufferedChannel(chnIdx, enable);
}

void GenericAnalogOutImpl::enableDdsChannel(unsigned int chnIdx, bool enable)
{
	getDacDevice(0)->enableDdsChannel(chnIdx, enable);
}

bool GenericAnalogOutImpl::isBufferedChannelEnabled(unsigned int chnIdx)
{
	return getDacDevice(0)->isBufferedChannelEnabled(chnIdx);
}

bool GenericAnalogOutImpl::isDdsChannelEnabled(unsigned int chnIdx)
{
	return getDacDevice(0)->isDdsChannelEnabled(chnIdx);
}

void GenericAnalogOutImpl::setKernelBuffersCount(unsigned int count)
{
	getDacDevice(0)->setKernelBuffersCount(count);
}
