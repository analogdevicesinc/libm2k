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

#ifndef M2KHARDWARETRIGGER_HPP
#define M2KHARDWARETRIGGER_HPP

#include "libm2k/m2kglobal.hpp"
#include "m2k.hpp"
#include <vector>
#include <memory>

extern "C" {
	struct iio_context;
	struct iio_device;
	struct iio_channel;
}

namespace libm2k {
namespace analog {
class M2kAnalogIn;

class LIBM2K_API M2kHardwareTrigger
{
public:
	enum condition {
		RISING_EDGE = 0,
		FALLING_EDGE = 1,
		LOW = 3,
		HIGH = 4,
		ANY_EDGE = 5,
		NONE = 6,
	};

	enum mode {
		ALWAYS = 0,
		ANALOG = 1,
		DIGITAL = 2,
		DIGITAL_OR_ANALOG = 3,
		DIGITAL_AND_ANALOG = 4,
		DIGITAL_XOR_ANALOG = 5,
		N_DIGITAL_OR_ANALOG = 6,
		N_DIGITAL_AND_ANALOG = 7,
		N_DIGITAL_XOR_ANALOG = 8,
	};

	enum source {
		CHANNEL_1 = 0,
		CHANNEL_2 = 1,
		CHANNEL_1_OR_CHANNEL_2 = 2,
		CHANNEL_1_AND_CHANNEL_2 = 3,
		CHANNEL_1_XOR_CHANNEL_2 = 4,
	};

	struct Settings {
		std::vector<condition> analog_condition;
		std::vector<condition> digital_condition;
		std::vector<int> level;
		std::vector<int> hysteresis;
		std::vector<enum mode> mode;
		source trigger_source;
		int delay;
	};

	typedef std::unique_ptr<M2kHardwareTrigger::Settings> settings_uptr;

public:

	M2kHardwareTrigger(struct iio_context *ctx);

	int getLevel(unsigned int chnIdx) const;
	void setLevel(unsigned int chnIdx, int level);
	void setLevelVolts(unsigned int chnIdx, double v_level);

	int getHysteresis(unsigned int chnIdx) const;
	void setHysteresis(unsigned int chnIdx, int histeresis);

	condition getAnalogCondition(unsigned int chnIdx) const;
	void setAnalogCondition(unsigned int chnIdx, condition cond);

	condition getDigitalCondition(unsigned int chnIdx) const;
	void setDigitalCondition(unsigned int chnIdx, condition cond);

	mode getTriggerMode(unsigned int chnIdx) const;
	void setTriggerMode(unsigned int chnIdx, mode mode);

	source getSource() const;
	void setSource(source src);

	unsigned int numChannels() const;

	int getSourceChannel() const;
	void setSourceChannel(unsigned int chnIdx);


	int getDelay() const;
	void setDelay(int delay);

	std::unique_ptr<struct Settings> getCurrentHwSettings();
	void setHwTriggerSettings(struct Settings *settings);

	void setStreamingFlag(bool);
	bool getStreamingFlag();

	static std::vector<std::string> getAvailableDigitalConditions();

private:
	struct iio_context *m_ctx;
	struct iio_device *m_trigger_device;
	std::vector<struct iio_channel *> m_analog_channels;
	std::vector<struct iio_channel *> m_digital_channels;
	std::vector<struct iio_channel *> m_logic_channels;
	struct iio_channel *m_delay_trigger;
	bool m_streaming_flag;

	unsigned int m_num_channels;

	static std::vector<std::string> m_trigger_analog_cond;
	static std::vector<std::string> m_trigger_digital_cond;
	static std::vector<std::string> m_trigger_mode;
	static std::vector<std::string> m_trigger_source;
	std::vector<bool> m_analog_enabled;
	std::vector<bool> m_digital_enabled;
};
}
}

#endif //M2KHARDWARETRIGGER_HPP
