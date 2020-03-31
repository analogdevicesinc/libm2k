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
#include "genericanalogout_impl.hpp"
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>
#include <iostream>

using namespace std;
using namespace libm2k::analog;
using namespace libm2k::utils;

GenericAnalogOutImpl::GenericAnalogOutImpl(iio_context *ctx, std::string dac_dev)
{
	m_devices_out.push_back(make_shared<DeviceOut>(ctx, dac_dev));
}

GenericAnalogOutImpl::~GenericAnalogOutImpl()
{
	m_devices_out.clear();
}

std::shared_ptr<libm2k::utils::DeviceOut> GenericAnalogOutImpl::getDacDevice(unsigned int index)
{
	if (index >= m_devices_out.size()) {
		throw_exception(EXC_INVALID_PARAMETER, "No such DAC device");
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
	return getDacDevice(0)->getAvailableSampleRates();
}

void GenericAnalogOutImpl::setCyclic(bool en)
{
	for (unsigned int i = 0; i < getDacDevice(0)->getNbChannels(true); i++) {
		m_cyclic.at(i) = en;
	}
	getDacDevice(0)->setCyclic(en);
}

void GenericAnalogOutImpl::setCyclic(unsigned int chn, bool en)
{
	if (chn >= getDacDevice(0)->getNbChannels(true)) {
		throw_exception(EXC_INVALID_PARAMETER, "Generic Analog Out: No such channel");
	}
	m_cyclic.at(chn) = en;
}

bool GenericAnalogOutImpl::getCyclic(unsigned int chn)
{
	if (chn >= getDacDevice(0)->getNbChannels(true)) {
		throw_exception(EXC_INVALID_PARAMETER, "Generic Analog Out: No such channel");
	}
	return m_cyclic.at(chn);
}

void GenericAnalogOutImpl::pushRaw(unsigned int chn_idx, short *data, unsigned int nb_samples)
{
	getDacDevice(0)->push(data, chn_idx, nb_samples, getCyclic(chn_idx));
}

void GenericAnalogOutImpl::push(unsigned int chn_idx, double *data, unsigned int nb_samples)
{
	getDacDevice(0)->push(data, chn_idx, nb_samples, getCyclic(chn_idx));
}

void GenericAnalogOutImpl::pushRaw(unsigned int chn_idx, std::vector<short> const &data)
{
	getDacDevice(0)->push(data, chn_idx, getCyclic(chn_idx));
}

void GenericAnalogOutImpl::push(unsigned int chn_idx, std::vector<double> const &data)
{
	getDacDevice(0)->push(data, chn_idx, getCyclic(chn_idx));
}

void GenericAnalogOutImpl::stop()
{
	getDacDevice(0)->stop();
}

string GenericAnalogOutImpl::getName()
{
	return getDacDevice(0)->getName();
}

void GenericAnalogOutImpl::enableChannel(unsigned int chnIdx, bool enable)
{
	getDacDevice(0)->enableChannel(chnIdx, enable, true);
}
