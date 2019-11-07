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

#ifndef M2KHARDWARETRIGGER_IMPL_v024_HPP
#define M2KHARDWARETRIGGER_IMPL_v024_HPP

#include <libm2k/utils/devicegeneric.hpp>
#include <libm2k/utils/devicein.hpp>
#include <libm2k/utils/channel.hpp>
#include <libm2k/digital/enums.hpp>
#include <libm2k/m2kexceptions.hpp>
#include "m2khardwaretrigger_impl.cpp"
#include <stdexcept>
#include <algorithm>
#include <iio.h>

using namespace libm2k::utils;
using namespace libm2k::analog;
using namespace libm2k::digital;
using namespace std;

class M2kHardwareTrigger::M2kHardwareTriggerV024Impl : public M2kHardwareTriggerImpl {


	std::vector<std::string> m_digital_out_direction = {
		"in",
		"out"
	};

	std::vector<std::string> m_digital_out_select= {
		"sw-trigger",
		"trigger-i-same-chan",
		"trigger-i-swap-chan",
		"trigger-adc",
		"trigger-in",
	};

	std::vector<std::string> m_trigger_analog_cond = {
		"edge-rising",
		"edge-falling",
		"level-low",
		"level-high",
	};

	std::vector<std::string> m_trigger_digital_cond = {
		"edge-rising",
		"edge-falling",
		"level-low",
		"level-high",
		"edge-any",
		"none"
	};

	std::vector<std::string> m_trigger_mode = {
		"always",
		"analog",
		"digital",
		"digital_OR_analog",
		"digital_AND_analog",
		"digital_XOR_analog",
		"!digital_OR_analog",
		"!digital_AND_analog",
		"!digital_XOR_analog",
	};

	std::vector<std::string> m_trigger_source = {
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

	std::vector<std::string> m_trigger_ext_digital_source = {
		"trigger-logic",
		"trigger-in"
	};

	std::vector<std::string> m_trigger_logic_mode = {
		"or",
		"and",
	};

	typedef std::pair<Channel *, std::string> channel_pair;

public:
	M2kHardwareTriggerV024Impl(struct iio_context *ctx, bool init = true) :
		M2kHardwareTriggerImpl(ctx, "m2k-adc-trigger")
	{
	}

	~M2kHardwareTriggerV024Impl()
	{
	}

	bool hasExternalTriggerOut()
	{
		return m_logic_channels.at(1)->hasAttribute("out_select");
	}

	bool hasExternalTriggerIn()
	{
		return true;
	}

	bool hasCrossIntrumentTrigger()
	{
		return m_logic_channels.at(1)->hasAttribute("out_select");
	}

	void setAnalogExternalOutSelect(M2K_TRIGGER_OUT_SELECT out_select)
	{
		unsigned int TRIGGER_OUT_PIN = 1;
		if (hasExternalTriggerOut()) {
			m_logic_channels.at(TRIGGER_OUT_PIN)->setStringValue("out_direction",
							       m_digital_out_direction.at(1));
			m_logic_channels.at(TRIGGER_OUT_PIN)->setStringValue("out_select",
								    m_digital_out_select.at(out_select));
		}
	}

	M2K_TRIGGER_OUT_SELECT getAnalogExternalOutSelect()
	{
		unsigned int TRIGGER_OUT_PIN = 1;
		if (hasExternalTriggerOut()) {
			std::string buf = m_logic_channels.at(TRIGGER_OUT_PIN)->getStringValue("out_select");

			auto it = std::find(m_digital_out_select.begin(),
					    m_digital_out_select.end(), buf.c_str());
			if  (it == m_digital_out_select.end()) {
				throw_exception(EXC_OUT_OF_RANGE, "unexpected value read from attribute: out_select");
			}

			return static_cast<M2K_TRIGGER_OUT_SELECT>(it - m_digital_out_select.begin());
		}
	}

	void setDigitalSource(M2K_TRIGGER_SOURCE_DIGITAL external_src)
	{
		if (!hasCrossIntrumentTrigger()) {
			M2kHardwareTriggerImpl::setDigitalSource(external_src);
		}

		if (external_src == SRC_NONE) {
			external_src = SRC_TRIGGER_IN;
		}

		m_digital_trigger_device->setStringValue(16, "trigger_mux_out",
							 m_trigger_ext_digital_source.at(external_src));
	}

	M2K_TRIGGER_SOURCE_DIGITAL getDigitalSource()
	{
		if (!hasCrossIntrumentTrigger()) {
			M2kHardwareTriggerImpl::getDigitalSource();
		}

		std::string buf = m_digital_trigger_device->getStringValue(16, "trigger_mux_out");
		auto it = std::find(m_trigger_ext_digital_source.begin(),
				    m_trigger_ext_digital_source.end(), buf.c_str());
		if  (it == m_trigger_ext_digital_source.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "unexpected value read from attribute: trigger");
		}

		return static_cast<M2K_TRIGGER_SOURCE_DIGITAL>(it - m_trigger_ext_digital_source.begin());
	}

	M2K_TRIGGER_SOURCE_ANALOG getAnalogSource()
	{
		std::string buf = m_delay_trigger->getStringValue("logic_mode");

		auto it = std::find(m_trigger_source.begin(),
				    m_trigger_source.end(), buf.c_str());
		if  (it == m_trigger_source.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "unexpected value read from attribute: logic_mode / source");
		}

		return static_cast<M2K_TRIGGER_SOURCE_ANALOG>(it - m_trigger_source.begin());
	}

	void setAnalogSource(M2K_TRIGGER_SOURCE_ANALOG src)
	{
		std::string src_str = m_trigger_source[src];
		m_delay_trigger->setStringValue("logic_mode", src_str);
	}

	struct SETTINGS* getCurrentHwSettings()
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

	void setHwTriggerSettings(struct SETTINGS *settings)
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
};

#endif
