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

#include "libm2k/m2khardwaretrigger.hpp"
#include "libm2k/m2kexceptions.hpp"
#include <stdexcept>
#include <algorithm>
#include <iio.h>

using namespace libm2k::analog;
using namespace std;

std::vector<std::string> M2kHardwareTrigger::m_trigger_analog_cond = {
	"edge-rising",
	"edge-falling",
	"level-low",
	"level-high",
};

std::vector<std::string> M2kHardwareTrigger::m_trigger_digital_cond = {
	"edge-rising",
	"edge-falling",
	"level-low",
	"level-high",
	"edge-any",
};

std::vector<std::string> M2kHardwareTrigger::m_trigger_mode = {
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

std::vector<std::string> M2kHardwareTrigger::m_trigger_source = {
	"a",
	"b",
	"a_OR_b",
	"a_AND_b",
	"a_XOR_b",
};

typedef std::pair<struct iio_channel *, std::string> channel_pair;

M2kHardwareTrigger::M2kHardwareTrigger(struct iio_context *ctx) :
	m_ctx(ctx)
{
	m_trigger_device = iio_context_find_device(m_ctx, "m2k-adc-trigger");

	if (!m_trigger_device) {
		throw no_device_exception("trigger_device = NULL");
	}

	// Get all channels and sort them ascending by name
	std::vector<std::pair<struct iio_channel *, std::string>> channels;
	for (unsigned int i = 0; i < iio_device_get_channels_count(m_trigger_device); i++) {
		struct iio_channel *chn = iio_device_get_channel(m_trigger_device, i);

		if (iio_channel_is_output(chn)) {
			continue;
		}

		std::string name = std::string(iio_channel_get_id(chn));
		std::pair<struct iio_channel *, std::string> chn_pair(chn, name);
		channels.push_back(chn_pair);
	}
	std::sort(channels.begin(), channels.end(),
		[](channel_pair a, channel_pair b)
			{ return a.second < b.second; });

	// Pick the analog, digital, trigger_logic and delay channels
	for (int i = 0; i < channels.size(); i++) {
		struct iio_channel *chn = channels[i].first;
		bool mode = iio_channel_find_attr(chn, "mode");
		bool trigger = iio_channel_find_attr(chn, "trigger");
		bool trigger_level = iio_channel_find_attr(chn,
				"trigger_level");
		bool trigger_hysteresis = iio_channel_find_attr(chn,
				"trigger_hysteresis");

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

	m_delay_trigger = iio_device_find_channel(m_trigger_device, "trigger",
			false);

	m_num_channels = m_analog_channels.size();

	if (m_analog_channels.size() < 1) {
		throw invalid_parameter_exception(
			"hardware trigger has no analog channels");
	}

	if (m_digital_channels.size() < 1) {
		throw invalid_parameter_exception(
			"hardware trigger has no digital channels");
	}

	if (m_logic_channels.size() < 1) {
		throw invalid_parameter_exception(
			"hardware trigger has no trigger_logic channels");
	}

	if (!m_delay_trigger) {
		throw invalid_parameter_exception("no delay trigger available");
	}
	setStreamingFlag(false);
}

unsigned int M2kHardwareTrigger::numChannels() const
{
	return m_num_channels;
}

M2kHardwareTrigger::condition M2kHardwareTrigger::getAnalogCondition(unsigned int chnIdx)
		const
{
	if (chnIdx >= numChannels()) {
		throw std::invalid_argument("Channel index is out of range");
	}

	ssize_t ret;
	char buf[4096];

	ret = iio_channel_attr_read(m_analog_channels[chnIdx], "trigger", buf,
		sizeof(buf));
	if (ret < 0) {
		throw std::runtime_error("failed to read attribute: trigger");
	}
	auto it = std::find(m_trigger_analog_cond.begin(),
		m_trigger_analog_cond.end(), buf);
	if  (it == m_trigger_analog_cond.end()) {
		throw std::runtime_error(
			"unexpected value read from attribute: trigger");
	}

	return static_cast<condition>(it - m_trigger_analog_cond.begin());

}

void M2kHardwareTrigger::setAnalogCondition(unsigned int chnIdx, condition cond)
{
	if (chnIdx >= numChannels()) {
		throw std::invalid_argument("Channel index is out of range");
	}

	if (cond == ANY_EDGE) {
		return; //or throw?
	}

	iio_channel_attr_write(m_analog_channels[chnIdx], "trigger",
		m_trigger_analog_cond[cond].c_str());
}

M2kHardwareTrigger::condition M2kHardwareTrigger::getDigitalCondition(unsigned int chnIdx)
		const
{
	if (chnIdx >= numChannels()) {
		throw std::invalid_argument("Channel index is out of range");
	}

	ssize_t ret;
	char buf[4096];

	ret = iio_channel_attr_read(m_digital_channels[chnIdx], "trigger", buf,
		sizeof(buf));
	if (ret < 0)
		throw "failed to read attribute: trigger";
	auto it = std::find(m_trigger_digital_cond.begin(),
		m_trigger_digital_cond.end(), buf);
	if  (it == m_trigger_digital_cond.end()) {
		throw "unexpected value read from attribute: trigger";
	}

	return static_cast<condition>(it - m_trigger_digital_cond.begin());
}

void M2kHardwareTrigger::setDigitalCondition(unsigned int chnIdx, condition cond)
{
	if (chnIdx >= numChannels()) {
		throw std::invalid_argument("Channel index is out of range");
	}

	iio_channel_attr_write(m_digital_channels[chnIdx], "trigger",
		m_trigger_digital_cond[cond].c_str());
}

int M2kHardwareTrigger::getLevel(unsigned int chnIdx) const
{
	if (chnIdx >= numChannels()) {
		throw std::invalid_argument("Channel index is out of range");
	}

	long long val;

	iio_channel_attr_read_longlong(m_analog_channels[chnIdx],
		"trigger_level", &val);

	return static_cast<int>(val);
}

void M2kHardwareTrigger::setLevel(unsigned int chnIdx, int level)
{
	if (chnIdx >= numChannels()) {
		throw std::invalid_argument("Channel index is out of range");
	}

	iio_channel_attr_write_longlong(m_analog_channels[chnIdx],
		"trigger_level", static_cast<long long> (level));
}

int M2kHardwareTrigger::getHysteresis(unsigned int chnIdx) const
{
	if (chnIdx >= numChannels()) {
		throw std::invalid_argument("Channel index is out of range");
	}

	long long val;

	iio_channel_attr_read_longlong(m_analog_channels[chnIdx],
		"trigger_hysteresis", &val);

	return static_cast<int>(val);
}

void M2kHardwareTrigger::setHysteresis(unsigned int chnIdx, int histeresis)
{
	if (chnIdx >= numChannels()) {
		throw std::invalid_argument("Channel index is out of range");
	}

	iio_channel_attr_write_longlong(m_analog_channels[chnIdx],
		"trigger_hysteresis", static_cast<long long>(histeresis));
}

M2kHardwareTrigger::mode M2kHardwareTrigger::getTriggerMode(unsigned int chnIdx) const
{
	if (chnIdx >= numChannels()) {
		throw std::invalid_argument("Channel index is out of range");
	}

	ssize_t ret;
	char buf[4096];

	ret = iio_channel_attr_read(m_logic_channels[chnIdx], "mode", buf,
		sizeof(buf));
	if (ret < 0) {
		throw ("failed to read attribute: mode");
	}
	auto it = std::find(m_trigger_mode.begin(),
		m_trigger_mode.end(), buf);
	if  (it == m_trigger_mode.end()) {
		throw ("unexpected value read from attribute: mode");
	}

	return static_cast<mode>(it - m_trigger_mode.begin());
}

void M2kHardwareTrigger::setTriggerMode(unsigned int chnIdx, M2kHardwareTrigger::mode mode)
{
	if (chnIdx >= numChannels()) {
		throw std::invalid_argument("Channel index is out of range");
	}

	iio_channel_attr_write(m_logic_channels[chnIdx], "mode",
		m_trigger_mode[mode].c_str());
}

M2kHardwareTrigger::source M2kHardwareTrigger::getSource() const
{
	char buf[4096];

	int ret = iio_channel_attr_read(m_delay_trigger, "logic_mode", buf, sizeof(buf));

	if (ret < 0)
		throw "failed to read attribute: logic_mode";
	auto it = std::find(m_trigger_source.begin(),
		m_trigger_source.end(), buf);
	if  (it == m_trigger_source.end()) {
		throw "unexpected value read from attribute: logic_mode / source";
	}

	return static_cast<source>(it - m_trigger_source.begin());
}

void M2kHardwareTrigger::setSource(source src)
{
	std::string src_str = m_trigger_source[src];
	iio_channel_attr_write(m_delay_trigger, "logic_mode",
			       src_str.c_str());
}

/*
 * Convenience function to be used when willing to use the trigger for only one
 * channel at a time.
 */
int M2kHardwareTrigger::getSourceChannel() const
{
	int chnIdx = -1;

	source src = getSource();

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
void M2kHardwareTrigger::setSourceChannel(unsigned int chnIdx)
{
	if (chnIdx >= numChannels()) {
		throw std::invalid_argument("Channel index is out of range");
	}

	// Currently we don't need trigger on multiple channels simultaneously
	// Also options 'a_OR_b', 'a_AND_b' and 'a_XOR_b' don't scale well for
	// 3, 4, .. channels (combinations rise exponentially).
	source src = static_cast<source>(chnIdx);
	setSource(src);
}

int M2kHardwareTrigger::getDelay() const
{
	long long delay;

	iio_channel_attr_read_longlong(m_delay_trigger, "delay", &delay);

	return static_cast<int>(delay);
}

void M2kHardwareTrigger::setDelay(int delay)
{
	iio_channel_attr_write_longlong(m_delay_trigger, "delay", delay);
}

void M2kHardwareTrigger::setStreamingFlag(bool val)
{
	m_streaming_flag = val;
	iio_device_attr_write_bool(m_trigger_device, "streaming", val);
}

bool M2kHardwareTrigger::getStreamingFlag()
{
	return m_streaming_flag;
}

M2kHardwareTrigger::settings_uptr M2kHardwareTrigger::getCurrentHwSettings()
{
	settings_uptr settings(new Settings);

	for (unsigned int i = 0; i < numChannels(); i++) {
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

void M2kHardwareTrigger::setHwTriggerSettings(struct Settings *settings)
{
	for (unsigned int i = 0; i < numChannels(); i++) {
		setAnalogCondition(i, settings->analog_condition[i]);
		setDigitalCondition(i, settings->digital_condition[i]);
		setLevel(i, settings->level[i]);
		setHysteresis(i, settings->hysteresis[i]);
		setTriggerMode(i, settings->mode[i]);
		setSource(settings->trigger_source);
		setDelay(settings->delay);
	}
}
