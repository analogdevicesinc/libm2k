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

#include "m2khardwaretrigger_impl.hpp"
#include "utils/devicegeneric.hpp"
#include "utils/devicein.hpp"
#include "utils/channel.hpp"
#include <libm2k/digital/enums.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iio.h>

using namespace libm2k;
using namespace libm2k::utils;
using namespace libm2k::digital;
using namespace std;

std::vector<std::string> M2kHardwareTriggerImpl::m_trigger_analog_cond = {
	"edge-rising",
	"edge-falling",
	"level-low",
	"level-high",
};

std::vector<std::string> M2kHardwareTriggerImpl::m_trigger_digital_cond = {
	"edge-rising",
	"edge-falling",
	"level-low",
	"level-high",
	"edge-any",
	"none"
};

std::vector<std::string> M2kHardwareTriggerImpl::m_trigger_mode = {
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

std::vector<std::string> M2kHardwareTriggerImpl::m_trigger_source = {
	"a",
	"b",
	"a_OR_b",
	"a_AND_b",
	"a_XOR_b",
};

std::vector<std::string>M2kHardwareTriggerImpl:: m_trigger_logic_mode = {
	"or",
	"and",
};

typedef std::pair<Channel *, std::string> channel_pair;

M2kHardwareTriggerImpl::M2kHardwareTriggerImpl(struct iio_context *ctx, bool init) :
	M2kHardwareTrigger()
{
	m_analog_trigger_device = make_shared<DeviceIn>(ctx, "m2k-adc-trigger");
	if (!m_analog_trigger_device) {
		throw_exception(m2k_exception::make("No analog trigger available").type(libm2k::EXC_INVALID_PARAMETER).build());
	}

	std::vector<std::pair<Channel*, std::string>> channels;
	for (unsigned int i = 0; i < m_analog_trigger_device->getNbChannels(false); i++) {
		Channel* ch = m_analog_trigger_device->getChannel(i, false);
		if (ch->isOutput()) {
			continue;
		}

		std::string name = ch->getId();
		std::pair<Channel*, std::string> chn_pair(ch, name);
		channels.push_back(chn_pair);
	}

	std::sort(channels.begin(), channels.end(),
		  [](channel_pair a, channel_pair b)
	{ return a.second < b.second; });

	// Pick the analog, digital, trigger_logic and delay channels
	for (unsigned int i = 0; i < channels.size(); i++) {
		Channel* chn = channels[i].first;
		bool mode = chn->hasAttribute("mode");
		bool trigger = chn->hasAttribute("trigger");
		bool trigger_level = chn->hasAttribute("trigger_level");
		bool trigger_hysteresis = chn->hasAttribute("trigger_hysteresis");

		if (trigger) {
			if (trigger_level && trigger_hysteresis) {
				m_analog_channels.push_back(chn);
				m_analog_enabled.push_back(false);
			} else if (!trigger_level && !trigger_hysteresis) {
				m_digital_channels.push_back(chn);
				m_digital_enabled.push_back(false);
			}
		} else if (mode) {
			m_logic_channels.push_back(chn);
		}
	}

	m_delay_trigger = m_analog_trigger_device->getChannel("trigger", false);

	m_num_channels = m_analog_channels.size();

	if (m_analog_channels.size() < 1) {
		throw_exception(m2k_exception::make("Hardware trigger has no analog channels").type(libm2k::EXC_INVALID_PARAMETER).build());
	}

	if (m_digital_channels.size() < 1) {
		throw_exception(m2k_exception::make("Hardware trigger has no digital channels").type(libm2k::EXC_INVALID_PARAMETER).build());
	}

	if (m_logic_channels.size() < 1) {
		throw_exception(m2k_exception::make("Hardware trigger has no trigger_logic channels").type(libm2k::EXC_INVALID_PARAMETER).build());
	}

	if (!m_delay_trigger) {
		throw_exception(m2k_exception::make("No delay trigger available").type(libm2k::EXC_INVALID_PARAMETER).build());
	}

	if (init) {
		for(unsigned int i = 0; i < m_analog_channels.size(); i++) {
			m_scaling.push_back(1);
			m_offset.push_back(0);
		}
		setAnalogStreamingFlag(false);
	}

	m_digital_trigger_device = make_shared<DeviceIn>(ctx, "m2k-logic-analyzer-rx");
	if (!m_digital_trigger_device) {
		throw_exception(m2k_exception::make("No digital trigger available").type(libm2k::EXC_INVALID_PARAMETER).build());
	}
}

M2kHardwareTriggerImpl::~M2kHardwareTriggerImpl()
{
	m_analog_channels.clear();
	m_logic_channels.clear();
	m_digital_channels.clear();
}

void M2kHardwareTriggerImpl::M2kHardwareTriggerImpl::reset()
{
	setAnalogSource(CHANNEL_1);
	setAnalogDelay(0);
	for (unsigned int i = 0; i < m_analog_channels.size(); i++) {
		setAnalogMode(i, ALWAYS);
		setAnalogLevel(i, 0.0);
		setAnalogHysteresis(i, 0.0);
	}

	for (unsigned int i = 0; i < m_digital_channels.size(); i++) {
		setDigitalCondition(i, NO_TRIGGER_DIGITAL);
	}
	setDigitalDelay(0);
	setDigitalExternalCondition(NO_TRIGGER_DIGITAL);
}

bool M2kHardwareTriggerImpl::hasExternalTriggerOut() const
{
	return m_logic_channels.at(1)->hasAttribute("out_select");
}

bool M2kHardwareTriggerImpl::hasExternalTriggerIn() const
{
	return false;
}

bool M2kHardwareTriggerImpl::hasCrossInstrumentTrigger() const
{
	return m_logic_channels.at(1)->hasAttribute("out_select");
}


M2K_TRIGGER_CONDITION_DIGITAL M2kHardwareTriggerImpl::getDigitalExternalCondition() const
{
	std::string buf = m_digital_trigger_device->getStringValue(16, "trigger");

	auto it = std::find(m_trigger_digital_cond.begin(),
			    m_trigger_digital_cond.end(), buf.c_str());
	if  (it == m_trigger_digital_cond.end()) {
		throw_exception(m2k_exception::make("Unexpected value read from attribute: trigger").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	return static_cast<M2K_TRIGGER_CONDITION_DIGITAL>(it - m_trigger_digital_cond.begin());
}

void M2kHardwareTriggerImpl::setDigitalExternalCondition(M2K_TRIGGER_CONDITION_DIGITAL ext_cond)
{
	m_digital_trigger_device->setStringValue(16, "trigger",
						 m_trigger_digital_cond[ext_cond]);
}

M2K_TRIGGER_CONDITION_DIGITAL M2kHardwareTriggerImpl::getAnalogExternalCondition(unsigned int chnIdx)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}
	std::string buf = m_digital_channels[chnIdx]->getStringValue("trigger");

	auto it = std::find(m_trigger_digital_cond.begin(),
			    m_trigger_digital_cond.end(), buf.c_str());
	if  (it == m_trigger_digital_cond.end()) {
		throw_exception(m2k_exception::make("Unexpected value read from attribute: trigger").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	return static_cast<M2K_TRIGGER_CONDITION_DIGITAL>(it - m_trigger_digital_cond.begin());
}


void M2kHardwareTriggerImpl::setAnalogExternalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION_DIGITAL cond)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	if (cond == NO_TRIGGER_DIGITAL) {
		throw_exception(m2k_exception::make("Analog External condition: can't set NO_TRIGGER for this channel.").type(libm2k::EXC_INVALID_PARAMETER).build());
	}

	m_digital_channels[chnIdx]->setStringValue("trigger", m_trigger_digital_cond[cond]);
}

M2K_TRIGGER_CONDITION_ANALOG M2kHardwareTriggerImpl::getAnalogCondition(unsigned int chnIdx)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	std::string buf = m_analog_channels[chnIdx]->getStringValue("trigger");
	auto it = std::find(m_trigger_analog_cond.begin(),
			    m_trigger_analog_cond.end(), buf.c_str());
	if  (it == m_trigger_analog_cond.end()) {
		throw_exception(m2k_exception::make("Unexpected value read from attribute: trigger").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	return static_cast<M2K_TRIGGER_CONDITION_ANALOG>(it - m_trigger_analog_cond.begin());

}

void M2kHardwareTriggerImpl::setAnalogCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION_ANALOG cond)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	m_analog_channels[chnIdx]->setStringValue("trigger", m_trigger_analog_cond[cond]);
}


