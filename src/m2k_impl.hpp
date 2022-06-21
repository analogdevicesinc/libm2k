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

	~M2kImpl() override;
	void reset() override;
	void deinitialize() override;

	void scanAllAnalogIn();
	void scanAllAnalogOut();
	void scanAllPowerSupply();
	void scanAllDigital();

	bool calibrate() override;
	bool calibrateADC() override;
	bool calibrateDAC() override;
	bool resetCalibration() override;
	double calibrateFromContext() override;

	libm2k::digital::M2kDigital* getDigital() override;
	libm2k::analog::M2kPowerSupply* getPowerSupply() override;
	libm2k::analog::M2kAnalogIn* getAnalogIn() override;
	libm2k::analog::M2kAnalogIn* getAnalogIn(std::string dev_name) override;
	libm2k::analog::M2kAnalogOut* getAnalogOut() override;
	std::vector<libm2k::analog::M2kAnalogIn*> getAllAnalogIn() override;
	std::vector<libm2k::analog::M2kAnalogOut*> getAllAnalogOut() override;

	bool hasMixedSignal() override;
	void startMixedSignalAcquisition(unsigned int nb_samples) override;
	void stopMixedSignalAcquisition() override;

	int getDacCalibrationOffset(unsigned int chn) override;
	double getDacCalibrationGain(unsigned int chn) override;
	int getAdcCalibrationOffset(unsigned int chn) override;
	double getAdcCalibrationGain(unsigned int chn) override;
	void setDacCalibrationOffset(unsigned int chn, int offset) override;
	void setDacCalibrationGain(unsigned int chn, double gain) override;
	void setAdcCalibrationOffset(unsigned int chn, int offset) override;
	void setAdcCalibrationGain(unsigned int chn, double gain) override;

	bool hasContextCalibration() override;
	std::map<double, std::shared_ptr<struct CALIBRATION_PARAMETERS>> &getLUT() override;
	bool isCalibrated() override;

	void setLed(bool on) override;
	bool getLed() override;
	std::string getFirmwareVersion() override;

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
	std::map<double, shared_ptr<struct CALIBRATION_PARAMETERS>> m_calibration_lut;

	double getCalibrationTemperature(double temperature);
	void blinkLed(const double duration = 4, bool blocking = false);
	void initialize();
};
}
}
#endif // M2K_IMPL_H
