/*
 * Copyright 2016 Analog Devices, Inc.
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

#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <libm2k/m2kcalibration.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>
#include <libm2k/enums.hpp>

#include <errno.h>
#include <iio.h>
#include <vector>
#include <thread>
#include <chrono>

using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::utils;

M2kCalibration::M2kCalibration(struct iio_context* ctx, M2kAnalogIn* analogIn,
			       M2kAnalogOut* analogOut):
	m_cancel(false),
	m_ctx(ctx),
	m_m2k_adc(analogIn),
	m_m2k_dac(analogOut),
	m_adc_calibrated(false),
	m_dac_calibrated(false),
	m_initialized(false),
	m_ad5625_dev(nullptr)
{
	m_ad5625_dev = make_shared<Device>(m_ctx, "ad5625");
}

M2kCalibration::~M2kCalibration()
{
}

bool M2kCalibration::initialize()
{
	m_initialized = false;

	if (!m_ctx)
		return false;

	m_m2k_fabric = make_shared<Device>(m_ctx, "m2k-fabric");
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

bool M2kCalibration::isInitialized() const
{
	return m_initialized;
}

void M2kCalibration::setAdcInCalibMode()
{
	// Make sure hardware triggers are disabled before calibrating
	try {
		m_trigger0_mode = m_m2k_adc->getTriggerMode(ANALOG_IN_CHANNEL_1);
		m_trigger1_mode = m_m2k_adc->getTriggerMode(ANALOG_IN_CHANNEL_2);
		m_m2k_adc->setTriggerMode(ANALOG_IN_CHANNEL_1, ALWAYS);
		m_m2k_adc->setTriggerMode(ANALOG_IN_CHANNEL_2, ALWAYS);

		/* Save the previous values for sampling frequency and oversampling ratio */
		adc_sampl_freq = m_m2k_adc->getSamplerate();
		adc_oversampl = m_m2k_adc->getOversamplingRatio();
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kCalibration::setDacInCalibMode()
{
	try {
		dac_a_sampl_freq = m_m2k_dac->getSamplerate(0);
		dac_a_oversampl = m_m2k_dac->getOversamplingRatio(0);
		dac_b_sampl_freq = m_m2k_dac->getSamplerate(1);
		dac_b_oversampl = m_m2k_dac->getOversamplingRatio(1);
	} catch(std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kCalibration::restoreAdcFromCalibMode()
{
	try {
		m_m2k_adc->setTriggerMode(ANALOG_IN_CHANNEL_1, m_trigger0_mode);
		m_m2k_adc->setTriggerMode(ANALOG_IN_CHANNEL_2, m_trigger1_mode);

		/* Restore the previous values for sampling frequency and oversampling ratio */
		m_m2k_adc->setSamplerate(adc_sampl_freq);
		m_m2k_adc->setOversamplingRatio(adc_oversampl);

	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kCalibration::restoreDacFromCalibMode()
{
	try {
		m_m2k_dac->setSamplerate(0, dac_a_sampl_freq);
		m_m2k_dac->setOversamplingRatio(0, dac_a_oversampl);
		m_m2k_dac->setSamplerate(1, dac_b_sampl_freq);
		m_m2k_dac->setOversamplingRatio(1, dac_b_oversampl);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kCalibration::configAdcSamplerate()
{
	// Make sure we calibrate at the highest sample rate
	try {
		m_m2k_adc->setSamplerate(1e8);
		m_m2k_adc->setOversamplingRatio(1);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kCalibration::configDacSamplerate()
{
	try {
		m_m2k_dac->setSamplerate(0, 75E6);
		m_m2k_dac->setOversamplingRatio(0, 1);
		m_m2k_dac->setSamplerate(1, 75E6);
		m_m2k_dac->setOversamplingRatio(1, 1);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

bool M2kCalibration::calibrateADCoffset()
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
	m_ad5625_dev->setDoubleValue(2, 2048, "raw", true);
	m_ad5625_dev->setDoubleValue(3, 2048, "raw", true);

	// Allow some time for the voltage to settle
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	const unsigned int num_samples = 1e5;
	try {
		ch_data = m_m2k_adc->getSamples(num_samples);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
	if (ch_data.size() == 0) {
		return false;
	}

	int16_t ch0_avg = Utils::average(ch_data.at(0).data(), num_samples);
	int16_t ch1_avg = Utils::average(ch_data.at(0).data(), num_samples);

	// Convert from raw format to signed raw
	int16_t tmp;

	tmp = ch0_avg;
	m_m2k_adc->convertChannelHostFormat(ANALOG_IN_CHANNEL_1, &ch0_avg, &tmp);
	tmp = ch1_avg;
	m_m2k_adc->convertChannelHostFormat(ANALOG_IN_CHANNEL_2, &ch1_avg, &tmp);

	voltage0 = m_m2k_adc->convertRawToVolts(ch0_avg, 1, 1);
	voltage1 = m_m2k_adc->convertRawToVolts(ch1_avg, 1, 1);

	m_adc_ch0_offset = (int)(2048 - ((voltage0 * 4096 * gain) / range));
	m_adc_ch1_offset = (int)(2048 - ((voltage1 * 4096 * gain) / range));

	fine_tune(20, m_adc_ch0_offset, m_adc_ch1_offset, num_samples);

	calibrated = true;

	ch_data.clear();
	return calibrated;
}

bool M2kCalibration::calibrateADCgain()
{
	int16_t tmp;
	double vref1 = 0.46172;
	const unsigned int num_samples = 1e5;
	double avg0, avg1;
	bool calibrated = false;
	std::vector<std::vector<double>> ch_data = {};

	if (!m_initialized) {
		return false;
	}

	setCalibrationMode(ADC_REF1);

	try {
		ch_data = m_m2k_adc->getSamples(num_samples);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
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

	avg0 = m_m2k_adc->convertRawToVolts(avg0, 1, 1);
	avg1 = m_m2k_adc->convertRawToVolts(avg1, 1, 1);

	m_adc_ch0_gain = vref1 / avg0;
	m_adc_ch1_gain = vref1 / avg1;

	setCalibrationMode(NONE);

	calibrated = true;

	ch_data.clear();
	return calibrated;
}

int M2kCalibration::adcOffsetChannel0() const
{
	return m_adc_ch0_offset;
}

int M2kCalibration::adcOffsetChannel1() const
{
	return m_adc_ch1_offset;
}

double M2kCalibration::adcGainChannel0() const
{
	return m_adc_ch0_gain;
}

double M2kCalibration::adcGainChannel1() const
{
	return m_adc_ch1_gain;
}

void M2kCalibration::updateDacCorrections()
{
	m_ad5625_dev->setDoubleValue(0, m_dac_a_ch_offset, "raw", true);
	m_ad5625_dev->setDoubleValue(1, m_dac_b_ch_offset, "raw", true);

	m_m2k_dac->setDacCalibVlsb(0, dacAvlsb());
	m_m2k_dac->setDacCalibVlsb(1, dacBvlsb());
}

void M2kCalibration::updateAdcCorrections()
{
	m_ad5625_dev->setDoubleValue(2, m_adc_ch0_offset, "raw", true);
	m_ad5625_dev->setDoubleValue(3, m_adc_ch1_offset, "raw", true);

	m_m2k_adc->setAdcCalibGain(ANALOG_IN_CHANNEL_1, m_adc_ch0_gain);
	m_m2k_adc->setAdcCalibGain(ANALOG_IN_CHANNEL_2, m_adc_ch1_gain);
}

bool M2kCalibration::resetCalibration()
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

bool M2kCalibration::fine_tune(size_t span, int16_t centerVal0, int16_t centerVal1,
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
		m_ad5625_dev->setDoubleValue(2, offset0, "raw", true);
		m_ad5625_dev->setDoubleValue(3, offset1, "raw", true);
		offset0++;
		offset1++;

		// Allow some time for the voltage to settle
		std::this_thread::sleep_for(std::chrono::milliseconds(5));

		try {
			ch_data = m_m2k_adc->getSamples(num_samples);
		} catch (std::runtime_error &e) {
			throw invalid_parameter_exception(e.what());
		}
		if (ch_data.size() == 0) {
			goto out_cleanup;
		}

		averagesCh0[i] = abs(Utils::average(ch_data.at(0).data(), num_samples));
		averagesCh1[i] = abs(Utils::average(ch_data.at(1).data(), num_samples));
	}

	minAvg0 = abs(averagesCh0[0]);
	minAvg1 = abs(averagesCh1[0]);

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

int M2kCalibration::dacAoffset() const
{
	return m_dac_a_ch_offset;
}

int M2kCalibration::dacBoffset() const
{
	return m_dac_b_ch_offset;
}

double M2kCalibration::dacAvlsb() const
{
	return m_dac_a_ch_vlsb;
}
double M2kCalibration::dacBvlsb() const
{
	return m_dac_b_ch_vlsb;
}

bool M2kCalibration::calibrateDACoffset()
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
	std::vector<short> vec_data(256, 0);
	std::vector<std::vector<short>> vec_data_all;
	try {
		vec_data_all.push_back(vec_data);
		vec_data_all.push_back(vec_data);

		//powerdown true before, false after->to start it,
		// should set synced all?
		m_m2k_dac->setSyncedDma(true);
		m_m2k_dac->enableChannel(0, true);
		m_m2k_dac->enableChannel(1, true);

		m_m2k_dac->setCyclic(true);
		m_m2k_dac->push(vec_data_all);

		m_m2k_dac->setSyncedDma(false);
		m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, false, "powerdown", true);


	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception("DAC offset calibration failed: "
						  + std::string(e.what()));
	}

	// Allow some time for the voltage to settle
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	try {
		ch_data = m_m2k_adc->getSamples(num_samples);
	} catch (std::runtime_error &e) {
		ch_data.clear();
		throw invalid_parameter_exception(e.what());
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

	double voltage0 = m_m2k_adc->convertRawToVolts(
				ch0_avg, m_adc_ch0_gain, 1);
	double voltage1 = m_m2k_adc->convertRawToVolts(
				ch1_avg, m_adc_ch1_gain, 1);

	m_dac_a_ch_offset = (int)(2048 - ((voltage0 * 9.06 ) / 0.002658));
	m_dac_b_ch_offset = (int)(2048 - ((voltage1 * 9.06 ) / 0.002658));

	m_ad5625_dev->setDoubleValue(0, m_dac_a_ch_offset, "raw", true);
	m_ad5625_dev->setDoubleValue(1, m_dac_b_ch_offset, "raw", true);

	try {
		m_m2k_dac->stop();
		m_m2k_dac->enableChannel(0, false);
		m_m2k_dac->enableChannel(1, false);
	} catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}

	setCalibrationMode(NONE);

	calibrated = true;

	ch_data.clear();
	return calibrated;
}

bool M2kCalibration::calibrateDACgain()
{
	int16_t tmp;
	bool calibrated = false;
	std::vector<std::vector<double>> ch_data = {};

	// connect ADC to DAC
	setCalibrationMode(DAC);

	// Use the positive half scale point for gain calibration
	std::vector<short> vec_data(256, 1024);
	std::vector<std::vector<short>> vec_data_all;
	try {
		vec_data_all.push_back(vec_data);
		vec_data_all.push_back(vec_data);

		//powerdown true before, false after->to start it,
		// should set synced all?
		m_m2k_dac->setSyncedDma(true);
		m_m2k_dac->enableChannel(0, true);
		m_m2k_dac->enableChannel(1, true);

		m_m2k_dac->setCyclic(true);
		m_m2k_dac->push(vec_data_all);

		m_m2k_dac->setSyncedDma(false);
		m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, false, "powerdown", true);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception("DAC gain calibration failed: "
						  + std::string(e.what()));
	}

	// Allow some time for the voltage to settle
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	const unsigned int num_samples = 1e5;
	try {
		ch_data = m_m2k_adc->getSamples(num_samples);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
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

	double voltage0 = m_m2k_adc->convertRawToVolts(
				ch0_avg, m_adc_ch0_gain, 1);
	double voltage1 = m_m2k_adc->convertRawToVolts(
				ch1_avg, m_adc_ch1_gain, 1);

	// Taking into account the voltage divider on the loopback path
	voltage0 *= 9.06;
	voltage1 *= 9.06;

	m_dac_a_ch_vlsb = voltage0 / 1024;
	m_dac_b_ch_vlsb = voltage1 / 1024;

	try {
		m_m2k_dac->stop();
		m_m2k_dac->enableChannel(0, false);
		m_m2k_dac->enableChannel(1, false);
	} catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}

	setCalibrationMode(NONE);

	calibrated = true;

	ch_data.clear();
	return calibrated;
}

/*
void M2kCalibration::dacOutputDC(struct iio_device *dac,
	struct iio_channel *channel, struct iio_buffer** buffer, size_t value)
{
	const size_t size = 256;
	int16_t data[size];

	value = (-value) << 4; // This should go away once channel type gets
	// changed from 'le:S16/16>>0' to 'le:S12/16>>4'
	setChannelEnableState(channel, true);

	if (*buffer)
		iio_buffer_destroy(*buffer);

	*buffer = iio_device_create_buffer(dac,
			size, true);
	if (!(*buffer)) {
		return;
	}

	std::fill_n(data, size, value);
	iio_channel_write(channel, *buffer, data, size * sizeof(data[0]));

	iio_buffer_push(*buffer);
}

void M2kCalibration::dacAOutputDCVolts(int16_t dac_a_volts)
{
	short dac_a_raw;
	setCalibrationMode(NONE);
	m_m2k_dac->enableChannel(0, true);
	m_m2k_dac->setSyncedDma(0, true);
	dac_a_raw = (dac_a_volts / m_dac_a_ch_vlsb);
	dac_a_raw = (-dac_a_raw) << 4; // This should go away once channel type gets
	// changed from 'le:S16/16>>0' to 'le:S12/16>>4'

	std::vector<short> vec_data(256, dac_a_raw);
	std::vector<std::vector<short>> vec_data_all;
	m_m2k_dac->setSyncedDma(0, false);

	iio_channel_attr_write_bool(m_dac_a_fabric, "powerdown", false);
}

void M2kCalibration::dacBOutputDCVolts(int16_t dac_b_volts)
{
	int dac_b_raw;
	setCalibrationMode(NONE);
	setChannelEnableState(m_dac_b_channel, true);
//	iio_device_attr_write_bool(m_m2k_dac_b, "dma_sync", true);
	dac_b_raw = (dac_b_volts / m_dac_b_ch_vlsb);
	dacBOutputDC(dac_b_raw);

//	iio_device_attr_write_bool(m_m2k_dac_b, "dma_sync", false);

	iio_channel_attr_write_bool(m_dac_b_fabric, "powerdown", false);
}

void M2kCalibration::dacAOutputDC(int16_t value)
{
//	dacOutputDC(m_m2k_dac_a, m_dac_a_channel, &m_dac_a_buffer, value);
}

void M2kCalibration::dacOutputStop()
{
	m_m2k_dac->stop();

	setCalibrationMode(NONE);
}

void M2kCalibration::dacBOutputDC(int16_t value)
{
//	dacOutputDC(m_m2k_dac_b, m_dac_b_channel, &m_dac_b_buffer, value);
}
*/

bool M2kCalibration::calibrateADC()
{
	bool ok;
	try {
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
		return true;
	} catch (std::runtime_error &e) {
		throw std::runtime_error("ADC calibration failed: " +
					 std::string(e.what()));
	}
calibration_fail:
	m_cancel=false;
	m_adc_calibrated = false;
	return false;
}

bool M2kCalibration::calibrateDAC()
{
	bool ok;
	try {
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
		return true;
	} catch (std::runtime_error &e) {
		throw std::runtime_error("DAC calibration failed " +
					 std::string(e.what()));
	}
calibration_fail:
	m_cancel=false;
	m_dac_calibrated = false;
	return false;
}

bool M2kCalibration::calibrateAll()
{
	bool ok;
	try {
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
	} catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
calibration_fail:
	m_cancel=false;
	return false;
}

void M2kCalibration::cancelCalibration()
{
	m_cancel=true;
}

bool M2kCalibration::setGainMode(int ch, int mode)
{
//	switch (mode) {
//	case HIGH:
//		m2k_adc->setChnHwGainMode(ch, M2kAdc::HIGH_GAIN_MODE);
//		break;
//	case LOW:
//		m2k_adc->setChnHwGainMode(ch, M2kAdc::LOW_GAIN_MODE);
//		break;
//	default:
//		return false;
//	}
	return true;
}

bool M2kCalibration::setCalibrationMode(int mode)
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

