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
#include <stdexcept>
#include <algorithm>
#include <iio.h>

using namespace libm2k;
using namespace libm2k::utils;
using namespace libm2k::digital;
using namespace std;

std::vector<std::string> M2kHardwareTriggerV024Impl::m_digital_out_direction = {
	"in",
	"out"
};

std::vector<std::string> M2kHardwareTriggerV024Impl::m_digital_out_select= {
	"sw-trigger",
	"trigger-i-same-chan",
	"trigger-i-swap-chan",
	"trigger-adc",
	"trigger-in",
};

std::vector<std::string> M2kHardwareTriggerV024Impl::m_trigger_source = {
	"a",
	"b",
	"a_OR_b",
	"a_AND_b",
	"a_XOR_b",
	"trigger_in",
	"a_OR_trigger_in",
	"b_OR_trigger_in",
	"a_OR_b_OR_trigger_in",
};

std::vector<std::string> M2kHardwareTriggerV024Impl::m_trigger_ext_digital_source = {
	"trigger-logic",
	"trigger-in"
};

typedef std::pair<Channel *, std::string> channel_pair;

M2kHardwareTriggerV024Impl::M2kHardwareTriggerV024Impl(struct iio_context *ctx, bool init) :
	M2kHardwareTriggerImpl(ctx, "m2k-adc-trigger")
{
}

M2kHardwareTriggerV024Impl::~M2kHardwareTriggerV024Impl()
{
}

void M2kHardwareTriggerV024Impl::reset()
{
	M2kHardwareTriggerImpl::reset();
	setAnalogExternalOutSelect(SELECT_NONE);
	setDigitalSource(SRC_NONE);
}

void M2kHardwareTriggerV024Impl::setAnalogExternalOutSelect(M2K_TRIGGER_OUT_SELECT out_select)
{
	unsigned int TRIGGER_OUT_PIN = 1;
	if (hasExternalTriggerOut()) {
		m_logic_channels.at(TRIGGER_OUT_PIN)->setStringValue("out_direction",
								     m_digital_out_direction.at(1));
		m_logic_channels.at(TRIGGER_OUT_PIN)->setStringValue("out_select",
								     m_digital_out_select.at(out_select));
	}
}

void M2kHardwareTrigger::setAnalogExternalOutSelect(M2K_TRIGGER_OUT_SELECT output_select)
{
	throw std::runtime_error("M2kHardwareTrigger: "
				 "the analog external output is not configurable on "
				 "the current board; Check the firmware version.");
}


M2K_TRIGGER_OUT_SELECT M2kHardwareTriggerV024Impl::getAnalogExternalOutSelect()
{
	unsigned int TRIGGER_OUT_PIN = 1;
	if (hasExternalTriggerOut()) {
		std::string buf = m_logic_channels.at(TRIGGER_OUT_PIN)->getStringValue("out_select");

		auto it = std::find(m_digital_out_select.begin(),
				    m_digital_out_select.end(), buf.c_str());
		if  (it == m_digital_out_select.end()) {
			throw_exception(m2k_exception::make("Unexpected value read from attribute: out_select").type(libm2k::EXC_OUT_OF_RANGE).build());
		}

		return static_cast<M2K_TRIGGER_OUT_SELECT>(it - m_digital_out_select.begin());
	}
	return SELECT_NONE;
}

M2K_TRIGGER_OUT_SELECT M2kHardwareTrigger::getAnalogExternalOutSelect()
{
	throw std::runtime_error("M2kHardwareTrigger: "
				 "the analog external output is not available on "
				 "the current board; Check the firmware version.");
}

bool M2kHardwareTriggerV024Impl::hasExternalTriggerOut() const
{
	return m_logic_channels.at(1)->hasAttribute("out_select");
}

bool M2kHardwareTriggerV024Impl::hasExternalTriggerIn() const
{
	return true;
}

bool M2kHardwareTriggerV024Impl::hasCrossInstrumentTrigger() const
{
	return m_logic_channels.at(1)->hasAttribute("out_select");
}

