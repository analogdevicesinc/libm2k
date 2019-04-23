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

#include "private/m2kanalogout_impl.cpp"

using namespace libm2k::analog;
using namespace libm2k::utils;
using namespace std;

M2kAnalogOut::M2kAnalogOut(iio_context *ctx, std::vector<std::string> dac_devs):
	Device(new M2kAnalogOutImpl(ctx, dac_devs))
{
	m_pimpl = dynamic_pointer_cast<M2kAnalogOutImpl>(Device::m_pimpl);
}

M2kAnalogOut::~M2kAnalogOut()
{
}

void M2kAnalogOut::openAnalogOut()
{
	m_pimpl->openAnalogOut();
}

void M2kAnalogOut::closeAnalogOut()
{
	m_pimpl->closeAnalogOut();
}

std::vector<double> M2kAnalogOut::getOversamplingRatio()
{
	return m_pimpl->getOversamplingRatio();
}

double M2kAnalogOut::getOversamplingRatio(unsigned int chn_idx)
{
	return m_pimpl->getOversamplingRatio(chn_idx);
}

std::vector<double> M2kAnalogOut::setOversamplingRatio(std::vector<double> oversampling_ratio)
{
	return m_pimpl->setOversamplingRatio(oversampling_ratio);
}

double M2kAnalogOut::setOversamplingRatio(unsigned int chn_idx, double oversampling_ratio)
{
	return m_pimpl->setOversamplingRatio(chn_idx, oversampling_ratio);
}

std::vector<double> M2kAnalogOut::getSamplerate()
{
	return m_pimpl->getSamplerate();
}

double M2kAnalogOut::getSamplerate(unsigned int chn_idx)
{
	return m_pimpl->getSamplerate(chn_idx);
}

std::vector<double> M2kAnalogOut::setSamplerate(std::vector<double> samplerates)
{
	return m_pimpl->setSamplerate(samplerates);
}

double M2kAnalogOut::setSamplerate(unsigned int chn_idx, double samplerate)
{
	return m_pimpl->setSamplerate(chn_idx, samplerate);
}

void M2kAnalogOut::setSyncedDma(bool en, int chn)
{
	m_pimpl->setSyncedDma(en, chn);
}

bool M2kAnalogOut::getSyncedDma(int chn)
{
	return m_pimpl->getSyncedDma(chn);
}

void M2kAnalogOut::setCyclic(bool en)
{
	m_pimpl->setCyclic(en);
}

void M2kAnalogOut::setCyclic(bool en, unsigned int chn)
{
	m_pimpl->setCyclic(en, chn);
}

bool M2kAnalogOut::getCyclic(unsigned int chn)
{
	return m_pimpl->getCyclic(chn);
}

int M2kAnalogOut::convertVoltsToRaw(double voltage, double vlsb,
				       double filterCompensation)
{
	return m_pimpl->convertVoltsToRaw(voltage, vlsb, filterCompensation);
}


void M2kAnalogOut::setDacCalibVlsb(unsigned int chn_idx, double vlsb)
{
	m_pimpl->setDacCalibVlsb(chn_idx, vlsb);
}

void M2kAnalogOut::push(std::vector<short> const &data, unsigned int chnIdx)
{
	m_pimpl->push(data, chnIdx);
}

void M2kAnalogOut::push(std::vector<double> const &data, unsigned int chnIdx)
{
	m_pimpl->push(data, chnIdx);
}

void M2kAnalogOut::push(std::vector<std::vector<short>> const &data)
{
	m_pimpl->push(data);
}

void M2kAnalogOut::push(std::vector<std::vector<double>> const &data)
{
	m_pimpl->push(data);
}

double M2kAnalogOut::getScalingFactor(unsigned int chn)
{
	return m_pimpl->getScalingFactor(chn);
}

double M2kAnalogOut::getFilterCompensation(double samplerate)
{
	return m_pimpl->getFilterCompensation(samplerate);
}

void M2kAnalogOut::stop()
{
	m_pimpl->stop();
}

void M2kAnalogOut::stop(unsigned int chn)
{
	m_pimpl->stop(chn);
}

void M2kAnalogOut::enableChannel(unsigned int chnIdx, bool enable)
{
	m_pimpl->enableChannel(chnIdx, enable);
}
