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

#include "m2kcalibration_impl.hpp"
#include "analog/m2kanalogin_impl.hpp"
#include "utils/devicegeneric.hpp"
#include <libm2k/analog/m2kanalogout.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>
#include <libm2k/enums.hpp>

#include <errno.h>
#include <iio.h>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>

using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::utils;

M2kCalibrationImpl::M2kCalibrationImpl(struct iio_context* ctx, M2kAnalogIn* analogIn,
				       M2kAnalogOut* analogOut):
	m_cancel(false),
	m_ctx(ctx),
	m_m2k_adc(dynamic_cast<M2kAnalogInImpl*>(analogIn)),
	m_m2k_dac(dynamic_cast<M2kAnalogOutImpl*>(analogOut)),
	m_m2k_trigger(analogIn->getTrigger()),
	m_adc_calibrated(false),
	m_dac_calibrated(false),
	m_initialized(false),
	m_ad5625_dev(nullptr)
{
	m_ad5625_dev = make_shared<DeviceGeneric>(m_ctx, "ad5625");
	m_adc_channels_enabled.push_back(false);
	m_adc_channels_enabled.push_back(false);
	m_dac_channels_enabled.push_back(false);
	m_dac_channels_enabled.push_back(false);
	m_dac_default_vlsb = 10.0 / ((double)( 1 << 12 ));
}

M2kCalibrationImpl::~M2kCalibrationImpl()
{
}

bool M2kCalibrationImpl::initialize()
{
	m_initialized = false;

	if (!m_ctx)
		return false;

	m_m2k_fabric = std::make_shared<DeviceGeneric>(m_ctx, "m2k-fabric");
	if (!m_m2k_fabric) {
		return false;
	}

	m_adc_ch0_gain = 1;
	m_adc_ch1_gain = 1;
	m_dac_a_ch_vlsb = 0;
	m_dac_b_ch_vlsb = 0;

	m_initialized = true;

	return m_initialized;
}

bool M2kCalibrationImpl::isInitialized() const
{
	return m_initialized;
}

void M2kCalibrationImpl::setAdcInCalibMode()
{
	// Make sure hardware triggers are disabled before calibrating
	__try {
		m_trigger0_mode = m_m2k_trigger->getAnalogMode(ANALOG_IN_CHANNEL_1);
		m_trigger1_mode = m_m2k_trigger->getAnalogMode(ANALOG_IN_CHANNEL_2);
		m_m2k_trigger->setAnalogMode(ANALOG_IN_CHANNEL_1, ALWAYS);
		m_m2k_trigger->setAnalogMode(ANALOG_IN_CHANNEL_2, ALWAYS);

		m_trigger_src = m_m2k_trigger->getAnalogSource();
		m_m2k_trigger->setAnalogSource(CHANNEL_1);

		/* Save the previous values for sampling frequency and oversampling ratio */
		adc_sampl_freq = m_m2k_adc->getSampleRate();
		adc_oversampl = m_m2k_adc->getOversamplingRatio();

		m_m2k_adc->setCalibscale(0, 1.0);
		m_m2k_adc->setCalibscale(1, 1.0);

		m_adc_channels_enabled.at(0) = m_m2k_adc->isChannelEnabled(0);
		m_adc_channels_enabled.at(1) = m_m2k_adc->isChannelEnabled(1);

		m_m2k_adc->enableChannel(0, true);
		m_m2k_adc->enableChannel(1, true);
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}
}

void M2kCalibrationImpl::setDacInCalibMode()
{
	__try {
		dac_a_sampl_freq = m_m2k_dac->getSampleRate(0);
		dac_a_oversampl = m_m2k_dac->getOversamplingRatio(0);
		dac_b_sampl_freq = m_m2k_dac->getSampleRate(1);
		dac_b_oversampl = m_m2k_dac->getOversamplingRatio(1);

		m_m2k_dac->setCalibscale(0, 1.0);
		m_m2k_dac->setCalibscale(1, 1.0);

		m_dac_channels_enabled.at(0) = m_m2k_dac->isChannelEnabled(0);
		m_dac_channels_enabled.at(1) = m_m2k_dac->isChannelEnabled(1);

		m_m2k_dac->enableChannel(0, true);
		m_m2k_dac->enableChannel(1, true);
	} __catch(exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}
}

