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

#ifndef M2KHARDWARETRIGGER_IMPL_HPP
#define M2KHARDWARETRIGGER_IMPL_HPP

#include <libm2k/m2khardwaretrigger.hpp>
#include <libm2k/enums.hpp>
#include <libm2k/digital/enums.hpp>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/m2k.hpp>
#include "utils/channel.hpp"
#include "utils/devicein.hpp"
#include <vector>
#include <memory>

using namespace libm2k::utils;
using namespace libm2k::digital;


namespace libm2k {
class M2kHardwareTriggerImpl : public M2kHardwareTrigger
{
public:
	M2kHardwareTriggerImpl(struct iio_context *ctx, bool init = true);

	virtual ~M2kHardwareTriggerImpl();

	void reset();

	int getAnalogLevelRaw(unsigned int chnIdx);
	void setAnalogLevelRaw(unsigned int chnIdx, int level);

	void setAnalogLevel(unsigned int chnIdx, double v_level);
	double getAnalogLevel(unsigned int chnIdx);

	double getAnalogHysteresis(unsigned int chnIdx);
	void setAnalogHysteresis(unsigned int chnIdx, double hysteresis);

	M2K_TRIGGER_CONDITION_ANALOG getAnalogCondition(unsigned int chnIdx);
	void setAnalogCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION_ANALOG cond);

	M2K_TRIGGER_CONDITION_DIGITAL getDigitalCondition(unsigned int chnIdx);
	void setDigitalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION_DIGITAL cond);

	M2K_TRIGGER_MODE getAnalogMode(unsigned int chnIdx);
	void setAnalogMode(unsigned int chnIdx, M2K_TRIGGER_MODE mode);

	libm2k::digital::DIO_TRIGGER_MODE getDigitalMode();
	void setDigitalMode(libm2k::digital::DIO_TRIGGER_MODE mode);

	M2K_TRIGGER_SOURCE_ANALOG getAnalogSource();
	void setAnalogSource(M2K_TRIGGER_SOURCE_ANALOG src);

	int getAnalogSourceChannel();
	void setAnalogSourceChannel(unsigned int chnIdx);

	int getAnalogDelay() const;
	void setAnalogDelay(int delay);

	int getDigitalDelay() const;
	void setDigitalDelay(int delay);

	struct SETTINGS *getCurrentHwSettings();
	void setHwTriggerSettings(struct SETTINGS *settings);

	void setAnalogStreamingFlag(bool enable);
	bool getAnalogStreamingFlag();

	void setDigitalStreamingFlag(bool enable);
	bool getDigitalStreamingFlag();

	static std::vector<std::string> getAvailableDigitalConditions();

	void setCalibParameters(unsigned int chnIdx, double scaling, double vert_offset);

	M2K_TRIGGER_CONDITION_DIGITAL getAnalogExternalCondition(unsigned int chnIdx);
	void setAnalogExternalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION_DIGITAL cond);


	M2K_TRIGGER_CONDITION_DIGITAL getDigitalExternalCondition() const;
	void setDigitalExternalCondition(M2K_TRIGGER_CONDITION_DIGITAL cond);

	bool hasExternalTriggerIn() const;
	bool hasExternalTriggerOut() const;
	bool hasCrossInstrumentTrigger() const;


	M2K_TRIGGER_CONDITION_DIGITAL getDigitalCondition(DIO_CHANNEL chn);
	void setDigitalCondition(DIO_CHANNEL chn, M2K_TRIGGER_CONDITION_DIGITAL cond);

	void setDigitalOutSource(M2K_TRIGGER_OUT_SOURCE src) override;
	M2K_TRIGGER_OUT_SOURCE getDigitalOutSource() const override;
	void setDigitalOutCondition(M2K_TRIGGER_CONDITION_DIGITAL cond) override;
	M2K_TRIGGER_CONDITION_DIGITAL getDigitalOutCondition() const override;
	void setAnalogOutSource(M2K_TRIGGER_OUT_SOURCE src) override;
	M2K_TRIGGER_OUT_SOURCE getAnalogOutSource() const override;
	void setAnalogOutCondition(M2K_TRIGGER_CONDITION_DIGITAL cond) override;
	M2K_TRIGGER_CONDITION_DIGITAL getAnalogOutCondition() const override;

protected:
	struct iio_device *m_trigger_device;
	std::vector<Channel *> m_analog_channels;
	std::vector<Channel *> m_digital_channels;
	std::vector<Channel *> m_logic_channels;
	std::shared_ptr<DeviceIn> m_digital_trigger_device;
	std::shared_ptr<DeviceIn> m_analog_trigger_device;

	Channel *m_delay_trigger;
	bool m_streaming_flag_digital;
	bool m_streaming_flag_analog;

	unsigned int m_num_channels;
	std::vector<bool> m_analog_enabled;
	std::vector<bool> m_digital_enabled;
	std::vector<double> m_scaling, m_offset;

	double m_firmware_version;

	static std::vector<std::string> m_trigger_analog_cond;
	static std::vector<std::string> m_trigger_digital_cond;
	static std::vector<std::string> m_trigger_mode;
	static std::vector<std::string> m_trigger_source;
	static std::vector<std::string> m_trigger_logic_mode;

};
}

#endif //M2KHARDWARETRIGGER_IMPL_HPP
