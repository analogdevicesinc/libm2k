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

using namespace libm2k;
using namespace libm2k::utils;
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
		"window_trigger",
	};

	std::vector<std::string> m_trigger_ext_digital_source = {
		"trigger-logic",
		"trigger-in"
	};

	std::vector<std::string> m_trigger_logic_mode = {
		"or",
		"and",
	};

	std::vector<std::string> m_trigger_logic_window_source = {
		"channel_0",
		"channel_1",
	};

	std::vector<std::string> m_trigger_logic_window_mode = {
		"wider-than",
		"shorter-than",
		"in-range",
		"out-of-range",
	};

	typedef std::pair<Channel *, std::string> channel_pair;

public:
	M2kHardwareTriggerV024Impl(struct iio_context *ctx, bool init = true) :
		M2kHardwareTriggerImpl(ctx, "m2k-adc-trigger")
	{
		std::vector<std::pair<Channel*, std::string>> channels;
		for (unsigned int i = 0; i < getNbChannels(false); i++) {
			Channel* ch = getChannel(i, false);
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

		m_window_trigger_channel = nullptr;
		for (unsigned int i = 0; i < channels.size(); i++) {
			Channel* chn = channels[i].first;
			bool attr = chn->hasAttribute("cnt_function");
			if (attr) {
				m_window_trigger_channel = chn;
			}
		}
	}

	~M2kHardwareTriggerV024Impl()
	{
	}

	void init()
	{
		M2kHardwareTriggerImpl::init();
		setAnalogExternalOutSelect(SELECT_NONE);
		setDigitalSource(SRC_NONE);
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
		return SELECT_NONE;
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

	int getWindowCntValue()
	{
		double val = m_window_trigger_channel->getDoubleValue("cnt_value");
		return static_cast<int>(val);
	}

	virtual int getCntFunction()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getCntFunction();
		}

		std::string buf = m_window_trigger_channel->getStringValue("cnt_function");

		auto it = std::find(m_trigger_logic_window_mode.begin(),
				    m_trigger_logic_window_mode.end(), buf.c_str());
		if  (it == m_trigger_logic_window_mode.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "unexpected value read from attribute: logic_mode / source");
		}

		M2K_CNT_FUNCTION val = static_cast<M2K_CNT_FUNCTION>(it - m_trigger_logic_window_mode.begin());
		int ret;
		// make a corelation between enum and index
		for (unsigned int i = 0; i < sizeof(M2K_CNT_FUNCTION); i++ ) {
			if (M2K_CNT_FUNCTION(i) == val) {
				ret = i;
			}
		}
		return ret;
	}

	void setCntFunction(M2K_CNT_FUNCTION val)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setCntFunction(val);
		}

		auto cnt_function = m_trigger_logic_window_mode[val];
		m_window_trigger_channel->setStringValue("cnt_function", cnt_function);
	}

	virtual int getL1Function()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getL1Function();
		}

		std::string buf = m_window_trigger_channel->getStringValue("l1_function");

		auto it = std::find(m_trigger_analog_cond.begin(),
				    m_trigger_analog_cond.end(), buf.c_str());
		if  (it == m_trigger_analog_cond.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "unexpected value read from attribute: logic_mode / source");
		}

		M2K_TRIGGER_CONDITION_ANALOG val = static_cast<M2K_TRIGGER_CONDITION_ANALOG>(it - m_trigger_analog_cond.begin());
		int ret;
		// make a corelation between enum and index
		for (unsigned int i = 0; i < sizeof(M2K_TRIGGER_CONDITION_ANALOG); i++ ) {
			if (M2K_TRIGGER_CONDITION_ANALOG(i) == val) {
				ret = i;
			}
		}
		return ret;
	}

	void setL1Function(M2K_TRIGGER_SOURCE_ANALOG val)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setL1Function(val);
		}

		auto l1_function = m_trigger_analog_cond[val];
		m_window_trigger_channel->setStringValue("l1_function", l1_function);
	}

	virtual int getL2Function()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getL2Function();
		}

		std::string buf = m_window_trigger_channel->getStringValue("l2_function");

		auto it = std::find(m_trigger_analog_cond.begin(),
				    m_trigger_analog_cond.end(), buf.c_str());
		if  (it == m_trigger_analog_cond.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "unexpected value read from attribute: logic_mode / source");
		}

		M2K_TRIGGER_CONDITION_ANALOG val = static_cast<M2K_TRIGGER_CONDITION_ANALOG>(it - m_trigger_analog_cond.begin());
		int ret;
		// make a corelation between enum and index
		for (unsigned int i = 0; i < sizeof(M2K_TRIGGER_CONDITION_ANALOG); i++ ) {
			if (M2K_TRIGGER_CONDITION_ANALOG(i) == val) {
				ret = i;
			}
		}
		return ret;
	}

	void setL2Function(M2K_TRIGGER_SOURCE_ANALOG val)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setL2Function(val);
		}

		auto l2_function = m_trigger_analog_cond[val];
		m_window_trigger_channel->setStringValue("l2_function", l2_function);
	}

	int getL1AnalogLevelRaw()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getL1AnalogLevelRaw();
		}

		double val = m_window_trigger_channel->getDoubleValue("l1_limit");
		return static_cast<int>(val);
	}

	void setL1AnalogLevelRaw(int level)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setL1AnalogLevelRaw(level);
		}

		m_window_trigger_channel->setLongValue("l1_limit", static_cast<long long>(level));
	}

	double getL1AnalogLevel()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getL1AnalogLevel();
		}

		int raw = getL1AnalogLevelRaw();
		// fix-me offset for second channel
		double volts = raw * m_scaling.at(0) + m_offset.at(0);
		return volts;
	}

	void setL1AnalogLevel(double v_level)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setL1AnalogLevel(v_level);
		}

		int raw = (v_level - m_offset.at(0)) / m_scaling.at(0);
		setL1AnalogLevelRaw(raw);
	}

	double getL1AnalogHysteresis()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getL1AnalogHysteresis();
		}

		double hysteresis_raw = m_window_trigger_channel->getDoubleValue("l1_hysteresis");
		return (hysteresis_raw * m_scaling.at(0));
	}

	void setL1AnalogHysteresis(double hysteresis)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setL1AnalogHysteresis(hysteresis);
		}

		int hysteresis_raw = hysteresis / m_scaling.at(0);
		m_window_trigger_channel->setLongValue("l1_hysteresis", static_cast<long long>(hysteresis_raw));
	}

	virtual int getL1Source()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getL1Source();
		}

		std::string buf = m_window_trigger_channel->getStringValue("l1_source");

		auto it = std::find(m_trigger_logic_window_source.begin(),
				    m_trigger_logic_window_source.end(), buf.c_str());
		if  (it == m_trigger_logic_window_source.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "unexpected value read from attribute: logic_mode / source");
		}

		M2K_WINDOW_L_SOURCE val = static_cast<M2K_WINDOW_L_SOURCE>(it - m_trigger_logic_window_source.begin());
		int ret;
		// make a corelation between enum and index
		for (unsigned int i = 0; i < sizeof(M2K_WINDOW_L_SOURCE); i++ ) {
			if (M2K_WINDOW_L_SOURCE(i) == val) {
				ret = i;
			}
		}
		return ret;
	}

	void setL1Source(M2K_WINDOW_L_SOURCE val)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setL1Source(val);
		}

		auto l1_source = m_trigger_logic_window_source[val];
		m_window_trigger_channel->setStringValue("l1_source", l1_source);
	}

	int getL2AnalogLevelRaw()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getL2AnalogLevelRaw();
		}

		double val = m_window_trigger_channel->getDoubleValue("l2_limit");
		return static_cast<int>(val);
	}

	void setL2AnalogLevelRaw(int level)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setL2AnalogLevelRaw(level);
		}

		m_window_trigger_channel->setLongValue("l2_limit", static_cast<long long>(level));
	}

	double getL2AnalogLevel()
	{
		int raw = getL2AnalogLevelRaw();
		double volts = raw * m_scaling.at(1) + m_offset.at(1);
		return volts;
	}

	void setL2AnalogLevel(double v_level)
	{
		int raw = (v_level - m_offset.at(1)) / m_scaling.at(1);
		setL2AnalogLevelRaw(raw);
	}

	double getL2AnalogHysteresis()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getL2AnalogHysteresis();
		}

		double hysteresis_raw = m_window_trigger_channel->getDoubleValue("l2_hysteresis");
		return (hysteresis_raw * m_scaling.at(1));
	}

	void setL2AnalogHysteresis(double hysteresis)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setL2AnalogHysteresis(hysteresis);
		}

		int hysteresis_raw = hysteresis / m_scaling.at(1);
		m_window_trigger_channel->setLongValue("l2_hysteresis", static_cast<long long>(hysteresis_raw));
	}

	virtual int getL2Source()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getL2Source();
		}

		std::string buf = m_window_trigger_channel->getStringValue("l2_source");

		auto it = std::find(m_trigger_logic_window_source.begin(),
				    m_trigger_logic_window_source.end(), buf.c_str());
		if  (it == m_trigger_logic_window_source.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "unexpected value read from attribute: logic_mode / source");
		}

		M2K_WINDOW_L_SOURCE val = static_cast<M2K_WINDOW_L_SOURCE>(it - m_trigger_logic_window_source.begin());
		int ret;
		// make a corelation between enum and index
		for (unsigned int i = 0; i < sizeof(M2K_WINDOW_L_SOURCE); i++ ) {
			if (M2K_WINDOW_L_SOURCE(i) == val) {
				ret = i;
			}
		}
		return ret;
	}

	void setL2Source(M2K_WINDOW_L_SOURCE val)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setL2Source(val);
		}

		auto l2_source = m_trigger_logic_window_source[val];
		m_window_trigger_channel->setStringValue("l2_source", l2_source);
	}

	int getWindowStartCnt()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getWindowStartCnt();
		}

		double val = m_window_trigger_channel->getDoubleValue("start_cnt");
		return static_cast<int>(val);
	}

	void setWindowStartCnt(int val)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setWindowStartCnt(val);
		}

		m_window_trigger_channel->setLongValue("start_cnt", static_cast<long long>(val));
	}

	int getWindowStopCnt()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getWindowStartCnt();
		}

		double val = m_window_trigger_channel->getDoubleValue("stop_cnt");
		return static_cast<int>(val);
	}

	void setWindowStopCnt(int val)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setWindowStartCnt(val);
		}

		m_window_trigger_channel->setLongValue("stop_cnt", static_cast<long long>(val));
	}

	int getWindowCntLimit1()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getWindowCntLimit1();
		}

		double val = m_window_trigger_channel->getDoubleValue("limit_1");
		return static_cast<int>(val);
	}

	void setWindowCntLimit1(int val)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setWindowCntLimit1(val);
		}

		m_window_trigger_channel->setLongValue("limit_1", static_cast<long long>(val));
	}

	int getWindowCntLimit2()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getWindowCntLimit2();
		}

		double val = m_window_trigger_channel->getDoubleValue("limit_2");
		return static_cast<int>(val);
	}

	void setWindowCntLimit2(int val)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setWindowCntLimit2(val);
		}

		m_window_trigger_channel->setLongValue("limit_2", static_cast<long long>(val));
	}

	bool getResetCntAtNewStart()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getResetCntAtNewStart();
		}

		bool rst_val = m_window_trigger_channel->getBoolValue("rst_at_new_start");
		return rst_val;
	}

	void setResetCntAtNewStart(bool val)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setResetCntAtNewStart(val);
		}

		m_window_trigger_channel->setBoolValue("rst_at_new_start", val);
	}

	bool getWindowSoftReset()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getWindowSoftReset();
		}

		bool rst_val = m_window_trigger_channel->getBoolValue("one_shot");
		return rst_val;
	}

	void setWindowSoftReset(bool val)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setWindowSoftReset(val);
		}

		m_window_trigger_channel->setBoolValue("one_shot", val);
	}

	bool getOneShotWindow()
	{
		if (!m_window_trigger_channel) {
			return M2kHardwareTriggerImpl::getOneShotWindow();
		}

		bool rst_val = m_window_trigger_channel->getBoolValue("one_shot");
		return rst_val;
	}

	void setOneShotWindow(bool val)
	{
		if (!m_window_trigger_channel) {
			M2kHardwareTriggerImpl::setOneShotWindow(val);
		}

		m_window_trigger_channel->setBoolValue("one_shot", val);
	}


private:
	Channel *m_window_trigger_channel;

};

#endif
