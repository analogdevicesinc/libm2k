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

#include <libm2k/analog/enums.hpp>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/m2k.hpp>
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
	M2kHardwareTrigger(struct iio_context *ctx);

	int getLevel(unsigned int chnIdx) const;
	void setLevel(unsigned int chnIdx, int level);
	void setLevelVolts(unsigned int chnIdx, double v_level);

	int getHysteresis(unsigned int chnIdx) const;
	void setHysteresis(unsigned int chnIdx, int histeresis);

	M2K_TRIGGER_CONDITION getAnalogCondition(unsigned int chnIdx) const;
	void setAnalogCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION cond);

	M2K_TRIGGER_CONDITION getDigitalCondition(unsigned int chnIdx) const;
	void setDigitalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION cond);

	M2K_TRIGGER_MODE getTriggerMode(unsigned int chnIdx) const;
	void setTriggerMode(unsigned int chnIdx, M2K_TRIGGER_MODE mode);

	M2K_TRIGGER_SOURCE getSource() const;
	void setSource(M2K_TRIGGER_SOURCE src);

	unsigned int numChannels() const;

	int getSourceChannel() const;
	void setSourceChannel(unsigned int chnIdx);


	int getDelay() const;
	void setDelay(int delay);

	settings_uptr getCurrentHwSettings();
	void setHwTriggerSettings(struct SETTINGS *settings);

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
