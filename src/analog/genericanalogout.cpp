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
	Device(new GenericAnalogOutImpl(ctx, dac_dev))
{
	m_pimpl = dynamic_pointer_cast<GenericAnalogOutImpl>(Device::m_pimpl);
}

GenericAnalogOut::~GenericAnalogOut()
{

}
double GenericAnalogOut::getSamplerate()
{
	return m_pimpl->getSamplerate();
}

double GenericAnalogOut::getSamplerate(unsigned int chn_idx)
{
	return m_pimpl->getSamplerate(chn_idx);
}

double GenericAnalogOut::setSamplerate(double sampleRate)
{
	return m_pimpl->setSamplerate(sampleRate);
}

double GenericAnalogOut::setSamplerate(unsigned int chn_idx, double sampleRate)
{
	return m_pimpl->setSamplerate(chn_idx, sampleRate);
}

std::vector<double> GenericAnalogOut::getAvailableSamplerates()
{
	return m_pimpl->getAvailableSamplerates();
}

void GenericAnalogOut::setCyclic(bool en)
{
	m_pimpl->setCyclic(en);
}

void GenericAnalogOut::setCyclic(bool en, unsigned int chn)
{
	m_pimpl->setCyclic(en, chn);
}

bool GenericAnalogOut::getCyclic(unsigned int chn)
{
	return m_pimpl->getCyclic(chn);
}

void GenericAnalogOut::push(std::vector<short> &data, unsigned int chn_idx)
{
	m_pimpl->push(data, chn_idx);
}

void GenericAnalogOut::push(std::vector<double> &data, unsigned int chn_idx)
{
	m_pimpl->push(data, chn_idx);
}

void libm2k::analog::GenericAnalogOut::openAnalogOut()
{
	m_pimpl->openAnalogOut();
}

void libm2k::analog::GenericAnalogOut::closeAnalogOut()
{
	m_pimpl->closeAnalogOut();
}

void GenericAnalogOut::stop()
{
	m_pimpl->stop();
}
