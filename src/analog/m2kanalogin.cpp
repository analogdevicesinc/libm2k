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

#include "private/m2kanalogin_impl.cpp"

using namespace libm2k::analog;
using namespace libm2k::utils;
using namespace std;
using namespace std::placeholders;

#define HIGH_MAX 2.5
#define HIGH_MIN -2.5
#define LOW_MAX 25
#define LOW_MIN -25

M2kAnalogIn::M2kAnalogIn(iio_context * ctx,
			 std::string adc_dev, bool sync) :
	m_pimpl(std::unique_ptr<M2kAnalogInImpl>(new M2kAnalogInImpl(ctx, adc_dev, sync)))
{
}

M2kAnalogIn::~M2kAnalogIn()
{
}

void M2kAnalogIn::init()
{
	m_pimpl->init();
}

void M2kAnalogIn::setAdcCalibGain(ANALOG_IN_CHANNEL channel, double gain)
{
	m_pimpl->setAdcCalibGain(channel, gain);
}

void M2kAnalogIn::setAdcCalibOffset(ANALOG_IN_CHANNEL channel, int calib_offset)
{
	m_pimpl->setAdcCalibOffset(channel, calib_offset);
}

double M2kAnalogIn::convertRawToVolts(int sample, double correctionGain,
		double hw_gain, double filterCompensation, double offset)
{
	return m_pimpl->convertRawToVolts(sample, correctionGain,
					  hw_gain, filterCompensation, offset);
}

int M2kAnalogIn::convertVoltsToRaw(double voltage, double correctionGain,
		double hw_gain, double filterCompensation, double offset)
{
	return m_pimpl->convertVoltsToRaw(voltage, correctionGain,
					  hw_gain, filterCompensation, offset);
}

unsigned int M2kAnalogIn::getNbChannels()
{
	return m_pimpl->getNbChannels();
}

string M2kAnalogIn::getName()
{
	return m_pimpl->getName();
}

void M2kAnalogIn::enableChannel(unsigned int chnIdx, bool enable)
{
	m_pimpl->enableChannel(chnIdx, enable);
}

bool M2kAnalogIn::isChannelEnabled(unsigned int chnIdx)
{
	return m_pimpl->isChannelEnabled(chnIdx);
}

void M2kAnalogIn::convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src)
{
	m_pimpl->convertChannelHostFormat(chn_idx, avg, src);
}

void M2kAnalogIn::convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src)
{
	m_pimpl->convertChannelHostFormat(chn_idx, avg, src);
}

double M2kAnalogIn::setCalibscale(unsigned int index, double calibscale)
{
	return m_pimpl->setCalibscale(index, calibscale);
}

double M2kAnalogIn::getCalibscale(unsigned int index)
{
	return m_pimpl->getCalibscale(index);
}

M2kHardwareTrigger *M2kAnalogIn::getTrigger()
{
	return m_pimpl->getTrigger();
}

void M2kAnalogIn::flushBuffer()
{
	m_pimpl->flushBuffer();
}

std::vector<std::vector<double>> M2kAnalogIn::getSamples(unsigned int nb_samples)
{
	return m_pimpl->getSamples(nb_samples, true);
}

std::vector<std::vector<double> > M2kAnalogIn::getSamplesRaw(unsigned int nb_samples)
{
	return m_pimpl->getSamples(nb_samples, false);
}

double *M2kAnalogIn::getSamplesInterleaved(unsigned int nb_samples)
{
	return m_pimpl->getSamplesInterleaved(nb_samples, true);
}

short *M2kAnalogIn::getSamplesRawInterleaved(unsigned int nb_samples)
{
	return m_pimpl->getSamplesRawInterleaved(nb_samples);
}

double M2kAnalogIn::processSample(int16_t sample, unsigned int channel)
{
	return m_pimpl->processSample(sample, channel);
}

short M2kAnalogIn::getVoltageRaw(unsigned int ch)
{
	return m_pimpl->getVoltageRaw(ch);
}

