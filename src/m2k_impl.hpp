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

#ifndef M2K_IMPL_H
#define M2K_IMPL_H

#include <libm2k/m2k.hpp>
#include "context_impl.hpp"
#include <libm2k/enums.hpp>

namespace libm2k {
class M2kHardwareTrigger;
class M2kCalibration;

namespace context {
class M2kImpl : public M2k, public ContextImpl
{
public:
	M2kImpl(std::string uri, iio_context* ctx, std::string name, bool sync);

	virtual ~M2kImpl();
	void reset();
	void deinitialize();

	void scanAllAnalogIn();
	void scanAllAnalogOut();
	void scanAllPowerSupply();
	void scanAllDigital();

	bool calibrate();
	bool calibrateADC();
	bool calibrateDAC();
	bool resetCalibration();

	libm2k::digital::M2kDigital* getDigital();
	libm2k::analog::M2kPowerSupply* getPowerSupply();
	libm2k::analog::M2kAnalogIn* getAnalogIn();
	libm2k::analog::M2kAnalogIn* getAnalogIn(std::string dev_name);
	libm2k::analog::M2kAnalogOut* getAnalogOut();
	std::vector<libm2k::analog::M2kAnalogIn*> getAllAnalogIn();
	std::vector<libm2k::analog::M2kAnalogOut*> getAllAnalogOut();

	void startMixedSignalAcquisition(unsigned int nb_samples) override;
	void stopMixedSignalAcquisition() override;

	int getDacCalibrationOffset(unsigned int chn);
	double getDacCalibrationGain(unsigned int chn);
	int getAdcCalibrationOffset(unsigned int chn);
	double getAdcCalibrationGain(unsigned int chn);
	void setDacCalibrationOffset(unsigned int chn, int offset);
	void setDacCalibrationGain(unsigned int chn, double gain);
	void setAdcCalibrationOffset(unsigned int chn, int offset);
	void setAdcCalibrationGain(unsigned int chn, double gain);


	void setLed(bool on);
	bool getLed();

private:
	M2kCalibration* m_calibration;
	libm2k::M2kHardwareTrigger *m_trigger;
	std::vector<analog::M2kAnalogOut*> m_instancesAnalogOut;
	std::vector<analog::M2kAnalogIn*> m_instancesAnalogIn;
	std::vector<analog::M2kPowerSupply*> m_instancesPowerSupply;
	std::vector<digital::M2kDigital*> m_instancesDigital;
	bool m_sync;
	std::string m_firmware_version;
	enum libm2k::M2K_TRIGGER_SOURCE_ANALOG analogSource;
	enum libm2k::M2K_TRIGGER_SOURCE_DIGITAL digitalSource;
	bool hasAnalogTrigger();
	bool hasDigitalTrigger();
	void blinkLed(const double duration = 4, bool blocking = false);
	void initialize();
};
}
}
#endif // M2K_IMPL_H
