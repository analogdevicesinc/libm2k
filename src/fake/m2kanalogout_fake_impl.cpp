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
#include "m2kanalogout_fake_impl.hpp"
#include "utils/devicegeneric.hpp"
#include "utils/deviceout.hpp"
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>
#include "utils/channel.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <iio.h>
#include <string.h>
#include <algorithm>

using namespace libm2k::analog;
using namespace libm2k::utils;
using namespace std;


std::vector<std::vector<double>> M2kFakeAnalogOutImpl::data_buffers_raw;

M2kFakeAnalogOutImpl::M2kFakeAnalogOutImpl(iio_context *ctx, std::vector<std::string> dac_devs, bool sync)
{
	m_dac_devices.push_back(new DeviceOut(ctx, dac_devs.at(0)));
	m_dac_devices.push_back(new DeviceOut(ctx, dac_devs.at(1)));

	m_m2k_fabric = make_shared<DeviceGeneric>(ctx, "m2k-fabric");
	if (!m_m2k_fabric) {
		THROW_M2K_EXCEPTION("Analog out: Cannot find m2k fabric device", libm2k::EXC_INVALID_PARAMETER);
	}

	m_calib_vlsb.push_back(10.0 / ((double)( 1 << 12 ) - 1));
	m_calib_vlsb.push_back(10.0 / ((double)( 1 << 12 ) - 1));

	m_filter_compensation_table[75E6] = 1.00;
	m_filter_compensation_table[75E5] = 1.525879;
	m_filter_compensation_table[75E4] = 1.164153;
	m_filter_compensation_table[75E3] = 1.776357;
	m_filter_compensation_table[75E2] = 1.355253;
	m_filter_compensation_table[75E1] = 1.033976;

	for (unsigned int i = 0; i < m_dac_devices.size(); i++) {
		m_cyclic.push_back(true);
		m_samplerate.push_back(75E6);
		m_nb_kernel_buffers.push_back(4);
		m_max_samplerate.push_back(-1);
		m_oversampling_ratio.push_back(1);
	}

	if (sync) {
		syncDevice();
	}
	// dma_start_sync attribute is only available in firmware versions newer than 0.24
	m_dma_start_sync_available = getDacDevice(0)->hasGlobalAttribute("dma_sync_start");

	// data_available attribute exists only in firmware versions newer than 0.23
	m_dma_data_available = getDacDevice(0)->hasBufferAttribute("data_available");
}

M2kFakeAnalogOutImpl::~M2kFakeAnalogOutImpl()
{
	for (auto d : m_dac_devices) {
		delete d;
	}
	m_dac_devices.clear();
}

void M2kFakeAnalogOutImpl::deinitialize()
{
	stop();
}

void M2kFakeAnalogOutImpl::reset()
{
	stop();
	setSyncedDma(false);
	for (unsigned int i = 0; i < getNbChannels(); i++) {
		m_samplerate.at(i) = 75E6;
		m_cyclic.at(i) = true;
		m_calib_vlsb.at(i) = 10.0 / ((double)( 1 << 12 ) - 1);
		setSampleRate(i, 75E6);
		setOversamplingRatio(i, 1);
		setKernelBuffersCount(i, 4);
	}
}

void M2kFakeAnalogOutImpl::syncDevice()
{
	m_samplerate.at(0) = getSampleRate(0);
	m_samplerate.at(1) = getSampleRate(1);
	//enable???
}

double M2kFakeAnalogOutImpl::getCalibscale(unsigned int index)
{
	return 1;//getDacDevice(index)->getDoubleValue("calibscale");
}

double M2kFakeAnalogOutImpl::setCalibscale(unsigned int index, double calibscale)
{
	return 1;//getDacDevice(index)->setDoubleValue(calibscale, "calibscale");
}

std::vector<int> M2kFakeAnalogOutImpl::getOversamplingRatio()
{
	std::vector<int> values = {};
	for (unsigned int i = 0; i < m_oversampling_ratio.size(); i++) {
		int val = m_oversampling_ratio.at(i);//->getLongValue("oversampling_ratio");
		values.push_back(val);
	}
	return values;
}