short M2kAnalogIn::getVoltageRaw(ANALOG_IN_CHANNEL ch)
{
	return m_pimpl->getVoltageRaw(ch);
}

std::vector<short> M2kAnalogIn::getVoltageRaw()
{
	return m_pimpl->getVoltageRaw();
}

double M2kAnalogIn::getVoltage(unsigned int ch)
{
	return m_pimpl->getVoltage(ch);
}

double M2kAnalogIn::getVoltage(ANALOG_IN_CHANNEL ch)
{
	return m_pimpl->getVoltage(ch);
}

std::vector<double> M2kAnalogIn::getVoltage()
{
	return m_pimpl->getVoltage();
}

short *M2kAnalogIn::getVoltageRawP()
{
	return m_pimpl->getVoltageRawP();
}

double *M2kAnalogIn::getVoltageP()
{
	return m_pimpl->getVoltageP();
}

double M2kAnalogIn::getScalingFactor(ANALOG_IN_CHANNEL ch)
{
	return m_pimpl->getScalingFactor(ch);
}

std::pair<double, double> M2kAnalogIn::getHysteresisRange(ANALOG_IN_CHANNEL chn)
{
	return m_pimpl->getHysteresisRange(chn);
}

void M2kAnalogIn::setRange(ANALOG_IN_CHANNEL channel, M2K_RANGE range)
{
	m_pimpl->setRange(channel, range);
}

void M2kAnalogIn::setRange(ANALOG_IN_CHANNEL channel, double min, double max)
{
	m_pimpl->setRange(channel, min, max);
}

M2K_RANGE M2kAnalogIn::getRange(ANALOG_IN_CHANNEL channel)
{
	return m_pimpl->getRange(channel);
}

std::pair<double, double> M2kAnalogIn::getRangeLimits(M2K_RANGE range)
{
	return m_pimpl->getRangeLimits(range);
}

std::vector<std::pair<std::string, std::pair<double, double>>> M2kAnalogIn::getAvailableRanges()
{
	return m_pimpl->getAvailableRanges();
}

void M2kAnalogIn::setVerticalOffset(ANALOG_IN_CHANNEL channel, double vertOffset)
{
	m_pimpl->setVerticalOffset(channel, vertOffset);
}

double M2kAnalogIn::getVerticalOffset(ANALOG_IN_CHANNEL channel)
{
	return m_pimpl->getVerticalOffset(channel);
}

double M2kAnalogIn::getOversamplingRatio()
{
	return m_pimpl->getOversamplingRatio();
}

double M2kAnalogIn::getOversamplingRatio(unsigned int chn_idx)
{
	return m_pimpl->getOversamplingRatio(chn_idx);
}

double M2kAnalogIn::setOversamplingRatio(double oversampling_ratio)
{
	return m_pimpl->setOversamplingRatio(oversampling_ratio);
}

double M2kAnalogIn::setOversamplingRatio(unsigned int chn_idx, double oversampling_ratio)
{
	return m_pimpl->setOversamplingRatio(chn_idx, oversampling_ratio);
}

double M2kAnalogIn::getSampleRate()
{
	return m_pimpl->getSampleRate();
}

double M2kAnalogIn::getSampleRate(unsigned int chn_idx)
{
	return m_pimpl->getSampleRate(chn_idx);
}

double M2kAnalogIn::setSampleRate(double samplerate)
{
	return m_pimpl->setSampleRate(samplerate);
}

double M2kAnalogIn::setSampleRate(unsigned int chn_idx, double samplerate)
{
	return m_pimpl->setSampleRate(chn_idx, samplerate);
}

double M2kAnalogIn::getFilterCompensation(double samplerate)
{
	return m_pimpl->getFilterCompensation(samplerate);
}

double M2kAnalogIn::getValueForRange(M2K_RANGE range)
{
	return m_pimpl->getValueForRange(range);
}

void M2kAnalogIn::setKernelBuffersCount(unsigned int count)
{
	return m_pimpl->setKernelBuffersCount(count);
}