M2K_TRIGGER_CONDITION_DIGITAL M2kHardwareTriggerImpl::getDigitalCondition(DIO_CHANNEL chn)
{
	std::string trigger_val = m_digital_trigger_device->getStringValue(chn, "trigger", false);
	std::vector<std::string> available_digital_conditions = getAvailableDigitalConditions();

	auto it = std::find(available_digital_conditions.begin(),
			    available_digital_conditions.end(), trigger_val.c_str());
	if (it == available_digital_conditions.end()) {
		throw_exception(m2k_exception::make("M2kDigital: Cannot read channel attribute: trigger").type(libm2k::EXC_INVALID_PARAMETER).build());
	}

	return static_cast<M2K_TRIGGER_CONDITION_DIGITAL>
			(it - available_digital_conditions.begin());
}

M2K_TRIGGER_CONDITION_DIGITAL M2kHardwareTriggerImpl::getDigitalCondition(unsigned int chn)
{
	DIO_CHANNEL idx = static_cast<DIO_CHANNEL>(chn);
	return getDigitalCondition(idx);
}

void M2kHardwareTriggerImpl::setDigitalCondition(DIO_CHANNEL chn, M2K_TRIGGER_CONDITION_DIGITAL cond)
{
	std::string trigger_val = getAvailableDigitalConditions()[cond];
	m_digital_trigger_device->setStringValue(chn, "trigger", trigger_val, false);
}