void M2kHardwareTriggerV024Impl::setDigitalSource(M2K_TRIGGER_SOURCE_DIGITAL external_src)
{
	if (!hasCrossInstrumentTrigger()) {
		M2kHardwareTriggerImpl::setDigitalSource(external_src);
	}

	if (external_src == SRC_NONE) {
		external_src = SRC_TRIGGER_IN;
	}

	m_digital_trigger_device->setStringValue(16, "trigger_mux_out",
						 m_trigger_ext_digital_source.at(external_src));
}

void M2kHardwareTrigger::setDigitalSource(M2K_TRIGGER_SOURCE_DIGITAL external_src)
{
	throw std::runtime_error("M2kHardwareTrigger: "
				 "the digital external source is not configurable on "
				 "the current board; Check the firmware version.");
}

M2K_TRIGGER_SOURCE_DIGITAL M2kHardwareTriggerV024Impl::getDigitalSource() const
{
	if (!hasCrossInstrumentTrigger()) {
		M2kHardwareTriggerImpl::getDigitalSource();
	}

	std::string buf = m_digital_trigger_device->getStringValue(16, "trigger_mux_out");
	auto it = std::find(m_trigger_ext_digital_source.begin(),
			    m_trigger_ext_digital_source.end(), buf.c_str());
	if  (it == m_trigger_ext_digital_source.end()) {
		throw_exception(m2k_exception::make("Unexpected value read from attribute: trigger").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	auto src = static_cast<M2K_TRIGGER_SOURCE_DIGITAL>(it - m_trigger_ext_digital_source.begin());
	if (src == SRC_ANALOG_IN) {
		return src;
	}

	if (getDigitalExternalCondition() != NO_TRIGGER_DIGITAL) {
		return SRC_TRIGGER_IN;
	}
	return SRC_NONE;
}

M2K_TRIGGER_SOURCE_DIGITAL M2kHardwareTrigger::getDigitalSource() const
{
	throw std::runtime_error("M2kHardwareTrigger: "
				 "the digital external source is not available on "
				 "the current board; Check the firmware version.");
}

M2K_TRIGGER_SOURCE_ANALOG M2kHardwareTriggerV024Impl::getAnalogSource()
{
	std::string buf = m_delay_trigger->getStringValue("logic_mode");

	auto it = std::find(m_trigger_source.begin(),
			    m_trigger_source.end(), buf.c_str());
	if  (it == m_trigger_source.end()) {
		throw_exception(m2k_exception::make("Unexpected value read from attribute: logic_mode / source").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	return static_cast<M2K_TRIGGER_SOURCE_ANALOG>(it - m_trigger_source.begin());
}

void M2kHardwareTriggerV024Impl::setAnalogSource(M2K_TRIGGER_SOURCE_ANALOG src)
{
	std::string src_str = m_trigger_source[src];
	m_delay_trigger->setStringValue("logic_mode", src_str);
}

struct SETTINGS* M2kHardwareTriggerV024Impl::getCurrentHwSettings()
{
	SETTINGS* settings = new SETTINGS;

	for (unsigned int i = 0; i < m_num_channels; i++) {
		settings->analog_condition.push_back(getAnalogCondition(i));
		settings->digital_condition.push_back(getDigitalExternalCondition());
		settings->level.push_back(getAnalogLevel(i));
		settings->hysteresis.push_back(getAnalogHysteresis(i));
		settings->mode.push_back(getAnalogMode(i));
		settings->trigger_source = getAnalogSource();
		settings->delay = getAnalogDelay();
	}

	return settings;
}

void M2kHardwareTriggerV024Impl::setHwTriggerSettings(struct SETTINGS *settings)
{
	for (unsigned int i = 0; i < m_num_channels; i++) {
		setAnalogCondition(i, settings->analog_condition[i]);
		setDigitalExternalCondition(settings->digital_condition[i]);
		setAnalogLevel(i, settings->level[i]);
		setAnalogLevelRaw(i, settings->raw_level[i]);
		setAnalogHysteresis(i, settings->hysteresis[i]);
		setAnalogMode(i, settings->mode[i]);
		setAnalogSource(settings->trigger_source);
		setAnalogDelay(settings->delay);
	}
}
