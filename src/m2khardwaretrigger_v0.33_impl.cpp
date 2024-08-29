/*
 * Copyright (c) 2024 Analog Devices Inc.
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
#include "m2khardwaretrigger_v0.33_impl.hpp"
#include <stdexcept>
#include <algorithm>
#include <iio.h>

using namespace libm2k;
using namespace libm2k::utils;
using namespace libm2k::digital;
using namespace std;

std::vector<std::string> M2kHardwareTriggerV033Impl::m_trigger_source_out = {
	"none",
	"trigger-Ti",
	"trigger-adc",
	"trigger-la",
};

std::vector<std::string> M2kHardwareTriggerV033Impl::m_trigger_condition_out = {
	"none",
	"level-low",
	"level-high",
	"edge-any",
	"edge-rising",
	"edge-falling",
};

std::vector<std::string> M2kHardwareTriggerV033Impl::m_trigger_status_out = {
	"disabled",
	"start",
	"stop",
};

M2kHardwareTriggerV033Impl::M2kHardwareTriggerV033Impl(struct iio_context *ctx, bool init) : M2kHardwareTriggerV024Impl(ctx, init)
{
	// Analog out trigger is shared between dac-a and dac-b
	m_analog_out_trigger_device = make_shared<DeviceIn>(ctx, "m2k-dac-a");
}

M2kHardwareTriggerV033Impl::~M2kHardwareTriggerV033Impl()
{
}

void M2kHardwareTriggerV033Impl::reset()
{
	M2kHardwareTriggerV024Impl::reset();
	setAnalogOutTriggerSource(TRIGGER_NONE);
	setAnalogOutTriggerCondition(NONE_OUT);
	setAnalogOutTriggerStatus(DISABLED);
}

void libm2k::M2kHardwareTriggerV033Impl::deinitialize()
{
	M2kHardwareTriggerV024Impl::deinitialize();
	setAnalogOutTriggerStatus(DISABLED);
}

void M2kHardwareTriggerV033Impl::setAnalogOutTriggerSource(M2K_TRIGGER_SOURCE_OUT src)
{
	if (src < 0 || src >= m_trigger_source_out.size())
	{
		THROW_M2K_EXCEPTION("M2kHardwareTrigger: invalid trigger source", libm2k::EXC_OUT_OF_RANGE);
	}
	m_analog_out_trigger_device->setStringValue("trigger_src", m_trigger_source_out[src]);
}

M2K_TRIGGER_SOURCE_OUT M2kHardwareTriggerV033Impl::getAnalogOutTriggerSource() const
{
	std::string buf = m_analog_out_trigger_device->getStringValue("trigger_src");

	auto it = std::find(m_trigger_source_out.begin(),
						m_trigger_source_out.end(), buf.c_str());
	if (it == m_trigger_source_out.end())
	{
		THROW_M2K_EXCEPTION("Unexpected value read from attribute: trigger_src", libm2k::EXC_OUT_OF_RANGE);
	}

	return static_cast<M2K_TRIGGER_SOURCE_OUT>(it - m_trigger_source_out.begin());
}

void M2kHardwareTriggerV033Impl::setAnalogOutTriggerCondition(M2K_TRIGGER_CONDITION_OUT condition)
{
	if (condition < 0 || condition >= m_trigger_condition_out.size())
	{
		THROW_M2K_EXCEPTION("M2kHardwareTrigger: invalid trigger condition", libm2k::EXC_OUT_OF_RANGE);
	}
	m_analog_out_trigger_device->setStringValue("trigger_condition", m_trigger_condition_out[condition]);
}

M2K_TRIGGER_CONDITION_OUT M2kHardwareTriggerV033Impl::getAnalogOutTriggerCondition() const
{
	std::string buf = m_analog_out_trigger_device->getStringValue("trigger_condition");

	auto it = std::find(m_trigger_condition_out.begin(),
						m_trigger_condition_out.end(), buf.c_str());
	if (it == m_trigger_condition_out.end())
	{
		THROW_M2K_EXCEPTION("Unexpected value read from attribute: trigger_src", libm2k::EXC_OUT_OF_RANGE);
	}

	return static_cast<M2K_TRIGGER_CONDITION_OUT>(it - m_trigger_condition_out.begin());
}

void M2kHardwareTriggerV033Impl::setAnalogOutTriggerStatus(M2K_TRIGGER_STATUS_ANALOG_OUT status)
{
	if (status < 0 || status >= m_trigger_status_out.size())
	{
		THROW_M2K_EXCEPTION("M2kHardwareTrigger: invalid trigger status", libm2k::EXC_OUT_OF_RANGE);
	}
	auto chn = m_analog_out_trigger_device->getChannel(0, true);
	chn->setStringValue("trigger_status", m_trigger_status_out[status]);
}

M2K_TRIGGER_STATUS_ANALOG_OUT M2kHardwareTriggerV033Impl::getAnalogOutTriggerStatus() const
{
	auto chn = m_analog_out_trigger_device->getChannel(0, true);
	auto status = chn->getStringValue("trigger_status");

	auto it = std::find(m_trigger_status_out.begin(),
						m_trigger_status_out.end(), status.c_str());
	if (it == m_trigger_status_out.end())
	{
		THROW_M2K_EXCEPTION("Unexpected value read from attribute: trigger_status", libm2k::EXC_OUT_OF_RANGE);
	}
	return static_cast<M2K_TRIGGER_STATUS_ANALOG_OUT>(it - m_trigger_status_out.begin());
}