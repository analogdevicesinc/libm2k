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

#include "private/genericanalogout_impl.cpp"

using namespace std;
using namespace libm2k::analog;
using namespace libm2k::utils;

GenericAnalogOut::GenericAnalogOut(iio_context *ctx, std::string dac_dev):
	m_pimpl(std::unique_ptr<GenericAnalogOutImpl>(new GenericAnalogOutImpl(ctx, dac_dev)))
{
}

GenericAnalogOut::~GenericAnalogOut()
{

}
double GenericAnalogOut::getSampleRate()
{
	return m_pimpl->getSampleRate();
}

double GenericAnalogOut::getSampleRate(unsigned int chn_idx)
{
	return m_pimpl->getSampleRate(chn_idx);
}

double GenericAnalogOut::setSampleRate(double sampleRate)
{
	return m_pimpl->setSampleRate(sampleRate);
}

double GenericAnalogOut::setSampleRate(unsigned int chn_idx, double sampleRate)
{
	return m_pimpl->setSampleRate(chn_idx, sampleRate);
}

std::vector<double> GenericAnalogOut::getAvailableSampleRates()
{
	return m_pimpl->getAvailableSampleRates();
}

void GenericAnalogOut::setCyclic(bool en)
{
	m_pimpl->setCyclic(en);
}

void GenericAnalogOut::setCyclic(unsigned int chn, bool en)
{
	m_pimpl->setCyclic(chn, en);
}

bool GenericAnalogOut::getCyclic(unsigned int chn)
{
	return m_pimpl->getCyclic(chn);
}

void GenericAnalogOut::pushRaw(unsigned int chn_idx, std::vector<short> const &data)
{
	m_pimpl->push(data, chn_idx);
}

void GenericAnalogOut::push(unsigned int chn_idx, std::vector<double> const &data)
{
	m_pimpl->push(data, chn_idx);
}

void GenericAnalogOut::push(unsigned int chn_idx, double *data, unsigned int nb_samples)
{
	m_pimpl->push(data, chn_idx, nb_samples);
}

void GenericAnalogOut::pushRaw(unsigned int chn_idx, short *data, unsigned int nb_samples)
{
	m_pimpl->push(data, chn_idx, nb_samples);
}

void GenericAnalogOut::stop()
{
	m_pimpl->stop();
}

std::string GenericAnalogOut::getName()
{
	return m_pimpl->getName();
}

void GenericAnalogOut::enableChannel(unsigned int chnIdx, bool enable)
{
	m_pimpl->enableChannel(chnIdx, enable, true);
}
