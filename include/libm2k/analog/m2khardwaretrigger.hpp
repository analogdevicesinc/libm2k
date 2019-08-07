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
#include <libm2k/digital/enums.hpp>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/m2k.hpp>
#include <vector>
#include <memory>

namespace libm2k {
namespace analog {
class M2kAnalogIn;
class LIBM2K_API M2kHardwareTrigger
{
public:
	M2kHardwareTrigger(struct iio_context *ctx);
	virtual ~M2kHardwareTrigger();

	int getAnalogLevelRaw(unsigned int chnIdx);
	void setAnalogLevelRaw(unsigned int chnIdx, int level);
	void setAnalogLevel(unsigned int chnIdx, double v_level);
	double getAnalogLevel(unsigned int chnIdx);

	int getAnalogHysteresis(unsigned int chnIdx);
	void setAnalogHysteresis(unsigned int chnIdx, int histeresis);

	M2K_TRIGGER_CONDITION getAnalogCondition(unsigned int chnIdx);
	void setAnalogCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION cond);

	M2K_TRIGGER_CONDITION getExternalCondition(unsigned int chnIdx);
	void setExternalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION cond);

	M2K_TRIGGER_CONDITION getDigitalCondition(unsigned int chnIdx);
	void setDigitalCondition(unsigned int chnIdx, M2K_TRIGGER_CONDITION cond);

	M2K_TRIGGER_MODE getAnalogMode(unsigned int chnIdx);
	void setAnalogMode(unsigned int chnIdx, M2K_TRIGGER_MODE mode);

	libm2k::digital::DIO_TRIGGER_MODE getDigitalMode();
	void setDigitalMode(libm2k::digital::DIO_TRIGGER_MODE mode);

	M2K_TRIGGER_SOURCE getAnalogSource();
	void setAnalogSource(M2K_TRIGGER_SOURCE src);

	int getAnalogSourceChannel();
	void setAnalogSourceChannel(unsigned int chnIdx);

	int getAnalogDelay() const;
	void setAnalogDelay(int delay);

	int getDigitalDelay() const;
	void setDigitalDelay(int delay);

	settings_uptr getCurrentHwSettings();
	void setHwTriggerSettings(struct SETTINGS *settings);

	void setAnalogStreamingFlag(bool);
	bool getAnalogStreamingFlag();

	void setDigitalStreamingFlag(bool);
	bool getDigitalStreamingFlag();

	static std::vector<std::string> getAvailableDigitalConditions();

	void setCalibParameters(unsigned int chnIdx, double scaling, double offset);
private:
	class M2kHardwareTriggerImpl;
	std::unique_ptr<M2kHardwareTriggerImpl> m_pimpl;
};
}
}

#endif //M2KHARDWARETRIGGER_HPP
