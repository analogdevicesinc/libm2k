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

#include "private/m2khardwaretrigger_v0.24_impl.cpp"

using namespace libm2k;
using namespace std;

typedef std::pair<Channel *, std::string> channel_pair;

M2kHardwareTrigger::M2kHardwareTrigger(struct iio_context *ctx, const std::string& fw_version)
{
	int diff = Utils::compareVersions(fw_version, "v0.24");
	if (diff < 0) {	//fw_version < 0.24
		m_pimpl = std::unique_ptr<M2kHardwareTriggerImpl>(new M2kHardwareTriggerImpl(ctx));
	} else {
		m_pimpl = std::unique_ptr<M2kHardwareTriggerV024Impl>(new M2kHardwareTriggerV024Impl(ctx));
	}
}

M2kHardwareTrigger::~M2kHardwareTrigger()
{
}

void M2kHardwareTrigger::init()
{
	m_pimpl->init();
}

int M2kHardwareTrigger::getWindowCntValue()
{
	return m_pimpl->getWindowCntValue();
}

void M2kHardwareTrigger::setCntFunction(M2K_CNT_FUNCTION val)
{
	m_pimpl->setCntFunction(val);
}

int M2kHardwareTrigger::getCntFunction()
{
	return m_pimpl->getCntFunction();
}

void M2kHardwareTrigger::setL1Function(M2K_TRIGGER_SOURCE_ANALOG val)
{
	m_pimpl->setL1Function(val);
}

int M2kHardwareTrigger::getL1Function()
{
	return m_pimpl->getL1Function();
}

void M2kHardwareTrigger::setL2Function(M2K_TRIGGER_SOURCE_ANALOG val)
{
	m_pimpl->setL2Function(val);
}

int M2kHardwareTrigger::getL2Function()
{
	return m_pimpl->getL2Function();
}

int M2kHardwareTrigger::getL1AnalogLevelRaw()
{
	return m_pimpl->getL1AnalogLevelRaw();
}

void M2kHardwareTrigger::setL1AnalogLevelRaw(int level)
{
	m_pimpl->setL1AnalogLevelRaw(level);
}

double M2kHardwareTrigger::getL1AnalogLevel()
{
	return m_pimpl->getL1AnalogLevel();
}

void M2kHardwareTrigger::setL1AnalogLevel(double v_level)
{
	m_pimpl->setL1AnalogLevel(v_level);
}

double M2kHardwareTrigger::getL1AnalogHysteresis()
{
	return m_pimpl->getL1AnalogHysteresis();
}

void M2kHardwareTrigger::setL1AnalogHysteresis(double hysteresis)
{
	m_pimpl->setL1AnalogHysteresis(hysteresis);
}

int M2kHardwareTrigger::getL1Source()
{
	return m_pimpl->getL1Source();
}

void M2kHardwareTrigger::setL1Source(M2K_WINDOW_L_SOURCE val)
{
	m_pimpl->setL1Source(val);
}

int M2kHardwareTrigger::getL2AnalogLevelRaw()
{
	return m_pimpl->getL2AnalogLevelRaw();
}

void M2kHardwareTrigger::setL2AnalogLevelRaw(int level)
{
	m_pimpl->setL2AnalogLevelRaw(level);
}

double M2kHardwareTrigger::getL2AnalogLevel()
{
	return m_pimpl->getL2AnalogLevel();
}

void M2kHardwareTrigger::setL2AnalogLevel(double v_level)
{
	m_pimpl->setL2AnalogLevel(v_level);
}

double M2kHardwareTrigger::getL2AnalogHysteresis()
{
	return m_pimpl->getL2AnalogHysteresis();
}

void M2kHardwareTrigger::setL2AnalogHysteresis(double hysteresis)
{
	m_pimpl->setL2AnalogHysteresis(hysteresis);
}

int M2kHardwareTrigger::getL2Source()
{
	return m_pimpl->getL2Source();
}

void M2kHardwareTrigger::setL2Source(M2K_WINDOW_L_SOURCE val)
{
	m_pimpl->setL2Source(val);
}

int M2kHardwareTrigger::getWindowStartCnt()
{
	return m_pimpl->getWindowStartCnt();
}

void M2kHardwareTrigger::setWindowStartCnt(int val)
{
	m_pimpl->setWindowStartCnt(val);
}

int M2kHardwareTrigger::getWindowStopCnt()
{
	return m_pimpl->getWindowStopCnt();
}

void M2kHardwareTrigger::setWindowStopCnt(int val)
{
	m_pimpl->setWindowStopCnt(val);
}

int M2kHardwareTrigger::getWindowCntLimit1()
{
	return m_pimpl->getWindowCntLimit1();
}

void M2kHardwareTrigger::setWindowCntLimit1(int val)
{
	m_pimpl->setWindowCntLimit1(val);
}

int M2kHardwareTrigger::getWindowCntLimit2()
{
	return m_pimpl->getWindowCntLimit2();
}

void M2kHardwareTrigger::setWindowCntLimit2(int val)
{
	m_pimpl->setWindowCntLimit2(val);
}

void M2kHardwareTrigger::setResetCntAtNewStart(bool val)
{
	m_pimpl->setResetCntAtNewStart(val);
}

bool M2kHardwareTrigger::getResetCntAtNewStart()
{
	return m_pimpl->getResetCntAtNewStart();
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

double M2kHardwareTrigger::getAnalogHysteresis(unsigned int chnIdx)
{
	return m_pimpl->getAnalogHysteresis(chnIdx);
}

void M2kHardwareTrigger::setAnalogHysteresis(unsigned int chnIdx, double hysteresis)
{
	m_pimpl->setAnalogHysteresis(chnIdx, hysteresis);
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
