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

#include "private/genericanalogin_impl.cpp"
#include "libm2k/enums.hpp"

using namespace libm2k::utils;
using namespace libm2k::analog;
using namespace std;

GenericAnalogIn::GenericAnalogIn(iio_context *ctx, std::string adc_dev) :
	m_pimpl(std::unique_ptr<GenericAnalogInImpl>(new GenericAnalogInImpl(ctx, adc_dev)))
{
}

GenericAnalogIn::~GenericAnalogIn()
{
}

std::vector<std::vector<double> > GenericAnalogIn::getSamplesRaw(unsigned int nb_samples)
{
	return m_pimpl->getSamples(nb_samples, false);
}

const double *GenericAnalogIn::getSamplesInterleaved(unsigned int nb_samples)
{
	return m_pimpl->getSamplesInterleaved(nb_samples, true);
}

const short *GenericAnalogIn::getSamplesRawInterleaved(unsigned int nb_samples)
{
	return m_pimpl->getSamplesRawInterleaved(nb_samples);
}

std::vector<std::vector<double>> GenericAnalogIn::getSamples(unsigned int nb_samples)
{
	return m_pimpl->getSamples(nb_samples, true);
}

double GenericAnalogIn::getSampleRate()
{
	return m_pimpl->getSampleRate();
}

double GenericAnalogIn::getSampleRate(unsigned int chn_idx)
{
	return m_pimpl->getSampleRate(chn_idx);
}

double GenericAnalogIn::setSampleRate(double sampleRate)
{
	return m_pimpl->setSampleRate(sampleRate);
}

double GenericAnalogIn::setSampleRate(unsigned int chn_idx, double sampleRate)
{
	return m_pimpl->setSampleRate(chn_idx, sampleRate);
}

std::vector<double> GenericAnalogIn::getAvailableSampleRates()
{
	return m_pimpl->getAvailableSampleRates();
}

string GenericAnalogIn::getDeviceName()
{
	return m_pimpl->getDeviceName();
}

libm2k::IIO_OBJECTS GenericAnalogIn::getIioObjects()
{
	return m_pimpl->getIioObjects();
}

void GenericAnalogIn::enableChannel(unsigned int index, bool enable)
{
	m_pimpl->enableChannel(index, enable);
}

void GenericAnalogIn::setKernelBuffersCount(unsigned int count)
{
	m_pimpl->setKernelBuffersCount(count);
}
