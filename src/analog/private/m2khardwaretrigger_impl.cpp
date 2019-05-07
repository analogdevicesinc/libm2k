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

#include "../../utils/private/device_impl.cpp"
#include <libm2k/analog/m2khardwaretrigger.hpp>
#include <libm2k/utils/channel.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <stdexcept>
#include <algorithm>
#include <iio.h>

using namespace libm2k::analog;
using namespace std;

class M2kHardwareTrigger::M2kHardwareTriggerImpl : public DeviceImpl {
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
	};

	typedef std::pair<Channel *, std::string> channel_pair;

public:
	M2kHardwareTriggerImpl(struct iio_context *ctx) :
		DeviceImpl(ctx, "m2k-adc-trigger")
	{
		std::vector<std::pair<Channel*, std::string>> channels;
		for (unsigned int i = 0; i < getNbChannels(); i++) {
			Channel* ch = getChannel(i);
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

		m_delay_trigger = getChannel("trigger");

		m_num_channels = m_analog_channels.size();

		if (m_analog_channels.size() < 1) {
			throw_exception(EXC_INVALID_PARAMETER,
					"hardware trigger has no analog channels");
		}

		if (m_digital_channels.size() < 1) {
			throw_exception(EXC_INVALID_PARAMETER,
					"hardware trigger has no digital channels");
		}

		if (m_logic_channels.size() < 1) {
			throw_exception(EXC_INVALID_PARAMETER,
					"hardware trigger has no trigger_logic channels");
		}

		if (!m_delay_trigger) {
			throw_exception(EXC_INVALID_PARAMETER, "no delay trigger available");
		}

		for(unsigned int i = 0; i < m_analog_channels.size(); i++) {
			m_scaling.push_back(1);
			m_offset.push_back(0);
		}
		setStreamingFlag(false);
	}

	~M2kHardwareTriggerImpl()
	{
		m_analog_channels.clear();
		m_logic_channels.clear();
		m_digital_channels.clear();
	}

	M2K_TRIGGER_CONDITION getAnalogCondition(unsigned int chnIdx)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel index is out of range");
		}

		std::string buf = m_analog_channels[chnIdx]->getStringValue("trigger");
		auto it = std::find(m_trigger_analog_cond.begin(),
				    m_trigger_analog_cond.end(), buf.c_str());
		if  (it == m_trigger_analog_cond.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "unexpected value read from attribute: trigger");
		}

		return static_cast<M2K_TRIGGER_CONDITION>(it - m_trigger_analog_cond.begin());

	}

	void setAnalogCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION cond)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel index is out of range");
		}

		if (cond == ANY_EDGE) {
			return; //or throw?
		}

		m_analog_channels[chnIdx]->setStringValue("trigger", m_trigger_analog_cond[cond]);
	}

	M2K_TRIGGER_CONDITION getDigitalCondition(unsigned int chnIdx)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel index is out of range");
		}
		std::string buf = m_digital_channels[chnIdx]->getStringValue("trigger");

		auto it = std::find(m_trigger_digital_cond.begin(),
				    m_trigger_digital_cond.end(), buf.c_str());
		if  (it == m_trigger_digital_cond.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "unexpected value read from attribute: trigger");
		}

		return static_cast<M2K_TRIGGER_CONDITION>(it - m_trigger_digital_cond.begin());
	}

	void setDigitalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION cond)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel index is out of range");
		}

		m_digital_channels[chnIdx]->setStringValue("trigger", m_trigger_digital_cond[cond]);
	}

	int getLevelRaw(unsigned int chnIdx)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel index is out of range");
		}

		double val = m_analog_channels[chnIdx]->getDoubleValue("trigger_level");

		return static_cast<int>(val);
	}

	void setLevelRaw(unsigned int chnIdx, int level)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel index is out of range");
		}

		m_analog_channels[chnIdx]->setLongValue("trigger_level", static_cast<long long>(level));
	}

	int getLevel(unsigned int chnIdx)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel index is out of range");
		}

		int raw = getLevelRaw(chnIdx);
		double volts = raw * m_scaling.at(chnIdx) + m_offset.at(chnIdx);
		return volts;
	}

	void setLevel(unsigned int chnIdx, double v_level)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel index is out of range");
		}

		int raw = (v_level - m_offset.at(chnIdx)) / m_scaling.at(chnIdx);
		setLevelRaw(chnIdx, raw);
	}

	int getHysteresis(unsigned int chnIdx)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel index is out of range");
		}

		double val = m_analog_channels[chnIdx]->getDoubleValue("trigger_hysteresis");
		return static_cast<int>(val);
	}

	void setHysteresis(unsigned int chnIdx, int histeresis)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel index is out of range");
		}

		m_analog_channels[chnIdx]->setLongValue("trigger_hysteresis", static_cast<long long>(histeresis));
	}

	M2K_TRIGGER_MODE getTriggerMode(unsigned int chnIdx)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel index is out of range");
		}

		std::string buf = m_logic_channels[chnIdx]->getStringValue("mode");
		auto it = std::find(m_trigger_mode.begin(),
				    m_trigger_mode.end(), buf.c_str());
		if  (it == m_trigger_mode.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "unexpected value read from attribute: mode");
		}

		return static_cast<M2K_TRIGGER_MODE>(it - m_trigger_mode.begin());
	}

	void setTriggerMode(unsigned int chnIdx, M2K_TRIGGER_MODE mode)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel index is out of range");
		}

		m_logic_channels[chnIdx]->setStringValue("mode", m_trigger_mode[mode].c_str());
	}

	M2K_TRIGGER_SOURCE getSource()
	{
		std::string buf = m_delay_trigger->getStringValue("logic_mode");

		auto it = std::find(m_trigger_source.begin(),
				    m_trigger_source.end(), buf.c_str());
		if  (it == m_trigger_source.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "unexpected value read from attribute: logic_mode / source");
		}

		return static_cast<M2K_TRIGGER_SOURCE>(it - m_trigger_source.begin());
	}

	void setSource(M2K_TRIGGER_SOURCE src)
	{
		std::string src_str = m_trigger_source[src];
		m_delay_trigger->setStringValue("logic_mode", src_str);
	}

	/*
 * Convenience function to be used when willing to use the trigger for only one
 * channel at a time.
 */
	int getSourceChannel()
	{
		int chnIdx = -1;

		M2K_TRIGGER_SOURCE src = getSource();

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
	void setSourceChannel(unsigned int chnIdx)
	{
		if (chnIdx >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "Source channel index is out of range");
		}

		// Currently we don't need trigger on multiple channels simultaneously
		// Also options 'a_OR_b', 'a_AND_b' and 'a_XOR_b' don't scale well for
		// 3, 4, .. channels (combinations rise exponentially).
		M2K_TRIGGER_SOURCE src = static_cast<M2K_TRIGGER_SOURCE>(chnIdx);
		setSource(src);
	}

	int getDelay() const
	{
		double delay = m_delay_trigger->getDoubleValue("delay");
		return static_cast<int>(delay);
	}

	void setDelay(int delay)
	{
		m_delay_trigger->setLongValue("delay", delay);
	}

	void setStreamingFlag(bool val)
	{
		m_streaming_flag = val;
		setBoolValue(val, "streaming");
	}

	bool getStreamingFlag()
	{
		return m_streaming_flag;
	}

	static std::vector<string> getAvailableDigitalConditions()
	{
		return {"edge-rising",
			"edge-falling",
			"level-low",
			"level-high",
			"edge-any",
			"none"};
	}

	settings_uptr getCurrentHwSettings()
	{
		settings_uptr settings(new SETTINGS);

		for (unsigned int i = 0; i < getNbChannels(); i++) {
			settings->analog_condition.push_back(getAnalogCondition(i));
			settings->digital_condition.push_back(getDigitalCondition(i));
			settings->level.push_back(getLevel(i));
			settings->hysteresis.push_back(getHysteresis(i));
			settings->mode.push_back(getTriggerMode(i));
			settings->trigger_source = getSource();
			settings->delay = getDelay();
		}

		return settings;
	}

	void setHwTriggerSettings(struct SETTINGS *settings)
	{
		for (unsigned int i = 0; i < getNbChannels(); i++) {
			setAnalogCondition(i, settings->analog_condition[i]);
			setDigitalCondition(i, settings->digital_condition[i]);
			setLevel(i, settings->level[i]);
			setLevelRaw(i, settings->raw_level[i]);
			setHysteresis(i, settings->hysteresis[i]);
			setTriggerMode(i, settings->mode[i]);
			setSource(settings->trigger_source);
			setDelay(settings->delay);
		}
	}

	void setCalibParameters(unsigned int chnIdx, double scaling, double offset)
	{
		m_scaling[chnIdx] = scaling;
		m_offset[chnIdx] = offset;
	}

private:
	struct iio_device *m_trigger_device;
	std::vector<Channel *> m_analog_channels;
	std::vector<Channel *> m_digital_channels;
	std::vector<Channel *> m_logic_channels;
	Channel *m_delay_trigger;
	bool m_streaming_flag;

//	std::vector<std::string> m_trigger_analog_cond;
//	std::vector<std::string> m_trigger_digital_cond;
//	std::vector<std::string> m_trigger_mode;
//	std::vector<std::string> m_trigger_source;

	unsigned int m_num_channels;
	std::vector<bool> m_analog_enabled;
	std::vector<bool> m_digital_enabled;
	std::vector<double> m_scaling, m_offset;
};
