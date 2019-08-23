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

#include "private/genericanalogin_impl.cpp"

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

double *GenericAnalogIn::getSamplesInterleaved(unsigned int nb_samples)
{
	return m_pimpl->getSamplesInterleaved(nb_samples, true);
}

short *GenericAnalogIn::getSamplesRawInterleaved(unsigned int nb_samples)
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

void GenericAnalogIn::enableChannel(unsigned int index, bool enable)
{
	m_pimpl->enableChannel(index, enable);
}

void GenericAnalogIn::setKernelBuffersCount(unsigned int count)
{
	m_pimpl->setKernelBuffersCount(count);
}
