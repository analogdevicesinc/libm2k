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

#include "utils/devicegeneric.hpp"
#include "utils/devicein.hpp"
#include "utils/channel.hpp"
#include <libm2k/digital/enums.hpp>
#include <libm2k/m2kexceptions.hpp>
#include "m2khardwaretrigger_v0.24_impl.hpp"
#include "m2khardwaretrigger_v0.32_impl.hpp"
#include <stdexcept>
#include <algorithm>
#include <iio.h>

using namespace libm2k;
using namespace libm2k::utils;
using namespace libm2k::digital;
using namespace std;


std::vector<std::string> M2kHardwareTriggerV032Impl::m_trigger_source_out = {
	"none",
	"trigger-i_0",
	"trigger-i_1",
	"trigger-adc",
	"trigger-la",
};

std::vector<std::string> M2kHardwareTriggerV032Impl::m_trigger_condition_out = {
	"none",
	"level-low",
	"level-high",
	"edge-any",
	"edge-rising",
	"edge-falling",
};

std::vector<std::string> M2kHardwareTriggerV032Impl::m_trigger_event_out = {
	"disabled",
	"start",
	"stop",
};

M2kHardwareTriggerV032Impl::M2kHardwareTriggerV032Impl(struct iio_context *ctx, bool init) :
	M2kHardwareTriggerV024Impl(ctx, init)
{
	UNUSED(init);
	m_analog_out_trigger_device = make_shared<DeviceIn>(ctx, "m2k-dac-a");
	m_digital_out_trigger_device= make_shared<DeviceIn>(ctx, "m2k-logic-analyzer-tx");
}

M2kHardwareTriggerV032Impl::~M2kHardwareTriggerV032Impl()
{
}

void M2kHardwareTriggerV032Impl::reset()
{
	M2kHardwareTriggerV024Impl::reset();
	// TODO: add extra reset functionality IF NEEDED!!
	setAnalogOutTriggerSource(TRIGGER_NONE);
	setAnalogOutTriggerCondition(NONE_OUT);
	setAnalogOutTriggeredAction(DISABLED);
	setDigitalOutTriggerSource(TRIGGER_NONE);
	setDigitalOutTriggerCondition(NONE_OUT);
}

void M2kHardwareTriggerV032Impl::setAnalogOutTriggerSource(M2K_TRIGGER_SOURCE_OUT src) 
{
	m_analog_out_trigger_device->setStringValue("trigger_src", m_trigger_source_out[src]);
}

M2K_TRIGGER_SOURCE_OUT M2kHardwareTriggerV032Impl::getAnalogOutTriggerSource() const 
{
	std::string buf = m_analog_out_trigger_device->getStringValue("trigger_src");
	
	auto it = std::find(m_trigger_source_out.begin(),
			    m_trigger_source_out.end(), buf.c_str());
	if  (it == m_trigger_source_out.end()) {
		THROW_M2K_EXCEPTION("Unexpected value read from attribute: trigger_src", libm2k::EXC_OUT_OF_RANGE);
	}	

	return static_cast<M2K_TRIGGER_SOURCE_OUT>(it - m_trigger_source_out.begin());
}

void M2kHardwareTriggerV032Impl::setAnalogOutTriggerCondition(M2K_TRIGGER_CONDITION_OUT condition) 
{
	m_analog_out_trigger_device->setStringValue("trigger_condition", m_trigger_condition_out[condition]);
}

M2K_TRIGGER_CONDITION_OUT M2kHardwareTriggerV032Impl::getAnalogOutTriggerCondition() const
{
	std::string buf = m_analog_out_trigger_device->getStringValue("trigger_condition");

	auto it = std::find(m_trigger_condition_out.begin(),
			    m_trigger_condition_out.end(), buf.c_str());
	if  (it == m_trigger_condition_out.end()) {
		THROW_M2K_EXCEPTION("Unexpected value read from attribute: trigger_src", libm2k::EXC_OUT_OF_RANGE);
	}	

	return static_cast<M2K_TRIGGER_CONDITION_OUT>(it - m_trigger_condition_out.begin());
}