void M2kCalibrationImpl::restoreAdcFromCalibMode()
{
	__try {
		m_m2k_trigger->setAnalogMode(ANALOG_IN_CHANNEL_1, m_trigger0_mode);
		m_m2k_trigger->setAnalogMode(ANALOG_IN_CHANNEL_2, m_trigger1_mode);

		m_m2k_trigger->setAnalogSource(m_trigger_src);

		/* Restore the previous values for sampling frequency and oversampling ratio */
		m_m2k_adc->setSampleRate(adc_sampl_freq);
		m_m2k_adc->setOversamplingRatio(adc_oversampl);

		m_m2k_adc->enableChannel(0, m_adc_channels_enabled.at(0));
		m_m2k_adc->enableChannel(1, m_adc_channels_enabled.at(1));
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}
}

void M2kCalibrationImpl::restoreDacFromCalibMode()
{
	__try {
		m_m2k_dac->setSampleRate(0, dac_a_sampl_freq);
		m_m2k_dac->setOversamplingRatio(0, dac_a_oversampl);
		m_m2k_dac->setSampleRate(1, dac_b_sampl_freq);
		m_m2k_dac->setOversamplingRatio(1, dac_b_oversampl);

		m_m2k_dac->enableChannel(0, m_dac_channels_enabled.at(0));
		m_m2k_dac->enableChannel(1, m_dac_channels_enabled.at(1));
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}
}

void M2kCalibrationImpl::configAdcSamplerate()
{
	// Make sure we calibrate at the highest sample rate
	__try {
		m_m2k_adc->setSampleRate(1e8);
		m_m2k_adc->setOversamplingRatio(1);
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}
}

void M2kCalibrationImpl::configDacSamplerate()
{
	__try {
		m_m2k_dac->setSampleRate(0, 75E6);
		m_m2k_dac->setOversamplingRatio(0, 1);
		m_m2k_dac->setSampleRate(1, 75E6);
		m_m2k_dac->setOversamplingRatio(1, 1);
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}
}

bool M2kCalibrationImpl::calibrateADCoffset()
{
	double gain = 1.3;
	double range = 3.192;
	bool calibrated = false;
	double voltage0, voltage1 = 0;
	std::vector<std::vector<double>> ch_data = {};

	if (!m_initialized) {
		return false;
	}

	// Ground ADC inputs
	setCalibrationMode(ADC_GND);

	// Set DAC channels to middle scale
	m_adc_ch0_vert_offset = m_m2k_adc->getRawVerticalOffset(static_cast<ANALOG_IN_CHANNEL>(0));
	m_adc_ch1_vert_offset = m_m2k_adc->getRawVerticalOffset(static_cast<ANALOG_IN_CHANNEL>(1));

	m_m2k_adc->setAdcCalibOffset(static_cast<ANALOG_IN_CHANNEL>(0), 2048);
	m_m2k_adc->setVerticalOffset(static_cast<ANALOG_IN_CHANNEL>(0), 0);
	m_m2k_adc->setAdcCalibOffset(static_cast<ANALOG_IN_CHANNEL>(1), 2048);
	m_m2k_adc->setVerticalOffset(static_cast<ANALOG_IN_CHANNEL>(1), 0);

	// Allow some time for the voltage to settle
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	const unsigned int num_samples = 1e5;
	__try {
		ch_data = m_m2k_adc->getSamplesRaw(num_samples);
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}
	if (ch_data.size() == 0) {
		return false;
	}

	int16_t ch0_avg = Utils::average(ch_data.at(0).data(), num_samples);
	int16_t ch1_avg = Utils::average(ch_data.at(1).data(), num_samples);

	// Convert from raw format to signed raw
	int16_t tmp;

	tmp = ch0_avg;
	m_m2k_adc->convertChannelHostFormat(ANALOG_IN_CHANNEL_1, &ch0_avg, &tmp);
	tmp = ch1_avg;
	m_m2k_adc->convertChannelHostFormat(ANALOG_IN_CHANNEL_2, &ch1_avg, &tmp);

	voltage0 = m_m2k_adc->convRawToVolts(ch0_avg, 1, 1);
	voltage1 = m_m2k_adc->convRawToVolts(ch1_avg, 1, 1);

	m_adc_ch0_offset = (int)(2048 - ((voltage0 * 4096 * gain) / range));
	m_adc_ch1_offset = (int)(2048 - ((voltage1 * 4096 * gain) / range));

	fine_tune(20, m_adc_ch0_offset, m_adc_ch1_offset, num_samples);

	calibrated = true;

	ch_data.clear();
	return calibrated;
}

