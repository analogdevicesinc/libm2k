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

#ifndef M2KCALIBRATION_IMPL_HPP
#define M2KCALIBRATION_IMPL_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/m2kcalibration.hpp>
#include <libm2k/m2khardwaretrigger.hpp>
#include "analog/m2kanalogin_impl.hpp"
#include <cstdint>
#include <cstdlib>
#include <string>
#include <memory>
#include <vector>

namespace libm2k {

class M2kCalibrationImpl : public M2kCalibration
{
public:
	M2kCalibrationImpl(struct iio_context* ctx, libm2k::analog::M2kAnalogIn* analogIn,
		       libm2k::analog::M2kAnalogOut* analogOut);
	virtual ~M2kCalibrationImpl();

	bool initialize();
	bool isInitialized() const;

	void setAdcInCalibMode();
	void setDacInCalibMode();
	void restoreAdcFromCalibMode();
	void restoreDacFromCalibMode();

	bool calibrateAll();
	bool calibrateADC();
	bool calibrateADCoffset();
	bool calibrateADCgain();
	bool calibrateDAC();
	bool calibrateDACoffset();
	bool calibrateDACgain();
	void cancelCalibration();

	int adcOffsetChannel0() const;
	int adcOffsetChannel1() const;
	int dacAoffset() const;
	int dacBoffset() const;
	double adcGainChannel0() const;
	double adcGainChannel1() const;
	double dacAvlsb() const;
	double dacBvlsb() const;

	bool resetCalibration();
	void updateAdcCorrections();
	void updateDacCorrections();

	bool setCalibrationMode(int);

private:
	bool m_cancel;

	struct iio_context *m_ctx;
	libm2k::analog::M2kAnalogInImpl* m_m2k_adc;
	libm2k::analog::M2kAnalogOut* m_m2k_dac;
	M2kHardwareTrigger* m_m2k_trigger;

	int m_adc_ch0_offset;
	int m_adc_ch1_offset;
	int m_dac_a_ch_offset;
	int m_dac_b_ch_offset;
	double m_adc_ch0_gain;
	double m_adc_ch1_gain;
	double m_dac_a_ch_vlsb;
	double m_dac_b_ch_vlsb;

	M2K_TRIGGER_MODE m_trigger0_mode;
	M2K_TRIGGER_MODE m_trigger1_mode;
	M2K_TRIGGER_SOURCE_ANALOG m_trigger_src;
	double adc_sampl_freq;
	double adc_oversampl;
	double dac_a_sampl_freq;
	double dac_a_oversampl;
	double dac_b_sampl_freq;
	double dac_b_oversampl;

	bool m_adc_calibrated;
	bool m_dac_calibrated;
	bool m_initialized;
	int m_calibration_mode;

	std::vector<bool> m_adc_channels_enabled;
	std::vector<bool> m_dac_channels_enabled;
	double m_dac_default_vlsb;

	std::shared_ptr<libm2k::utils::DeviceGeneric> m_ad5625_dev;
	std::shared_ptr<libm2k::utils::DeviceGeneric> m_m2k_fabric;
	void configAdcSamplerate();
	void configDacSamplerate();
	bool fine_tune(size_t span, int16_t centerVal0, int16_t centerVal1, size_t num_samples);
	int16_t processRawSample(int16_t value);
};

}

#endif /* M2KCALIBRATION_IMPL_HPP */