void M2kHardwareTriggerV032Impl::setAnalogOutTriggeredAction(M2K_TRIGGER_ACTION_ANALOG_OUT event)
{
	switch (event)
	{
	case START:
		setAnalogOutTriggeredStart(true);
		setAnalogOutTriggeredStop(false);
		break;
	case STOP:
		setAnalogOutTriggeredStart(false);
		setAnalogOutTriggeredStop(true);	
		break;
	default:
		setAnalogOutTriggeredStart(false);
		setAnalogOutTriggeredStop(false);
	}
}

M2K_TRIGGER_ACTION_ANALOG_OUT M2kHardwareTriggerV032Impl::getAnalogOutTriggeredAction()
{
	auto chn = m_analog_out_trigger_device->getChannel(0, true);
	auto start = chn->getStringValue("start_trigger");
	auto stop = chn->getStringValue("stop_trigger");

	if (start == "enabled" && stop == "enabled") {
		THROW_M2K_EXCEPTION("M2kHardwareTrigger: having both trigger actions set simultaneously causes conflict.",
							libm2k::EXC_RUNTIME_ERROR);
	} else if (start == "enabled" && stop == "disabled") {
		return START;
	} else if (start == "disabled" && stop == "enabled") {
		return STOP;
	}
	return DISABLED;
}

bool M2kHardwareTriggerV032Impl::setAnalogOutTriggeredStart(bool en)
{
	auto chn = m_analog_out_trigger_device->getChannel(0, true);
	chn->setStringValue("start_trigger", en ? "enabled" : "disabled");
	return chn->getStringValue("start_trigger") == "enabled";
}

bool M2kHardwareTriggerV032Impl::setAnalogOutTriggeredStop(bool en)
{
	auto chn = m_analog_out_trigger_device->getChannel(0, true);
	chn->setStringValue("stop_trigger", en ? "enabled" : "disabled");
	return chn->getStringValue("stop_trigger") == "enabled";
}

void M2kHardwareTriggerV032Impl::setDigitalOutTriggerSource(M2K_TRIGGER_SOURCE_OUT src)
{
	m_digital_out_trigger_device->setStringValue("trigger_src", m_trigger_source_out[src]);
}

M2K_TRIGGER_SOURCE_OUT M2kHardwareTriggerV032Impl::getDigitalOutTriggerSource() const
{
	std::string buf = m_digital_out_trigger_device->getStringValue("trigger_src");
	
	auto it = std::find(m_trigger_source_out.begin(),
			    m_trigger_source_out.end(), buf.c_str());
	if  (it == m_trigger_source_out.end()) {
		THROW_M2K_EXCEPTION("Unexpected value read from attribute: trigger_src", libm2k::EXC_OUT_OF_RANGE);
	}	

	return static_cast<M2K_TRIGGER_SOURCE_OUT>(it - m_trigger_source_out.begin());
}

void M2kHardwareTriggerV032Impl::setDigitalOutTriggerCondition(M2K_TRIGGER_CONDITION_OUT condition)
{
	m_digital_out_trigger_device->setStringValue("trigger_condition", m_trigger_condition_out[condition]);
}

M2K_TRIGGER_CONDITION_OUT M2kHardwareTriggerV032Impl::getDigitalOutTriggerCondition() const
{
	std::string buf = m_digital_out_trigger_device->getStringValue("trigger_condition");

	auto it = std::find(m_trigger_condition_out.begin(),
			    m_trigger_condition_out.end(), buf.c_str());
	if  (it == m_trigger_condition_out.end()) {
		THROW_M2K_EXCEPTION("Unexpected value read from attribute: trigger_src", libm2k::EXC_OUT_OF_RANGE);
	}	

	return static_cast<M2K_TRIGGER_CONDITION_OUT>(it - m_trigger_condition_out.begin());
}