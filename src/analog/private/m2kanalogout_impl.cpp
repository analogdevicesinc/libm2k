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
#include <libm2k/utils/devicegeneric.hpp>
#include <libm2k/utils/deviceout.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>

#include <iostream>
#include <thread>
#include <chrono>
#include <iio.h>
#include <string.h>

using namespace libm2k::analog;
using namespace libm2k::utils;
using namespace std;

class M2kAnalogOut::M2kAnalogOutImpl : public DeviceGeneric {
public:
	M2kAnalogOutImpl(iio_context *ctx, std::vector<std::string> dac_devs, bool sync):
		DeviceGeneric(ctx, "")
	{
		m_dac_devices.push_back(new DeviceOut(ctx, dac_devs.at(0)));
		m_dac_devices.push_back(new DeviceOut(ctx, dac_devs.at(1)));

		m_m2k_fabric = make_shared<DeviceGeneric>(ctx, "m2k-fabric");
		if (!m_m2k_fabric) {
			throw_exception(EXC_INVALID_PARAMETER, "Analog out: Cannot find m2k fabric device");
		}

		m_calib_vlsb.push_back(10.0 / ((double)( 1 << 12 )));
		m_calib_vlsb.push_back(10.0 / ((double)( 1 << 12 )));

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
		}

		if (sync) {
			syncDevice();
		}
		// dma_start_sync attribute is only available in firmware versions newer than 0.24
		m_dma_start_sync_available = (Utils::compareVersions(Utils::getFirmwareVersion(ctx), "v0.24") > 0);
		// data_available attribute exists only in firmware versions newer than 0.23
		m_dma_data_available = (Utils::compareVersions(Utils::getFirmwareVersion(ctx), "v0.23") > 0);
	}

	~M2kAnalogOutImpl()
	{
		stop();
		for (auto d : m_dac_devices) {
			delete d;
		}
		m_dac_devices.clear();
	}

	void init()
	{
		setOversamplingRatio(0, 1);
		setOversamplingRatio(1, 1);
		setSampleRate(0, 75E6);
		setSampleRate(1, 75E6);
		m_calib_vlsb.at(0) = 10.0 / ((double)( 1 << 12 ));
		m_calib_vlsb.at(1) = 10.0 / ((double)( 1 << 12 ));
		m_cyclic.at(0) = true;
		m_cyclic.at(1) = true;
		enableChannel(0, true);
		enableChannel(1, true);
		m_samplerate.at(0) = 75E6;
		m_samplerate.at(1) = 75E6;
	}

	void syncDevice()
	{
		m_samplerate.at(0) = getSampleRate(0);
		m_samplerate.at(1) = getSampleRate(1);
		//enable???
	}

	double getCalibscale(unsigned int index)
	{
		return getDacDevice(index)->getDoubleValue("calibscale");
	}

	double setCalibscale(unsigned int index, double calibscale)
	{
		getDacDevice(index)->setDoubleValue(calibscale, "calibscale");
		return getCalibscale(index);
	}

	std::vector<int> getOversamplingRatio()
	{
		std::vector<int> values = {};
		for (unsigned int i = 0; i < m_dac_devices.size(); i++) {
			int val = m_dac_devices.at(i)->getLongValue("oversampling_ratio");
			values.push_back(val);
		}
		return values;
	}

	int getOversamplingRatio(unsigned int chn_idx)
	{
		return getDacDevice(chn_idx)->getLongValue("oversampling_ratio");
	}

	std::vector<int> setOversamplingRatio(std::vector<int> oversampling_ratio)
	{
		std::vector<int> values = {};
		for (unsigned int i = 0; i < oversampling_ratio.size(); i++) {
			int val = m_dac_devices.at(i)->setLongValue(oversampling_ratio.at(i),
									 "oversampling_ratio");
			values.push_back(val);
		}
		return values;
	}

	int setOversamplingRatio(unsigned int chn_idx, int oversampling_ratio)
	{
		return getDacDevice(chn_idx)->setLongValue(oversampling_ratio,
								 "oversampling_ratio");
	}

	std::vector<double> getSampleRate()
	{
		std::vector<double> values = {};
		for (unsigned int i = 0; i < m_dac_devices.size(); i++) {
			double val = m_dac_devices.at(i)->getDoubleValue("sampling_frequency");
			values.push_back(val);
		}
		return values;
	}

	double getSampleRate(unsigned int chn_idx)
	{
		return getDacDevice(chn_idx)->getDoubleValue("sampling_frequency");
	}

	std::vector<double> setSampleRate(std::vector<double> samplerates)
	{
		if (samplerates.size() >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
		}
		std::vector<double> values = {};
		for (unsigned int i = 0; i < samplerates.size(); i++) {
			m_samplerate.at(i) = m_dac_devices.at(i)->setDoubleValue(samplerates.at(i),
									 "sampling_frequency");
			values.push_back(m_samplerate.at(i));
		}
		return values;
	}

	double setSampleRate(unsigned int chn_idx, double samplerate)
	{
		if (chn_idx >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
		}
		m_samplerate.at(chn_idx) = getDacDevice(chn_idx)->setDoubleValue(samplerate, "sampling_frequency");
		return m_samplerate.at(chn_idx);
	}

	void setSyncedDma(bool en, int chn = -1)
	{
		if (chn < 0) {
			for (auto dac : m_dac_devices) {
				dac->setBoolValue(en, "dma_sync");
			}
		} else {
			getDacDevice(chn)->setBoolValue(en, "dma_sync");
		}
	}

	bool getSyncedDma(int chn = -1)
	{
		return getDacDevice(chn)->getBoolValue("dma_sync");
	}

	void setSyncedStartDma(bool en, int chn = -1)
	{
		if (!m_dma_start_sync_available) {
			throw_exception(EXC_RUNTIME_ERROR, "Invalid firmware version: 0.24 or greater is required.");
		}
		if (chn < 0) {
			for (auto dac : m_dac_devices) {
				dac->setBoolValue(en, "dma_sync_start");
			}
		} else {
			getDacDevice(chn)->setBoolValue(en, "dma_sync_start");
		}
	}

	bool getSyncedStartDma(int chn = -1)
	{
		if (!m_dma_start_sync_available) {
			throw_exception(EXC_RUNTIME_ERROR, "Invalid firmware version: 0.24 or greater is required.");
		}
		return getDacDevice(chn)->getBoolValue("dma_sync_start");
	}

	void setCyclic(bool en)
	{
		for (unsigned int i = 0; i < m_dac_devices.size(); i++) {
			m_cyclic.at(i) = en;
			m_dac_devices.at(i)->setCyclic(en);
		}
	}

	void setCyclic(unsigned int chn, bool en)
	{
		getDacDevice(chn)->setCyclic(en);
		m_cyclic.at(chn) = en;
	}

	bool getCyclic(unsigned int chn)
	{
		if (chn >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
		}
		return m_cyclic.at(chn);
	}

	short convVoltsToRaw(double voltage, double vlsb,
					    double filterCompensation)
	{
		// TO DO: explain this formula....
		return voltage * ((-1 * (1 / vlsb) * 16) / filterCompensation);
	}

	short convertVoltsToRaw(unsigned int channel, double voltage)
	{
		if (channel >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
		}

		return convVoltsToRaw(voltage, m_calib_vlsb.at(channel),
					 getFilterCompensation(
					 m_samplerate.at(channel)));
	}

	double convRawToVolts(short raw, double vlsb, double filterCompensation)
	{
		return -((raw * filterCompensation * vlsb) / 16);
	}

	double convertRawToVolts(unsigned int channel, short raw)
	{
		if (channel >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
		}

		return convRawToVolts(raw, m_calib_vlsb.at(channel),
					 getFilterCompensation(
					 m_samplerate.at(channel)));
	}

	void setDacCalibVlsb(unsigned int chn_idx, double vlsb)
	{
		m_calib_vlsb[chn_idx] = vlsb;
	}


	/*
	 * push short (raw) on one single channel
	 */
	void pushRaw(std::vector<short> const &data, unsigned int chnIdx)
	{
		short *ptr = (short*)data.data();
		pushRawBytes(chnIdx, ptr, data.size());
	}

	void pushRawBytes(unsigned int chnIdx, short *data, unsigned int nb_samples)
	{
		if (chnIdx >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
		}

		m_dac_devices.at(chnIdx)->push(data, 0, nb_samples, getCyclic(chnIdx));

		setSyncedDma(false, chnIdx);
	}


	/*
	 * push double (voltage) on one single channel
	 */
	void push(std::vector<double> const &data, unsigned int chnIdx)
	{
		double *ptr = (double*)data.data();
		pushBytes(chnIdx, ptr, data.size());
	}

	void pushBytes(unsigned int chnIdx, double *data, unsigned int nb_samples)
	{
		if (chnIdx >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
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
	void pushRaw(std::vector<std::vector<short>> const &data)
	{
		std::vector<std::vector<short>> data_buffers;
		bool streamingData = true;
		bool isBufferEmpty = true;

		for (unsigned int  chn = 0; chn < data.size(); chn++) {
			streamingData &= !getCyclic(chn);
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
			if (m_dma_start_sync_available) {
				setSyncedStartDma(true);
			}
			setSyncedDma(false);
		}
	}

	void pushRawInterleaved(short *data, unsigned int nb_channels, unsigned int nb_samples)
	{
		if ((nb_samples % nb_channels) !=0) {
                        throw_exception(EXC_INVALID_PARAMETER, "Analog Out: Input array length must be multiple of channels");
                }
		std::vector<std::vector<short>> data_buffers;
		unsigned int bufferSize = nb_samples/nb_channels;
		bool streamingData = true;
		bool isBufferEmpty = true;

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
			if (m_dma_start_sync_available) {
				setSyncedStartDma(true);
			}
			setSyncedDma(false);
		}
	}


	/*
	 * push double (voltage) on multiple channels
	 */
	void push(std::vector<std::vector<double>> const &data)
	{
		std::vector<std::vector<short>> data_buffers;
		bool streamingData = true;
		bool isBufferEmpty = true;

		for (unsigned int  chn = 0; chn < data.size(); chn++) {
			streamingData &= !getCyclic(chn);
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
			std::vector<short> raw_data_buffer = {};
			for (unsigned int i = 0; i < size; i++) {
				raw_data_buffer.push_back(convertVoltsToRaw(chn, data[chn][i]));
			}
			m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
			data_buffers.push_back(raw_data_buffer);
		}

		if ((streamingData && isBufferEmpty) || !streamingData) {
			if (m_dma_start_sync_available) {
				setSyncedStartDma(true);
			}
			setSyncedDma(false);
		}
	}

	void pushInterleaved(double *data, unsigned int nb_channels, unsigned int nb_samples)
	{
		if ((nb_samples % nb_channels) !=0) {
                        throw_exception(EXC_INVALID_PARAMETER, "Analog Out: Input array length must be multiple of channels");
                }
		std::vector<std::vector<short>> data_buffers;
		unsigned int bufferSize = nb_samples/nb_channels;
		bool streamingData = true;
		bool isBufferEmpty = true;

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
			if (m_dma_start_sync_available) {
				setSyncedStartDma(true);
			}
			setSyncedDma(false);
		}
	}

	double getScalingFactor(unsigned int chn)
	{
		if (chn >= m_calib_vlsb.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "No such channel");
		}
		return (-1 * (1 / m_calib_vlsb.at(chn)) * 16) /
				getFilterCompensation(getSampleRate(chn));
	}

	double getFilterCompensation(double samplerate)
	{
		return m_filter_compensation_table.at(samplerate);
	}

	void stop()
	{
		m_m2k_fabric->setBoolValue(0, true, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, true, "powerdown", true);
		setSyncedDma(true, 0);
		setSyncedDma(true, 1);

		for (DeviceOut* dev : m_dac_devices) {
			dev->stop();
		}
	}

	void stop(unsigned int chn)
	{
		m_m2k_fabric->setBoolValue(chn, true, "powerdown", true);
		setSyncedDma(true, chn);
		getDacDevice(chn)->stop();
	}

	void enableChannel(unsigned int chnIdx, bool enable)
	{
		m_m2k_fabric->setBoolValue(chnIdx, false, "powerdown", true);
		setSyncedDma(false, chnIdx);
		getDacDevice(chnIdx)->enableChannel(0, enable, true);
	}

	bool isChannelEnabled(unsigned int chnIdx)
	{
		return getDacDevice(chnIdx)->isChannelEnabled(0, true);
	}

	DeviceOut* getDacDevice(unsigned int chnIdx)
	{
		if (chnIdx >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "M2kAnalogOut: No such channel");
		}
		return m_dac_devices.at(chnIdx);
	}

	std::vector<double> getAvailableSampleRates(unsigned int chnIdx)
	{
		return getDacDevice(chnIdx)->getAvailableSampleRates();
	}

	void setKernelBuffersCount(unsigned int chnIdx, unsigned int count)
	{
		if (chnIdx >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "M2kAnalogOut: No such channel");
		}
		m_dac_devices[chnIdx]->setKernelBuffersCount(count);
		m_nb_kernel_buffers[chnIdx] = count;
	}

	struct IIO_OBJECTS getIioObjects()
	{
		auto dev_a_iio = getDacDevice(0)->getIioObjects();
		auto dev_b_iio = getDacDevice(1)->getIioObjects();
		dev_a_iio.buffers_tx.insert(dev_a_iio.buffers_tx.end(), dev_b_iio.buffers_tx.begin(), dev_b_iio.buffers_tx.end());
		dev_a_iio.channels_out.insert(dev_a_iio.channels_out.end(), dev_b_iio.channels_out.begin(), dev_b_iio.channels_out.end());
		dev_a_iio.devices.insert(dev_a_iio.devices.end(), dev_b_iio.devices.begin(), dev_b_iio.devices.end());
		return dev_a_iio;
	}

private:
	std::shared_ptr<DeviceGeneric> m_m2k_fabric;
	std::vector<double> m_calib_vlsb;
	std::vector<bool> m_cyclic;
	std::vector<double> m_samplerate;

	std::map<double, double> m_filter_compensation_table;
	std::vector<DeviceOut*> m_dac_devices;

	bool m_dma_start_sync_available;
	bool m_dma_data_available;
	std::vector<unsigned int> m_nb_kernel_buffers;
};