void M2kHardwareTriggerImpl::setDigitalCondition(unsigned int chn, M2K_TRIGGER_CONDITION_DIGITAL cond)
{
	DIO_CHANNEL idx = static_cast<DIO_CHANNEL>(chn);
	setDigitalCondition(idx, cond);
}


int M2kHardwareTriggerImpl::getAnalogLevelRaw(unsigned int chnIdx)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	double val = m_analog_channels[chnIdx]->getDoubleValue("trigger_level");

	return static_cast<int>(val);
}

void M2kHardwareTriggerImpl::setAnalogLevelRaw(unsigned int chnIdx, int level)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	m_analog_channels[chnIdx]->setLongValue("trigger_level", static_cast<long long>(level));
}

double M2kHardwareTriggerImpl::getAnalogLevel(unsigned int chnIdx)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	int raw = getAnalogLevelRaw(chnIdx);
	double volts = raw * m_scaling.at(chnIdx) - m_offset.at(chnIdx);
	return volts;
}

void M2kHardwareTriggerImpl::setAnalogLevel(unsigned int chnIdx, double v_level)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	int raw = (v_level + m_offset.at(chnIdx)) / m_scaling.at(chnIdx);
	setAnalogLevelRaw(chnIdx, raw);
}

double M2kHardwareTriggerImpl::getAnalogHysteresis(unsigned int chnIdx)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	double hysteresis_raw = m_analog_channels[chnIdx]->getDoubleValue("trigger_hysteresis");
	return (hysteresis_raw * m_scaling.at(chnIdx));
}

void M2kHardwareTriggerImpl::setAnalogHysteresis(unsigned int chnIdx, double hysteresis)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	int hysteresis_raw = hysteresis / m_scaling.at(chnIdx);
	m_analog_channels[chnIdx]->setLongValue("trigger_hysteresis", static_cast<long long>(hysteresis_raw));
}

M2K_TRIGGER_MODE M2kHardwareTriggerImpl::getAnalogMode(unsigned int chnIdx)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	std::string buf = m_logic_channels[chnIdx]->getStringValue("mode");
	auto it = std::find(m_trigger_mode.begin(),
			    m_trigger_mode.end(), buf.c_str());
	if  (it == m_trigger_mode.end()) {
		throw_exception(m2k_exception::make("Unexpected value read from attribute: mode").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	return static_cast<M2K_TRIGGER_MODE>(it - m_trigger_mode.begin());
}

void M2kHardwareTriggerImpl::setAnalogMode(unsigned int chnIdx, M2K_TRIGGER_MODE mode)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	m_logic_channels[chnIdx]->setStringValue("mode", m_trigger_mode[mode].c_str());
}

void M2kHardwareTriggerImpl::setDigitalMode(DIO_TRIGGER_MODE trig_mode)
{
	std::string trigger_mode = m_trigger_logic_mode[trig_mode];
	m_digital_trigger_device->setStringValue(DIO_CHANNEL_0, "trigger_logic_mode", trigger_mode, false);
}

DIO_TRIGGER_MODE M2kHardwareTriggerImpl::getDigitalMode()
{
	std::string trigger_mode = "";
	trigger_mode = m_digital_trigger_device->getStringValue(DIO_CHANNEL_0,
								"trigger_logic_mode", false);

	auto it = std::find(m_trigger_logic_mode.begin(), m_trigger_logic_mode.end(),
			    trigger_mode.c_str());
	if (it == m_trigger_logic_mode.end()) {
		throw_exception(m2k_exception::make("Cannot read channel attribute: trigger logic mode").type(libm2k::EXC_OUT_OF_RANGE).build());
	}
	return static_cast<DIO_TRIGGER_MODE>(it - m_trigger_logic_mode.begin());
}