bool M2kCalibrationImpl::calibrateADCgain()
{
	int16_t tmp;
	double vref1 = 0.46172;
	const unsigned int num_samples = 15e4;
	double avg0, avg1;
	bool calibrated = false;
	std::vector<std::vector<double>> ch_data = {};

	if (!m_initialized) {
		return false;
	}

	setCalibrationMode(ADC_REF1);

	__try {
		ch_data = m_m2k_adc->getSamplesRaw(num_samples);
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}
	if (ch_data.size() == 0) {
		return false;
	}

	avg0 = Utils::average(ch_data.at(0).data(), num_samples);
	avg1 = Utils::average(ch_data.at(1).data(), num_samples);

	tmp = avg0;
	m_m2k_adc->convertChannelHostFormat(ANALOG_IN_CHANNEL_1, &avg0, &tmp);
	tmp = avg1;
	m_m2k_adc->convertChannelHostFormat(ANALOG_IN_CHANNEL_2, &avg1, &tmp);

	avg0 = m_m2k_adc->convRawToVolts(avg0, 1, 1);
	avg1 = m_m2k_adc->convRawToVolts(avg1, 1, 1);

	m_adc_ch0_gain = vref1 / avg0;
	m_adc_ch1_gain = vref1 / avg1;

	setCalibrationMode(NONE);

	calibrated = true;

	ch_data.clear();
	return calibrated;
}

int M2kCalibrationImpl::adcOffsetChannel0() const
{
	return m_adc_ch0_offset;
}

int M2kCalibrationImpl::adcOffsetChannel1() const
{
	return m_adc_ch1_offset;
}

double M2kCalibrationImpl::adcGainChannel0() const
{
	return m_adc_ch0_gain;
}

double M2kCalibrationImpl::adcGainChannel1() const
{
	return m_adc_ch1_gain;
}

void M2kCalibrationImpl::updateDacCorrections()
{
	m_ad5625_dev->setDoubleValue(0, m_dac_a_ch_offset, "raw", true);
	m_ad5625_dev->setDoubleValue(1, m_dac_b_ch_offset, "raw", true);

	m_m2k_adc->setCalibscale(ANALOG_IN_CHANNEL_1, m_adc_ch0_gain);
	m_m2k_adc->setCalibscale(ANALOG_IN_CHANNEL_2, m_adc_ch1_gain);

	m_m2k_dac->setCalibscale(0, m_dac_default_vlsb / dacAvlsb());
	m_m2k_dac->setCalibscale(1, m_dac_default_vlsb / dacBvlsb());
}

void M2kCalibrationImpl::updateAdcCorrections()
{
	m_m2k_adc->setAdcCalibOffset(static_cast<ANALOG_IN_CHANNEL>(0), m_adc_ch0_offset, m_adc_ch0_vert_offset);
	m_m2k_adc->setAdcCalibOffset(static_cast<ANALOG_IN_CHANNEL>(1), m_adc_ch0_offset, m_adc_ch1_vert_offset);

	m_m2k_adc->setCalibscale(ANALOG_IN_CHANNEL_1, m_adc_ch0_gain);
	m_m2k_adc->setCalibscale(ANALOG_IN_CHANNEL_2, m_adc_ch1_gain);
}

