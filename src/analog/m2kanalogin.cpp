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
			 std::string adc_dev) :
	Device(new M2kAnalogInImpl(ctx, adc_dev))
{
	m_pimpl = dynamic_pointer_cast<M2kAnalogInImpl>(Device::m_pimpl);
}

M2kAnalogIn::~M2kAnalogIn()
{
}

void M2kAnalogIn::setAdcCalibGain(ANALOG_IN_CHANNEL channel, double gain)
{
	m_pimpl->setAdcCalibGain(channel, gain);
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

void M2kAnalogIn::setStreamingFlag(bool en)
{
	m_pimpl->setStreamingFlag(en);
}

bool M2kAnalogIn::getStreamingFlag()
{
	return m_pimpl->getStreamingFlag();
}

M2kHardwareTrigger *M2kAnalogIn::getTrigger()
{
	return m_pimpl->getTrigger();
}

std::vector<std::vector<double>> M2kAnalogIn::getSamples(unsigned int nb_samples,
							 bool processed)
{
	return m_pimpl->getSamples(nb_samples, processed);
}

double M2kAnalogIn::processSample(int16_t sample, unsigned int channel)
{
	return m_pimpl->processSample(sample, channel);
}

uint16_t M2kAnalogIn::getVoltageRaw(unsigned int ch)
{
	return m_pimpl->getVoltageRaw(ch);
}

uint16_t M2kAnalogIn::getVoltageRaw(ANALOG_IN_CHANNEL ch)
{
	return m_pimpl->getVoltageRaw(ch);
}

std::vector<uint16_t> M2kAnalogIn::getVoltageRaw()
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

double M2kAnalogIn::getScalingFactor(ANALOG_IN_CHANNEL ch)
{
	return m_pimpl->getScalingFactor(ch);
}

void M2kAnalogIn::openAnalogIn()
{
	std::cout << "Opened analog in for " << getName() << "\n";
}

void M2kAnalogIn::closeAnalogIn()
{
	std::cout << "Opened analog in for " << getName() << "\n";
}

int M2kAnalogIn::getDelay()
{
	return m_pimpl->getDelay();
}

void M2kAnalogIn::setDelay(int delay)
{
	m_pimpl->setDelay(delay);
}

int M2kAnalogIn::getLevel(ANALOG_IN_CHANNEL chnIdx) const
{
	return m_pimpl->getLevel(chnIdx);
}

void M2kAnalogIn::setLevel(ANALOG_IN_CHANNEL chnIdx, int level)
{
	m_pimpl->setLevel(chnIdx, level);
}

int M2kAnalogIn::getHysteresis(ANALOG_IN_CHANNEL chnIdx) const
{
	return m_pimpl->getHysteresis(chnIdx);
}

void M2kAnalogIn::setHysteresis(ANALOG_IN_CHANNEL chnIdx, int hysteresis)
{
	m_pimpl->setHysteresis(chnIdx, hysteresis);
}

std::pair<double, double> M2kAnalogIn::getHysteresisRange(ANALOG_IN_CHANNEL chn)
{
	return m_pimpl->getHysteresisRange(chn);
}

M2K_TRIGGER_CONDITION M2kAnalogIn::getAnalogCondition(ANALOG_IN_CHANNEL chnIdx) const
{
	return m_pimpl->getAnalogCondition(chnIdx);
}

void M2kAnalogIn::setAnalogCondition(ANALOG_IN_CHANNEL chnIdx, M2K_TRIGGER_CONDITION cond)
{
	m_pimpl->setAnalogCondition(chnIdx, cond);
}

M2K_TRIGGER_CONDITION M2kAnalogIn::getDigitalCondition(ANALOG_IN_CHANNEL chnIdx) const
{
	return m_pimpl->getDigitalCondition(chnIdx);
}

void M2kAnalogIn::setDigitalCondition(ANALOG_IN_CHANNEL chnIdx, M2K_TRIGGER_CONDITION cond)
{
	m_pimpl->setDigitalCondition(chnIdx, cond);
}

M2K_TRIGGER_SOURCE M2kAnalogIn::getSource() const
{
	return m_pimpl->getSource();
}

void M2kAnalogIn::setSource(M2K_TRIGGER_SOURCE src)
{
	m_pimpl->setSource(src);
}

void M2kAnalogIn::setSourceChannel(ANALOG_IN_CHANNEL channel)
{
	m_pimpl->setSourceChannel(channel);
}

ANALOG_IN_CHANNEL M2kAnalogIn::getSourceChannel()
{
	return m_pimpl->getSourceChannel();
}

void M2kAnalogIn::setTriggerMode(ANALOG_IN_CHANNEL channel,
				 M2K_TRIGGER_MODE mode)
{
	m_pimpl->setTriggerMode(channel, mode);
}

M2K_TRIGGER_MODE M2kAnalogIn::getTriggerMode(ANALOG_IN_CHANNEL channel)
{
	return m_pimpl->getTriggerMode(channel);
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

std::vector<M2K_RANGE> M2kAnalogIn::getAvailableRanges()
{
	return m_pimpl->getAvailableRanges();
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