M2K_TRIGGER_SOURCE_ANALOG M2kHardwareTriggerImpl::getAnalogSource()
{
	std::string buf = m_delay_trigger->getStringValue("logic_mode");

	auto it = std::find(m_trigger_source.begin(),
			    m_trigger_source.end(), buf.c_str());
	if  (it == m_trigger_source.end()) {
		throw_exception(m2k_exception::make("Unexpected value read from attribute: logic_mode / source").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	return static_cast<M2K_TRIGGER_SOURCE_ANALOG>(it - m_trigger_source.begin());
}

void M2kHardwareTriggerImpl::setAnalogSource(M2K_TRIGGER_SOURCE_ANALOG src)
{
	if (src >= m_trigger_source.size()) {
		throw_exception(m2k_exception::make("M2kHardwareTrigger: "
						    "the provided analog source is not supported on "
						    "the current board; Check the firmware version.").type(libm2k::EXC_INVALID_PARAMETER).build());
	}
	std::string src_str = m_trigger_source[src];
	m_delay_trigger->setStringValue("logic_mode", src_str);
}

/*
 * Convenience function to be used when willing to use the trigger for only one
 * channel at a time.
 */
int M2kHardwareTriggerImpl::getAnalogSourceChannel()
{
	int chnIdx = -1;

	M2K_TRIGGER_SOURCE_ANALOG src = getAnalogSource();

	// Returning the channel index if a single channel is set as a source
	// and -1 if multiple channels are set.

	if (src == CHANNEL_1 || src == CHANNEL_2) {
		chnIdx = src;
	}
	return chnIdx;
}

/*
 * Convenience function to be used when willing to enable the trigger for only
 * one channel at a time.
 */
void M2kHardwareTriggerImpl::setAnalogSourceChannel(unsigned int chnIdx)
{
	if (chnIdx >= m_num_channels) {
		throw_exception(m2k_exception::make("Channel index is out of range").type(libm2k::EXC_OUT_OF_RANGE).build());
	}

	// Currently we don't need trigger on multiple channels simultaneously
	// Also options 'a_OR_b', 'a_AND_b' and 'a_XOR_b' don't scale well for
	// 3, 4, .. channels (combinations rise exponentially).
	M2K_TRIGGER_SOURCE_ANALOG src = static_cast<M2K_TRIGGER_SOURCE_ANALOG>(chnIdx);
	setAnalogSource(src);
}

int M2kHardwareTriggerImpl::getAnalogDelay() const
{
	double delay = m_delay_trigger->getDoubleValue("delay");
	return static_cast<int>(delay);
}

void M2kHardwareTriggerImpl::setAnalogDelay(int delay)
{
	m_delay_trigger->setLongValue("delay", delay);
}

int M2kHardwareTriggerImpl::getDigitalDelay() const
{
	return (int)m_digital_trigger_device->getLongValue(0, "trigger_delay", false);
}

void M2kHardwareTriggerImpl::setDigitalDelay(int delay)
{
	m_digital_trigger_device->setLongValue(0, delay, "trigger_delay", false);
}

void M2kHardwareTriggerImpl::setDigitalStreamingFlag(bool val)
{
	/* Make sure the trigger is reset before enabling the streaming flag. */
	if (val) {
		m_digital_trigger_device->setBoolValue(0, "streaming");
	}
	m_streaming_flag_digital = val;
	m_digital_trigger_device->setBoolValue(val, "streaming");
}

bool M2kHardwareTriggerImpl::getDigitalStreamingFlag()
{
	return m_streaming_flag_digital;
}

void M2kHardwareTriggerImpl::setAnalogStreamingFlag(bool val)
{
	/* Make sure the trigger is reset before enabling the streaming flag. */
	if (val) {
		m_analog_trigger_device->setBoolValue(0, "streaming");
	}
	m_streaming_flag_analog = val;
	m_analog_trigger_device->setBoolValue(val, "streaming");
}

bool M2kHardwareTriggerImpl::getAnalogStreamingFlag()
{
	return m_streaming_flag_analog;
}

std::vector<string> M2kHardwareTriggerImpl::getAvailableDigitalConditions()
{
	return {"edge-rising",
		"edge-falling",
		"level-low",
		"level-high",
		"edge-any",
		"none"};
}

struct SETTINGS* M2kHardwareTriggerImpl::getCurrentHwSettings()
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

void M2kHardwareTriggerImpl::setHwTriggerSettings(struct SETTINGS *settings)
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

void M2kHardwareTriggerImpl::setCalibParameters(unsigned int chnIdx, double scaling, double offset)
{
	m_scaling[chnIdx] = scaling;
	m_offset[chnIdx] = offset;
}