bool M2kCalibrationImpl::resetCalibration()
{
	if (!m_initialized) {
		initialize();
	}

	setCalibrationMode(NONE);

	m_adc_ch0_offset = 2048;
	m_adc_ch1_offset = 2048;

	m_dac_a_ch_offset =  2048;
	m_dac_b_ch_offset =  2048;

	m_adc_ch0_gain = 1;
	m_adc_ch1_gain = 1;

	m_dac_a_ch_vlsb = 0.0034;
	m_dac_b_ch_vlsb = 0.0034;

	updateAdcCorrections();
	updateDacCorrections();

	m_adc_calibrated = false;
	m_dac_calibrated = false;
	return true;
}

bool M2kCalibrationImpl::fine_tune(size_t span, int16_t centerVal0, int16_t centerVal1,
				   size_t num_samples)
{
	int16_t *candidateOffsets0 = new int16_t[span + 1];
	int16_t *candidateOffsets1 = new int16_t[span + 1];
	double *averagesCh0 = new double[span + 1];
	double *averagesCh1 = new double[span + 1];
	double minAvg0, minAvg1;
	std::vector<std::vector<double>> ch_data = {};
	int16_t offset0, offset1;
	unsigned int i, i0 = 0, i1 = 0;
	bool ret = true;

	offset0 = centerVal0 - span / 2;
	offset1 = centerVal1 - span / 2;
	for (i = 0; i < span + 1; i++) {
		candidateOffsets0[i] = offset0;
		candidateOffsets1[i] = offset1;
		m_m2k_adc->setAdcCalibOffset(static_cast<ANALOG_IN_CHANNEL>(0), offset0);
		m_m2k_adc->setAdcCalibOffset(static_cast<ANALOG_IN_CHANNEL>(1), offset1);
		offset0++;
		offset1++;

		// Allow some time for the voltage to settle
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

		__try {
			ch_data = m_m2k_adc->getSamplesRaw(num_samples);
		} __catch (exception_type &e) {
			throw_exception(EXC_INVALID_PARAMETER, e.what());
		}
		if (ch_data.size() == 0) {
			goto out_cleanup;
		}

		averagesCh0[i] = std::abs(Utils::average(ch_data.at(0).data(), num_samples));
		averagesCh1[i] = std::abs(Utils::average(ch_data.at(1).data(), num_samples));
	}

	minAvg0 = std::abs(averagesCh0[0]);
	minAvg1 = std::abs(averagesCh1[0]);

	for (unsigned int i = 1; i < span + 1; i++) {
		if (averagesCh0[i] < minAvg0) {
			minAvg0 = averagesCh0[i];
			i0 = i;
		}
		if (averagesCh1[i] < minAvg1) {
			minAvg1 = averagesCh1[i];
			i1 = i;
		}
	}

	setCalibrationMode(NONE);

	m_adc_ch0_offset = candidateOffsets0[i0];
	m_adc_ch1_offset = candidateOffsets1[i1];

	m_ad5625_dev->setDoubleValue(2, m_adc_ch0_offset, "raw", true);
	m_ad5625_dev->setDoubleValue(3, m_adc_ch1_offset, "raw", true);

out_cleanup:
	delete[] candidateOffsets0;
	delete[] candidateOffsets1;
	delete[] averagesCh0;
	delete[] averagesCh1;
	return ret;
}

int M2kCalibrationImpl::dacAoffset() const
{
	return m_dac_a_ch_offset;
}

int M2kCalibrationImpl::dacBoffset() const
{
	return m_dac_b_ch_offset;
}

double M2kCalibrationImpl::dacAvlsb() const
{
	return m_dac_a_ch_vlsb;
}
double M2kCalibrationImpl::dacBvlsb() const
{
	return m_dac_b_ch_vlsb;
}

