﻿/*
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

M2kAnalogOut::M2kAnalogOut(iio_context *ctx, std::vector<std::string> dac_devs, bool sync) :
	m_pimpl(std::unique_ptr<M2kAnalogOutImpl>(new M2kAnalogOutImpl(ctx, dac_devs, sync)))
{
}

M2kAnalogOut::~M2kAnalogOut()
{
}

void M2kAnalogOut::init()
{
	m_pimpl->init();
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

std::vector<double> M2kAnalogOut::getSampleRate()
{
	return m_pimpl->getSampleRate();
}

double M2kAnalogOut::getSampleRate(unsigned int chn_idx)
{
	return m_pimpl->getSampleRate(chn_idx);
}

std::vector<double> M2kAnalogOut::setSampleRate(std::vector<double> samplerates)
{
	return m_pimpl->setSampleRate(samplerates);
}

double M2kAnalogOut::setSampleRate(unsigned int chn_idx, double samplerate)
{
	return m_pimpl->setSampleRate(chn_idx, samplerate);
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

void M2kAnalogOut::setCyclic(unsigned int chn, bool en)
{
	m_pimpl->setCyclic(chn, en);
}

bool M2kAnalogOut::getCyclic(unsigned int chn)
{
	return m_pimpl->getCyclic(chn);
}

double M2kAnalogOut::setCalibscale(unsigned int index, double calibscale)
{
	return m_pimpl->setCalibscale(index, calibscale);
}

double M2kAnalogOut::getCalibscale(unsigned int index)
{
	return m_pimpl->getCalibscale(index);
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

void M2kAnalogOut::pushRaw(unsigned int chnIdx, std::vector<short> const &data)
{
	m_pimpl->pushRaw(data, chnIdx);
}

void M2kAnalogOut::push(unsigned int chnIdx, std::vector<double> const &data)
{
	m_pimpl->push(data, chnIdx);
}

void M2kAnalogOut::pushRaw(std::vector<std::vector<short>> const &data)
{
	m_pimpl->pushRaw(data);
}

void M2kAnalogOut::push(unsigned int chnIdx, double *data, unsigned int nb_samples)
{
	m_pimpl->push(chnIdx, data, nb_samples);
}

void M2kAnalogOut::pushRaw(unsigned int chnIdx, short *data, unsigned int nb_samples)
{
	m_pimpl->pushRaw(chnIdx, data, nb_samples);
}

void M2kAnalogOut::pushInterleaved(double *data, unsigned int nb_channels, unsigned int nb_samples_per_channel)
{
	m_pimpl->push(data, nb_channels, nb_samples_per_channel);
}

void M2kAnalogOut::pushRawInterleaved(short *data, unsigned int nb_channels, unsigned int nb_samples_per_channel)
{
	m_pimpl->pushRaw(data, nb_channels, nb_samples_per_channel);
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

bool M2kAnalogOut::isChannelEnabled(unsigned int chnIdx)
{
	return m_pimpl->isChannelEnabled(chnIdx);
}
