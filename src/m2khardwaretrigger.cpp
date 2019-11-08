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

#include "private/m2khardwaretrigger_v0.24_impl.cpp"

using namespace libm2k;
using namespace std;

typedef std::pair<Channel *, std::string> channel_pair;

M2kHardwareTrigger::M2kHardwareTrigger(struct iio_context *ctx, const std::string& fw_version)
{
	int diff = Utils::compareVersions(fw_version, "v0.24");
	if (diff > 0) {	//fw_ver < 0.24
		m_pimpl = std::unique_ptr<M2kHardwareTriggerImpl>(new M2kHardwareTriggerImpl(ctx));
	} else {
		m_pimpl = std::unique_ptr<M2kHardwareTriggerV024Impl>(new M2kHardwareTriggerV024Impl(ctx));
	}
}

M2kHardwareTrigger::~M2kHardwareTrigger()
{
}

M2K_TRIGGER_CONDITION_ANALOG M2kHardwareTrigger::getAnalogCondition(unsigned int chnIdx)
{
	return m_pimpl->getAnalogCondition(chnIdx);
}

void M2kHardwareTrigger::setAnalogCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION_ANALOG cond)
{
	m_pimpl->setAnalogCondition(chnIdx, cond);
}


M2K_TRIGGER_CONDITION_DIGITAL M2kHardwareTrigger::getDigitalExternalCondition()
{
	return m_pimpl->getDigitalExternalCondition();
}

void M2kHardwareTrigger::setDigitalExternalCondition(M2K_TRIGGER_CONDITION_DIGITAL cond)
{
	m_pimpl->setDigitalExternalCondition(cond);
}

M2K_TRIGGER_CONDITION_DIGITAL M2kHardwareTrigger::getAnalogExternalCondition(unsigned int chnIdx)
{
	return m_pimpl->getAnalogExternalCondition(chnIdx);
}

void M2kHardwareTrigger::setAnalogExternalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION_DIGITAL cond)
{
	m_pimpl->setAnalogExternalCondition(chnIdx, cond);
}

void M2kHardwareTrigger::setDigitalSource(M2K_TRIGGER_SOURCE_DIGITAL external_src)
{
	m_pimpl->setDigitalSource(external_src);
}

M2K_TRIGGER_SOURCE_DIGITAL M2kHardwareTrigger::getDigitalSource() const
{
	return m_pimpl->getDigitalSource();
}

M2K_TRIGGER_OUT_SELECT M2kHardwareTrigger::getAnalogExternalOutSelect()
{
	return m_pimpl->getAnalogExternalOutSelect();
}

bool M2kHardwareTrigger::hasExternalTriggerIn() const
{
	return m_pimpl->hasExternalTriggerIn();
}

bool M2kHardwareTrigger::hasExternalTriggerOut() const
{
	return m_pimpl->hasExternalTriggerOut();
}

bool M2kHardwareTrigger::hasCrossInstrumentTrigger() const
{
	return m_pimpl->hasCrossIntrumentTrigger();
}

void M2kHardwareTrigger::setAnalogExternalOutSelect(M2K_TRIGGER_OUT_SELECT output_select)
{
	m_pimpl->setAnalogExternalOutSelect(output_select);
}

M2K_TRIGGER_CONDITION_DIGITAL M2kHardwareTrigger::getDigitalCondition(unsigned int chnIdx)
{
	return m_pimpl->getDigitalCondition(chnIdx);
}

void M2kHardwareTrigger::setDigitalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION_DIGITAL cond)
{
	m_pimpl->setDigitalCondition(chnIdx, cond);
}

M2K_TRIGGER_MODE M2kHardwareTrigger::getAnalogMode(unsigned int chnIdx)
{
	return m_pimpl->getAnalogMode(chnIdx);
}

void M2kHardwareTrigger::setAnalogMode(unsigned int chnIdx, M2K_TRIGGER_MODE mode)
{
	m_pimpl->setAnalogMode(chnIdx, mode);
}