bool M2kCalibrationImpl::calibrateDACoffset()
{
	int16_t tmp;
	bool calibrated = false;
	const unsigned int num_samples = 1e5;
	std::vector<std::vector<double>> ch_data = {};

	if (!m_initialized) {
		return false;
	}

	// connect ADC to DAC
	setCalibrationMode(DAC);

	// Set DAC offset channels to middle scale
	m_ad5625_dev->setDoubleValue(0, 2048, "raw", true);
	m_ad5625_dev->setDoubleValue(1, 2048, "raw", true);

	// write to DAC
	int16_t value = processRawSample(0);
	std::vector<short> vec_data(256, value);
	std::vector<std::vector<short>> vec_data_all;
	__try {
		vec_data_all.push_back(vec_data);
		vec_data_all.push_back(vec_data);

		//powerdown true before, false after->to start it,
		// should set synced all?
		m_m2k_dac->setSyncedDma(true);
		m_m2k_dac->enableChannel(0, true);
		m_m2k_dac->enableChannel(1, true);

		m_m2k_dac->setCyclic(true);
		m_m2k_dac->pushRaw(vec_data_all);

		m_m2k_dac->setSyncedDma(false);
		m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, false, "powerdown", true);


	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, "DAC offset calibration failed: "
				+ std::string(e.what()));
	}

	// Allow some time for the voltage to settle
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	__try {
		ch_data = m_m2k_adc->getSamplesRaw(num_samples);
	} __catch (exception_type &e) {
		ch_data.clear();
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}
	if (ch_data.size() == 0) {
		return false;
	}

	int16_t ch0_avg = Utils::average(ch_data.at(0).data(), num_samples);
	int16_t ch1_avg = Utils::average(ch_data.at(1).data(), num_samples);

	tmp = ch0_avg;
	m_m2k_adc->convertChannelHostFormat(ANALOG_IN_CHANNEL_1, &ch0_avg, &tmp);
	tmp = ch1_avg;
	m_m2k_adc->convertChannelHostFormat(ANALOG_IN_CHANNEL_2, &ch1_avg, &tmp);

	double voltage0 = m_m2k_adc->convRawToVolts(
				ch0_avg, m_adc_ch0_gain, 1);
	double voltage1 = m_m2k_adc->convRawToVolts(
				ch1_avg, m_adc_ch1_gain, 1);

	m_dac_a_ch_offset = (int)(2048 - ((voltage0 * 9.06 ) / 0.002658));
	m_dac_b_ch_offset = (int)(2048 - ((voltage1 * 9.06 ) / 0.002658));

	m_ad5625_dev->setDoubleValue(0, m_dac_a_ch_offset, "raw", true);
	m_ad5625_dev->setDoubleValue(1, m_dac_b_ch_offset, "raw", true);

	__try {
		m_m2k_dac->stop();
		m_m2k_dac->enableChannel(0, false);
		m_m2k_dac->enableChannel(1, false);
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}

	setCalibrationMode(NONE);

	calibrated = true;

	ch_data.clear();
	return calibrated;
}

bool M2kCalibrationImpl::calibrateDACgain()
{
	int16_t tmp;
	bool calibrated = false;
	std::vector<std::vector<double>> ch_data = {};

	// connect ADC to DAC
	setCalibrationMode(DAC);

	int16_t value = processRawSample(1024);
	// Use the positive half scale point for gain calibration
	std::vector<short> vec_data(256, value);
	std::vector<std::vector<short>> vec_data_all;
	__try {
		vec_data_all.push_back(vec_data);
		vec_data_all.push_back(vec_data);

		//powerdown true before, false after->to start it,
		// should set synced all?
		m_m2k_dac->setSyncedDma(true);
		m_m2k_dac->enableChannel(0, true);
		m_m2k_dac->enableChannel(1, true);

		m_m2k_dac->setCyclic(true);
		m_m2k_dac->pushRaw(vec_data_all);

		m_m2k_dac->setSyncedDma(false);
		m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, false, "powerdown", true);
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, "DAC gain calibration failed: "
				+ std::string(e.what()));
	}

	// Allow some time for the voltage to settle
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	const unsigned int num_samples = 15e4;
	__try {
		ch_data = m_m2k_adc->getSamplesRaw(num_samples);
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}
	if (ch_data.size() == 0) {
		return false;
	}

	int16_t ch0_avg = Utils::average(ch_data.at(0).data(), num_samples);
	int16_t ch1_avg = Utils::average(ch_data.at(1).data(), num_samples);

	tmp = ch0_avg;
	m_m2k_adc->convertChannelHostFormat(ANALOG_IN_CHANNEL_1, &ch0_avg, &tmp);
	tmp = ch1_avg;
	m_m2k_adc->convertChannelHostFormat(ANALOG_IN_CHANNEL_2, &ch1_avg, &tmp);

	double voltage0 = m_m2k_adc->convRawToVolts(
				ch0_avg, m_adc_ch0_gain, 1);
	double voltage1 = m_m2k_adc->convRawToVolts(
				ch1_avg, m_adc_ch1_gain, 1);

	// Taking into account the voltage divider on the loopback path
	voltage0 *= 9.06;
	voltage1 *= 9.06;

	m_dac_a_ch_vlsb = voltage0 / 1024;
	m_dac_b_ch_vlsb = voltage1 / 1024;

	__try {
		m_m2k_dac->stop();
		m_m2k_dac->enableChannel(0, false);
		m_m2k_dac->enableChannel(1, false);
	} __catch (exception_type &e) {
		//		throw std::runtime_error(
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}

	setCalibrationMode(NONE);

	calibrated = true;

	ch_data.clear();
	return calibrated;
}

