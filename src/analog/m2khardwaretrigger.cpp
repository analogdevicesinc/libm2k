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

#include "private/m2khardwaretrigger_impl.cpp"

using namespace libm2k::analog;
using namespace std;

typedef std::pair<Channel *, std::string> channel_pair;

M2kHardwareTrigger::M2kHardwareTrigger(struct iio_context *ctx) :
	Device(new M2kHardwareTriggerImpl(ctx))
{
	m_pimpl = dynamic_pointer_cast<M2kHardwareTriggerImpl>(Device::m_pimpl);
}

M2kHardwareTrigger::~M2kHardwareTrigger()
{
}

M2K_TRIGGER_CONDITION M2kHardwareTrigger::getAnalogCondition(unsigned int chnIdx)
{
	return m_pimpl->getAnalogCondition(chnIdx);
}

void M2kHardwareTrigger::setAnalogCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION cond)
{
	m_pimpl->setAnalogCondition(chnIdx, cond);
}

M2K_TRIGGER_CONDITION M2kHardwareTrigger::getDigitalCondition(unsigned int chnIdx)
{
	return m_pimpl->getDigitalCondition(chnIdx);
}

void M2kHardwareTrigger::setDigitalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION cond)
{
	m_pimpl->setDigitalCondition(chnIdx, cond);
}

int M2kHardwareTrigger::getLevelRaw(unsigned int chnIdx)
{
	return m_pimpl->getLevelRaw(chnIdx);
}

void M2kHardwareTrigger::setLevelRaw(unsigned int chnIdx, int level)
{
	m_pimpl->setLevelRaw(chnIdx, level);
}

double M2kHardwareTrigger::getLevel(unsigned int chnIdx)
{
	m_pimpl->getLevel(chnIdx);
}

void M2kHardwareTrigger::setLevel(unsigned int chnIdx, double v_level)
{
	m_pimpl->setLevel(chnIdx, v_level);
}

int M2kHardwareTrigger::getHysteresis(unsigned int chnIdx)
{
	return m_pimpl->getHysteresis(chnIdx);
}

void M2kHardwareTrigger::setHysteresis(unsigned int chnIdx, int histeresis)
{
	m_pimpl->setHysteresis(chnIdx, histeresis);
}

M2K_TRIGGER_MODE M2kHardwareTrigger::getTriggerMode(unsigned int chnIdx)
{
	return m_pimpl->getTriggerMode(chnIdx);
}

void M2kHardwareTrigger::setTriggerMode(unsigned int chnIdx, M2K_TRIGGER_MODE mode)
{
	m_pimpl->setTriggerMode(chnIdx, mode);
}

M2K_TRIGGER_SOURCE M2kHardwareTrigger::getSource()
{
	return m_pimpl->getSource();
}

void M2kHardwareTrigger::setSource(M2K_TRIGGER_SOURCE src)
{
	m_pimpl->setSource(src);
}

/*
 * Convenience function to be used when willing to use the trigger for only one
 * channel at a time.
 */
int M2kHardwareTrigger::getSourceChannel()
{
	return m_pimpl->getSourceChannel();
}

/*
 * Convenience function to be used when willing to enable the trigger for only
 * one channel at a time.
 */
void M2kHardwareTrigger::setSourceChannel(unsigned int chnIdx)
{
	m_pimpl->setSourceChannel(chnIdx);
}

int M2kHardwareTrigger::getDelay() const
{
	return m_pimpl->getDelay();
}

void M2kHardwareTrigger::setDelay(int delay)
{
	m_pimpl->setDelay(delay);
}

void M2kHardwareTrigger::setStreamingFlag(bool val)
{
	m_pimpl->setStreamingFlag(val);
}

bool M2kHardwareTrigger::getStreamingFlag()
{
	return m_pimpl->getStreamingFlag();
}

std::vector<string> M2kHardwareTrigger::getAvailableDigitalConditions()
{
	return M2kHardwareTriggerImpl::getAvailableDigitalConditions();
}

void M2kHardwareTrigger::setCalibParameters(unsigned int chnIdx, double scaling, double offset)
{
	m_pimpl->setCalibParameters(chnIdx, scaling, offset);
}

settings_uptr M2kHardwareTrigger::getCurrentHwSettings()
{
	return m_pimpl->getCurrentHwSettings();
}

void M2kHardwareTrigger::setHwTriggerSettings(struct SETTINGS *settings)
{
	m_pimpl->setHwTriggerSettings(settings);
}