DIO_TRIGGER_MODE M2kHardwareTrigger::getDigitalMode()
{
	return m_pimpl->getDigitalMode();
}

void M2kHardwareTrigger::setDigitalMode(DIO_TRIGGER_MODE mode)
{
	m_pimpl->setDigitalMode(mode);
}

int M2kHardwareTrigger::getAnalogLevelRaw(unsigned int chnIdx)
{
	return m_pimpl->getAnalogLevelRaw(chnIdx);
}

void M2kHardwareTrigger::setAnalogLevelRaw(unsigned int chnIdx, int level)
{
	m_pimpl->setAnalogLevelRaw(chnIdx, level);
}

double M2kHardwareTrigger::getAnalogLevel(unsigned int chnIdx)
{
	return m_pimpl->getAnalogLevel(chnIdx);
}

void M2kHardwareTrigger::setAnalogLevel(unsigned int chnIdx, double v_level)
{
	m_pimpl->setAnalogLevel(chnIdx, v_level);
}

int M2kHardwareTrigger::getAnalogHysteresis(unsigned int chnIdx)
{
	return m_pimpl->getAnalogHysteresis(chnIdx);
}

void M2kHardwareTrigger::setAnalogHysteresis(unsigned int chnIdx, int histeresis)
{
	m_pimpl->setAnalogHysteresis(chnIdx, histeresis);
}

M2K_TRIGGER_SOURCE_ANALOG M2kHardwareTrigger::getAnalogSource()
{
	return m_pimpl->getAnalogSource();
}

void M2kHardwareTrigger::setAnalogSource(M2K_TRIGGER_SOURCE_ANALOG src)
{
	m_pimpl->setAnalogSource(src);
}

/*
 * Convenience function to be used when willing to use the trigger for only one
 * channel at a time.
 */
int M2kHardwareTrigger::getAnalogSourceChannel()
{
	return m_pimpl->getAnalogSourceChannel();
}

/*
 * Convenience function to be used when willing to enable the trigger for only
 * one channel at a time.
 */
void M2kHardwareTrigger::setAnalogSourceChannel(unsigned int chnIdx)
{
	m_pimpl->setAnalogSourceChannel(chnIdx);
}

int M2kHardwareTrigger::getAnalogDelay() const
{
	return m_pimpl->getAnalogDelay();
}

void M2kHardwareTrigger::setAnalogDelay(int delay)
{
	m_pimpl->setAnalogDelay(delay);
}

int M2kHardwareTrigger::getDigitalDelay() const
{
	return m_pimpl->getDigitalDelay();
}

void M2kHardwareTrigger::setDigitalDelay(int delay)
{
	m_pimpl->setDigitalDelay(delay);
}

void M2kHardwareTrigger::setAnalogStreamingFlag(bool val)
{
	m_pimpl->setAnalogStreamingFlag(val);
}

bool M2kHardwareTrigger::getAnalogStreamingFlag()
{
	return m_pimpl->getAnalogStreamingFlag();
}

void M2kHardwareTrigger::setDigitalStreamingFlag(bool val)
{
	m_pimpl->setDigitalStreamingFlag(val);
}

bool M2kHardwareTrigger::getDigitalStreamingFlag()
{
	return m_pimpl->getDigitalStreamingFlag();
}

std::vector<string> M2kHardwareTrigger::getAvailableDigitalConditions()
{
	return M2kHardwareTriggerImpl::getAvailableDigitalConditions();
}

void M2kHardwareTrigger::setCalibParameters(unsigned int chnIdx, double scaling, double vert_offset)
{
	m_pimpl->setCalibParameters(chnIdx, scaling, vert_offset);
}

struct SETTINGS* M2kHardwareTrigger::getCurrentHwSettings()
{
	return m_pimpl->getCurrentHwSettings();
}

void M2kHardwareTrigger::setHwTriggerSettings(struct SETTINGS *settings)
{
	m_pimpl->setHwTriggerSettings(settings);
}
