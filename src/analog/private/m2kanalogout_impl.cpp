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
		}

		if (sync) {
			syncDevice();
		}
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
		std::vector<double> values = {};
		for (unsigned int i = 0; i < samplerates.size(); i++) {
			double val = m_dac_devices.at(i)->setDoubleValue(samplerates.at(i),
									 "sampling_frequency");
			values.push_back(val);
		}
		return values;
	}

	double setSampleRate(unsigned int chn_idx, double samplerate)
	{
		return getDacDevice(chn_idx)->setDoubleValue(samplerate,
								 "sampling_frequency");
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

	int convertVoltsToRaw(double voltage, double vlsb,
					    double filterCompensation)
	{
		// TO DO: explain this formula....
		return voltage * ((-1 * (1 / vlsb) * 16) / filterCompensation);
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
		pushRaw(chnIdx, ptr, data.size());
	}

	void pushRaw(unsigned int chnIdx, short *data, unsigned int nb_samples)
	{
		if (chnIdx >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
		}
		m_m2k_fabric->setBoolValue(chnIdx, false, "powerdown", true);
		setSyncedDma(true, chnIdx);

		m_samplerate.at(0) = getSampleRate(0);
		m_samplerate.at(1) = getSampleRate(1);

		std::vector<short> raw_data_buffer(data, data + nb_samples);

		m_dac_devices.at(chnIdx)->push(raw_data_buffer, 0, getCyclic(chnIdx));

		setSyncedDma(false, chnIdx);
	}


	/*
	 * push double (voltage) on one single channel
	 */
	void push(std::vector<double> const &data, unsigned int chnIdx)
	{
		double *ptr = (double*)data.data();
		push(chnIdx, ptr, data.size());
	}

	void push(unsigned int chnIdx, double *data, unsigned int nb_samples)
	{
		if (chnIdx >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
		}
		m_m2k_fabric->setBoolValue(chnIdx, false, "powerdown", true);
		setSyncedDma(true, chnIdx);

		std::vector<short> raw_data_buffer = {};

		m_samplerate.at(0) = getSampleRate(0);
		m_samplerate.at(1) = getSampleRate(1);

		for (unsigned int i = 0; i < nb_samples; i++) {
			raw_data_buffer.push_back(processSample(data[i], chnIdx));
		}
		m_dac_devices.at(chnIdx)->push(raw_data_buffer, 0, getCyclic(chnIdx));

		setSyncedDma(false, chnIdx);
	}


	/*
	 * push short (raw) on multiple channels
	 */
	void pushRaw(std::vector<std::vector<short>> const &data)
	{
		std::vector<std::vector<short>> data_buffers;
		m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, false, "powerdown", true);
		setSyncedDma(true);

		m_samplerate.at(0) = getSampleRate(0);
		m_samplerate.at(1) = getSampleRate(1);

		for (unsigned int chn = 0; chn < data.size(); chn++) {
			size_t size = data.at(chn).size();
			std::vector<short> raw_data_buffer(data.at(chn).data(), data.at(chn).data() + size);
			m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
		}

		setSyncedDma(false);
	}

	void pushRaw(short *data, unsigned int nb_channels, unsigned int nb_samples)
	{
		if ((nb_samples % nb_channels) !=0) {
                        throw_exception(EXC_INVALID_PARAMETER, "Analog Out: Input array length must be multiple of channels");
                }
		std::vector<std::vector<short>> data_buffers;
		m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, false, "powerdown", true);
		setSyncedDma(true);

		m_samplerate.at(0) = getSampleRate(0);
		m_samplerate.at(1) = getSampleRate(1);

		for (unsigned int chn = 0; chn < nb_channels; chn++) {
			std::vector<short> raw_data_buffer = {};
			for (unsigned int i = 0, off = 0; i < (nb_samples/nb_channels); i++, off += nb_channels) {
				raw_data_buffer.push_back(data[chn + off]);
			}
			m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
		}

		setSyncedDma(false);
	}


	/*
	 * push double (voltage) on multiple channels
	 */
	void push(std::vector<std::vector<double>> const &data)
	{
		std::vector<std::vector<short>> data_buffers;
		m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, false, "powerdown", true);
		setSyncedDma(true);

		m_samplerate.at(0) = getSampleRate(0);
		m_samplerate.at(1) = getSampleRate(1);

		for (unsigned int chn = 0; chn < data.size(); chn++) {
			size_t size = data.at(chn).size();
			std::vector<short> raw_data_buffer = {};
			for (unsigned int i = 0; i < size; i++) {
				raw_data_buffer.push_back(processSample(data[chn][i], chn));
			}
			m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
			data_buffers.push_back(raw_data_buffer);
		}

		setSyncedDma(false);
	}

	void push(double *data, unsigned int nb_channels, unsigned int nb_samples)
	{
		if ((nb_samples % nb_channels) !=0) {
                        throw_exception(EXC_INVALID_PARAMETER, "Analog Out: Input array length must be multiple of channels");
                }
		std::vector<std::vector<short>> data_buffers;
		m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, false, "powerdown", true);
		setSyncedDma(true);

		m_samplerate.at(0) = getSampleRate(0);
		m_samplerate.at(1) = getSampleRate(1);

		for (unsigned int chn = 0; chn < nb_channels; chn++) {
			std::vector<short> raw_data_buffer = {};
			for (unsigned int i = 0, off = 0; i < (nb_samples/nb_channels); i++, off += nb_channels) {
				raw_data_buffer.push_back(processSample(data[chn + off], chn));
			}
			m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
			data_buffers.push_back(raw_data_buffer);
		}

		setSyncedDma(false);
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

		for (DeviceOut* dev : m_dac_devices) {
			dev->stop();
		}
	}

	void stop(unsigned int chn)
	{
		m_m2k_fabric->setBoolValue(chn, true, "powerdown", true);
		getDacDevice(chn)->stop();
	}

	short processSample(double value, unsigned int channel)
	{
		return convertVoltsToRaw(value, m_calib_vlsb.at(channel),
					 getFilterCompensation(
						 m_samplerate.at(channel)));
	}

	void enableChannel(unsigned int chnIdx, bool enable)
	{
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

private:
	std::shared_ptr<DeviceGeneric> m_m2k_fabric;
	std::vector<double> m_calib_vlsb;
	std::vector<bool> m_cyclic;
	std::vector<double> m_samplerate;

	std::map<double, double> m_filter_compensation_table;
	std::vector<DeviceOut*> m_dac_devices;
};