int M2kFakeAnalogOutImpl::getOversamplingRatio(unsigned int chn_idx)
{
	return m_oversampling_ratio.at(chn_idx);//getDacDevice(chn_idx)->getLongValue("oversampling_ratio");
}

std::vector<int> M2kFakeAnalogOutImpl::setOversamplingRatio(std::vector<int> oversampling_ratio)
{
	std::vector<int> values = {};
	for (unsigned int i = 0; i < oversampling_ratio.size(); i++) {
		m_oversampling_ratio.at(i) = oversampling_ratio.at(i);//m_dac_devices.at(i)->setLongValue(oversampling_ratio.at(i),
							   // "oversampling_ratio");
		values.push_back(m_oversampling_ratio.at(i));
	}
	return values;
}

int M2kFakeAnalogOutImpl::setOversamplingRatio(unsigned int chn_idx, int oversampling_ratio)
{
	m_oversampling_ratio.at(chn_idx) = oversampling_ratio;
	return m_oversampling_ratio.at(chn_idx);//getDacDevice(chn_idx)->setLongValue(oversampling_ratio,
		//				   "oversampling_ratio");
}

std::vector<double> M2kFakeAnalogOutImpl::getSampleRate()
{
	std::vector<double> values = {};
	for (unsigned int i = 0; i < m_dac_devices.size(); i++) {
		double val = m_samplerate.at(i);//m_dac_devices.at(i)->getDoubleValue("sampling_frequency");
		values.push_back(val);
	}
	return values;
}

double M2kFakeAnalogOutImpl::getSampleRate(unsigned int chn_idx)
{
	return m_samplerate.at(chn_idx);//getDacDevice(chn_idx)->getDoubleValue("sampling_frequency");
}