bool M2kCalibrationImpl::calibrateADC()
{
	bool ok;
	__try {
		if (!m_initialized) {
			initialize();
		}

		setAdcInCalibMode();

		configAdcSamplerate();
		ok = calibrateADCoffset();
		if (!ok || m_cancel) {
			goto calibration_fail;
		}

		ok = calibrateADCgain();

		if (!ok || m_cancel) {
			goto calibration_fail;
		}

		updateAdcCorrections();
		restoreAdcFromCalibMode();
		m_adc_calibrated = true;
		m_m2k_adc->stopAcquisition();
		return true;
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, "ADC calibration failed: " +
				std::string(e.what()));
	}
calibration_fail:
	m_cancel=false;
	m_adc_calibrated = false;
	return false;
}

bool M2kCalibrationImpl::calibrateDAC()
{
	bool ok;
	__try {
		if (!m_initialized) {
			initialize();
		}

		if (!m_adc_calibrated) {
			calibrateADC();
		}
		setDacInCalibMode();
		setAdcInCalibMode();

		configAdcSamplerate();
		configDacSamplerate();

		ok = calibrateDACoffset();

		if (!ok || m_cancel) {
			goto calibration_fail;
		}

		ok = calibrateDACgain();

		if (!ok  || m_cancel) {
			goto calibration_fail;
		}

		updateDacCorrections();
		restoreDacFromCalibMode();
		restoreAdcFromCalibMode();

		m_dac_calibrated = true;
		m_m2k_adc->stopAcquisition();
		return true;
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, "DAC calibration failed " +
				std::string(e.what()));
	}
calibration_fail:
	m_cancel=false;
	m_dac_calibrated = false;
	return false;
}

bool M2kCalibrationImpl::calibrateAll()
{
	bool ok;
	__try {
		initialize();

		ok = calibrateADC();

		if (!ok || m_cancel) {
			goto calibration_fail;
		}

		if (Utils::getHardwareRevision(m_ctx) != "A") {
			std::this_thread::sleep_for
					(std::chrono::milliseconds(750));
		}

		ok = calibrateDAC();

		if (!ok || m_cancel) {
			goto calibration_fail;
		}

		return true;
	} __catch (exception_type &e) {
		throw_exception(EXC_INVALID_PARAMETER, e.what());
	}
calibration_fail:
	m_cancel=false;
	return false;
}

void M2kCalibrationImpl::cancelCalibration()
{
	m_cancel=true;
}

bool M2kCalibrationImpl::setCalibrationMode(int mode)
{
	std::string strMode;
	switch (mode) {
	case ADC_GND:
		strMode = "adc_gnd";
		break;
	case ADC_REF1:
		strMode = "adc_ref1";
		break;
	case ADC_REF2:
		strMode = "adc_ref2";
		break;
	case DAC:
		strMode = "dac";
		break;
	case libm2k::NONE:
		strMode = "none";
		break;
	default:
		return false;
	}
	m_m2k_fabric->setStringValue("calibration_mode", strMode);
	m_calibration_mode = mode;
	return true;
}

int16_t M2kCalibrationImpl::processRawSample(int16_t value)
{
	/* 12-bit DAC values must be 16-bit MSB aligned. */
	return ((-value) << 4);
}