std::vector<double> M2kFakeAnalogOutImpl::setSampleRate(std::vector<double> samplerates)
{
	if (samplerates.size() >= m_dac_devices.size()) {
		THROW_M2K_EXCEPTION("Analog Out: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	std::vector<double> values = {};
	for (unsigned int i = 0; i < samplerates.size(); i++) {
		m_samplerate.at(i) = samplerates.at(i);//m_dac_devices.at(i)->setDoubleValue(samplerates.at(i),
							//		 "sampling_frequency");
		values.push_back(m_samplerate.at(i));
	}
	return values;
}

double M2kFakeAnalogOutImpl::setSampleRate(unsigned int chn_idx, double samplerate)
{
	if (chn_idx >= m_dac_devices.size()) {
		THROW_M2K_EXCEPTION("Analog Out: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	m_samplerate.at(chn_idx) = samplerate;//getDacDevice(chn_idx)->setDoubleValue(samplerate, "sampling_frequency");
	return m_samplerate.at(chn_idx);
}

void M2kFakeAnalogOutImpl::setSyncedDma(bool en, int chn)
{
	if (chn < 0) {
		for (auto dac : m_dac_devices) {
			dac->setBoolValue(en, "dma_sync");
		}
	} else {
		getDacDevice(chn)->setBoolValue(en, "dma_sync");
	}
}

bool M2kFakeAnalogOutImpl::getSyncedDma(int chn)
{
	return getDacDevice(chn)->getBoolValue("dma_sync");
}

void M2kFakeAnalogOutImpl::setSyncedStartDma(bool en, int chn)
{
	if (!m_dma_start_sync_available) {
		THROW_M2K_EXCEPTION("Invalid firmware version: 0.24 or greater is required.", libm2k::EXC_INVALID_FIRMWARE_VERSION);
	}
	if (chn < 0) {
		for (auto dac : m_dac_devices) {
			dac->setBoolValue(en, "dma_sync_start");
		}
	} else {
		getDacDevice(chn)->setBoolValue(en, "dma_sync_start");
	}
}

bool M2kFakeAnalogOutImpl::getSyncedStartDma(int chn)
{
	if (!m_dma_start_sync_available) {
		THROW_M2K_EXCEPTION("Invalid firmware version: 0.24 or greater is required.", libm2k::EXC_INVALID_FIRMWARE_VERSION);
	}
	return getDacDevice(chn)->getBoolValue("dma_sync_start");
}

void M2kFakeAnalogOutImpl::setCyclic(bool en)
{
	for (unsigned int i = 0; i < m_dac_devices.size(); i++) {
		m_cyclic.at(i) = en;
		m_dac_devices.at(i)->setCyclic(en);
	}
}

void M2kFakeAnalogOutImpl::setCyclic(unsigned int chn, bool en)
{
	//getDacDevice(chn)->setCyclic(en);
	m_cyclic.at(chn) = en;
}

bool M2kFakeAnalogOutImpl::getCyclic(unsigned int chn)
{
	if (chn >= m_dac_devices.size()) {
		THROW_M2K_EXCEPTION("Analog Out: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	return m_cyclic.at(chn);
}

short M2kFakeAnalogOutImpl::convVoltsToRaw(double voltage, double vlsb,
				       double filterCompensation)
{
	// TO DO: explain this formula....
	return (short)(((voltage * (-1 * (1 / vlsb)) - 0.5) / filterCompensation)  ) << 4;
}

short M2kFakeAnalogOutImpl::convertVoltsToRaw(unsigned int channel, double voltage)
{
	if (channel >= m_dac_devices.size()) {
		THROW_M2K_EXCEPTION("Analog Out: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}

	return convVoltsToRaw(voltage, m_calib_vlsb.at(channel),
			      getFilterCompensation(
				      m_samplerate.at(channel)));
}

double M2kFakeAnalogOutImpl::convRawToVolts(short raw, double vlsb, double filterCompensation)
{
	return -((((double)(raw >> 4) + 0.5) * filterCompensation * vlsb));
}

double M2kFakeAnalogOutImpl::convertRawToVolts(unsigned int channel, short raw)
{
	if (channel >= m_dac_devices.size()) {
		THROW_M2K_EXCEPTION("Analog Out: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}

	return convRawToVolts(raw, m_calib_vlsb.at(channel),
			      getFilterCompensation(
				      m_samplerate.at(channel)));
}

void M2kFakeAnalogOutImpl::setDacCalibVlsb(unsigned int chn_idx, double vlsb)
{
	m_calib_vlsb[chn_idx] = vlsb;
}


void M2kFakeAnalogOutImpl::pushRaw(unsigned int chnIdx, std::vector<short> const &data)
{
	short *ptr = (short*)data.data();
	pushRawBytes(chnIdx, ptr, data.size());
}

void M2kFakeAnalogOutImpl::pushRawBytes(unsigned int chnIdx, short *data, unsigned int nb_samples)
{
	if (chnIdx >= m_dac_devices.size()) {
		THROW_M2K_EXCEPTION("Analog Out: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}

	m_dac_devices.at(chnIdx)->push(data, 0, nb_samples, getCyclic(chnIdx));

	setSyncedDma(false, chnIdx);
}


void M2kFakeAnalogOutImpl::push(unsigned int chnIdx, std::vector<double> const &data)
{
	double *ptr = (double*)data.data();
	pushBytes(chnIdx, ptr, data.size());
}


void M2kFakeAnalogOutImpl::pushBytes(unsigned int chnIdx, double *data, unsigned int nb_samples)
{
	if (chnIdx >= m_dac_devices.size()) {
		THROW_M2K_EXCEPTION("Analog Out: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	std::vector<short> raw_data_buffer = {};

	for (unsigned int i = 0; i < nb_samples; i++) {
		raw_data_buffer.push_back(convertVoltsToRaw(chnIdx, data[i]));
	}
	m_dac_devices.at(chnIdx)->push(raw_data_buffer, 0, getCyclic(chnIdx));
}


/*
	 * push short (raw) on multiple channels
	 */
void M2kFakeAnalogOutImpl::pushRaw(std::vector<std::vector<short>> const &data)
{
	std::vector<std::vector<short>> data_buffers;
	bool streamingData = true;
	bool isBufferEmpty = true;
	bool allChannelsPushed = (data.size() != getNbChannels()) ? false : true;

	for (unsigned int  chn = 0; chn < data.size(); chn++) {
		streamingData &= !getCyclic(chn);
		allChannelsPushed &= (data.at(chn).size() != 0);
	}
	if (streamingData && m_dma_data_available) {
		// all kernel buffers are empty when maximum buffer space is equal with the unused space
		unsigned int unusedBufferSpace, maxBufferSpace;
		for (unsigned int  chn = 0; chn < data.size(); chn++) {
			size_t size = data.at(chn).size();
			m_dac_devices.at(chn)->initializeBuffer(size, false);
			unusedBufferSpace = m_dac_devices[chn]->getBufferLongValue("data_available");
			maxBufferSpace = 2u * size * (m_nb_kernel_buffers.at(chn) - 1);
			isBufferEmpty &= (maxBufferSpace == unusedBufferSpace);
		}
		if (isBufferEmpty) {
			// when kernel buffers are empty both channels must be synchronized
			setSyncedDma(true);
		}
	} else {
		setSyncedDma(true);
	}

	for (unsigned int chn = 0; chn < data.size(); chn++) {
		size_t size = data.at(chn).size();
		std::vector<short> raw_data_buffer(data.at(chn).data(), data.at(chn).data() + size);
		m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
	}

	if ((streamingData && isBufferEmpty) || !streamingData) {
		if (m_dma_start_sync_available && allChannelsPushed) {
			setSyncedStartDma(true);
		}
		setSyncedDma(false);
	}
}

void M2kFakeAnalogOutImpl::pushRawInterleaved(short *data, unsigned int nb_channels, unsigned int nb_samples)
{
	if ((nb_samples % nb_channels) !=0) {
		THROW_M2K_EXCEPTION("Analog Out: Input array length must be multiple of channels", libm2k::EXC_INVALID_PARAMETER);
	}
	std::vector<std::vector<short>> data_buffers;
	unsigned int bufferSize = nb_samples/nb_channels;
	bool streamingData = true;
	bool isBufferEmpty = true;
	bool allChannelsPushed = (nb_channels != getNbChannels()) ? false : true;

	for (unsigned int  chn = 0; chn < nb_channels; chn++) {
		streamingData &= !getCyclic(chn);
	}
	if (streamingData && m_dma_data_available) {
		// all kernel buffers are empty when maximum buffer space is equal with the unused space
		unsigned int unusedBufferSpace, maxBufferSpace;
		for (unsigned int  chn = 0; chn < nb_channels; chn++) {
			m_dac_devices.at(chn)->initializeBuffer(bufferSize, false);
			unusedBufferSpace = m_dac_devices[chn]->getBufferLongValue("data_available");
			maxBufferSpace = 2u * bufferSize * (m_nb_kernel_buffers.at(chn) - 1);
			isBufferEmpty &= (maxBufferSpace == unusedBufferSpace);
		}
		if (isBufferEmpty) {
			// when kernel buffers are empty both channels must be synchronized
			setSyncedDma(true);
		}
	} else {
		setSyncedDma(true);
	}

	for (unsigned int chn = 0; chn < nb_channels; chn++) {
		std::vector<short> raw_data_buffer = {};
		for (unsigned int i = 0, off = 0; i < (bufferSize); i++, off += nb_channels) {
			raw_data_buffer.push_back(data[chn + off]);
		}
		m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
	}

	if ((streamingData && isBufferEmpty) || !streamingData) {
		if (m_dma_start_sync_available && allChannelsPushed) {
			setSyncedStartDma(true);
		}
		setSyncedDma(false);
	}
}


/*
	 * push double (voltage) on multiple channels
	 */
void M2kFakeAnalogOutImpl::push(std::vector<std::vector<double>> const &data)
{
	std::vector<std::vector<short>> data_buffers;
	bool streamingData = true;
	bool isBufferEmpty = true;
	bool allChannelsPushed = (data.size() != getNbChannels()) ? false : true;

	for (unsigned int  chn = 0; chn < data.size(); chn++) {
		streamingData &= !getCyclic(chn);
		allChannelsPushed &= (data.at(chn).size() != 0);
	}

	if (streamingData && m_dma_data_available) {
		// all kernel buffers are empty when maximum buffer space is equal with the unused space
		unsigned int unusedBufferSpace, maxBufferSpace;
		for (unsigned int  chn = 0; chn < data.size(); chn++) {
			size_t size = data.at(chn).size();
			//m_dac_devices.at(chn)->initializeBuffer(size, false);
			unusedBufferSpace = 1;//m_dac_devices[chn]->getBufferLongValue("data_available");
			maxBufferSpace = 2u * size * (m_nb_kernel_buffers.at(chn) - 1);
			isBufferEmpty &= (maxBufferSpace == unusedBufferSpace);
		}
		if (isBufferEmpty) {
			// when kernel buffers are empty both channels must be synchronized
			setSyncedDma(true);
		}
	} else {
		setSyncedDma(true);
	}

	for (unsigned int chn = 0; chn < data.size(); chn++) {
		size_t size = data.at(chn).size();
		std::vector<short> raw_data_buffer = {};
		std::vector<double> actual_data = {};
		for (unsigned int i = 0; i < size; i++) {
			raw_data_buffer.push_back(convertVoltsToRaw(chn, data[chn][i]));
			actual_data.push_back(data[chn][i]);
		}
		//m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
		data_buffers.push_back(raw_data_buffer);
		data_buffers_raw.push_back(actual_data);
	}

	if ((streamingData && isBufferEmpty) || !streamingData) {
		if (m_dma_start_sync_available && allChannelsPushed) {
			setSyncedStartDma(true);
		}
		setSyncedDma(false);
	}
}

void M2kFakeAnalogOutImpl::pushInterleaved(double *data, unsigned int nb_channels, unsigned int nb_samples)
{
	if ((nb_samples % nb_channels) !=0) {
		THROW_M2K_EXCEPTION("Analog Out: Input array length must be multiple of channels", libm2k::EXC_INVALID_PARAMETER);
	}
	std::vector<std::vector<short>> data_buffers;
	unsigned int bufferSize = nb_samples/nb_channels;
	bool streamingData = true;
	bool isBufferEmpty = true;
	bool allChannelsPushed = (nb_channels != getNbChannels()) ? false : true;

	for (unsigned int  chn = 0; chn < nb_channels; chn++) {
		streamingData &= !getCyclic(chn);
	}
	if (streamingData && m_dma_data_available) {
		// all kernel buffers are empty when maximum buffer space is equal with the unused space
		unsigned int unusedBufferSpace, maxBufferSpace;
		for (unsigned int  chn = 0; chn < nb_channels; chn++) {
			m_dac_devices.at(chn)->initializeBuffer(bufferSize, false);
			unusedBufferSpace = m_dac_devices[chn]->getBufferLongValue("data_available");
			maxBufferSpace = 2u * bufferSize * (m_nb_kernel_buffers.at(chn) - 1);
			isBufferEmpty &= (maxBufferSpace == unusedBufferSpace);
		}
		if (isBufferEmpty) {
			// when kernel buffers are empty both channels must be synchronized
			setSyncedDma(true);
		}
	} else {
		setSyncedDma(true);
	}
	for (unsigned int chn = 0; chn < nb_channels; chn++) {
		std::vector<short> raw_data_buffer = {};
		for (unsigned int i = 0, off = 0; i < (bufferSize); i++, off += nb_channels) {
			raw_data_buffer.push_back(convertVoltsToRaw(chn, data[chn + off]));
		}
		m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
		data_buffers.push_back(raw_data_buffer);
	}

	if ((streamingData && isBufferEmpty) || !streamingData) {
		if (m_dma_start_sync_available && allChannelsPushed) {
			setSyncedStartDma(true);
		}
		setSyncedDma(false);
	}
}

double M2kFakeAnalogOutImpl::getScalingFactor(unsigned int chn)
{
	if (chn >= m_calib_vlsb.size()) {
		THROW_M2K_EXCEPTION("Analog Out: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	return (-1 * (1 / m_calib_vlsb.at(chn)) * 16) /
			getFilterCompensation(getSampleRate(chn));
}

double M2kFakeAnalogOutImpl::getFilterCompensation(double samplerate)
{
	return m_filter_compensation_table.at(samplerate);
}

void M2kFakeAnalogOutImpl::stop()
{
/*	m_m2k_fabric->setBoolValue(0, true, "powerdown", true);
	m_m2k_fabric->setBoolValue(1, true, "powerdown", true);*/
	setSyncedDma(true, 0);
	setSyncedDma(true, 1);

	for (DeviceOut* dev : m_dac_devices) {
		dev->stop();
	}
}

void M2kFakeAnalogOutImpl::stop(unsigned int chn)
{
	m_m2k_fabric->setBoolValue(chn, true, "powerdown", true);
	setSyncedDma(true, chn);
	getDacDevice(chn)->stop();
}

void M2kFakeAnalogOutImpl::enableChannel(unsigned int chnIdx, bool enable)
{
//	m_m2k_fabric->setBoolValue(chnIdx, false, "powerdown", true);
	setSyncedDma(false, chnIdx);
	getDacDevice(chnIdx)->enableChannel(0, enable, true);
}

bool M2kFakeAnalogOutImpl::isChannelEnabled(unsigned int chnIdx)
{
	return getDacDevice(chnIdx)->isChannelEnabled(0, true);
}

DeviceOut* M2kFakeAnalogOutImpl::getDacDevice(unsigned int chnIdx)
{
	if (chnIdx >= m_dac_devices.size()) {
		THROW_M2K_EXCEPTION("Analog Out: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	return m_dac_devices.at(chnIdx);
}

std::vector<double> M2kFakeAnalogOutImpl::getAvailableSampleRates(unsigned int chnIdx)
{
	std::vector<std::string> stringValues;
	std::vector<double> values;

	stringValues.push_back("75000000");//getDacDevice(chnIdx)->getAvailableAttributeValues("sampling_frequency");
	std::transform(stringValues.begin(), stringValues.end(), std::back_inserter(values),
		       [] (std::string &s) -> double { return std::stod(s); });
	return values;
}

void M2kFakeAnalogOutImpl::setKernelBuffersCount(unsigned int chnIdx, unsigned int count)
{
	if (chnIdx >= m_dac_devices.size()) {
		THROW_M2K_EXCEPTION("Analog Out: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}
//	m_dac_devices[chnIdx]->setKernelBuffersCount(count);
	m_nb_kernel_buffers[chnIdx] = count;
}

struct IIO_OBJECTS M2kFakeAnalogOutImpl::getIioObjects()
{
	auto dev_a_iio = getDacDevice(0)->getIioObjects();
	auto dev_b_iio = getDacDevice(1)->getIioObjects();
	dev_a_iio.buffers_tx.insert(dev_a_iio.buffers_tx.end(), dev_b_iio.buffers_tx.begin(), dev_b_iio.buffers_tx.end());
	dev_a_iio.channels_out.insert(dev_a_iio.channels_out.end(), dev_b_iio.channels_out.begin(), dev_b_iio.channels_out.end());
	dev_a_iio.devices.insert(dev_a_iio.devices.end(), dev_b_iio.devices.begin(), dev_b_iio.devices.end());
	return dev_a_iio;
}

void M2kFakeAnalogOutImpl::cancelBuffer()
{
	for (DeviceOut *dev : m_dac_devices) {
		dev->cancelBuffer();
	}
}

void M2kFakeAnalogOutImpl::cancelBuffer(unsigned int chn)
{
	getDacDevice(chn)->cancelBuffer();
}

unsigned int M2kFakeAnalogOutImpl::getNbChannels()
{
	return m_dac_devices.size();
}

string M2kFakeAnalogOutImpl::getChannelName(unsigned int channel)
{
	std::string name = "";
	auto chn = getDacDevice(channel)->getChannel(0, true);
	if (chn) {
		name = chn->getId();
	}
	return name;
}

double M2kFakeAnalogOutImpl::getMaximumSamplerate(unsigned int chn_idx)
{
	if (chn_idx >= m_dac_devices.size()) {
		THROW_M2K_EXCEPTION("Analog Out: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}
	if (m_max_samplerate[chn_idx] < 0) {
		auto values = getAvailableSampleRates(chn_idx);
		m_max_samplerate[chn_idx] = *(max_element(values.begin(), values.end()));
	}
	return m_max_samplerate[chn_idx];
}
