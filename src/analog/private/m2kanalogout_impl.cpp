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
	M2kAnalogOutImpl(iio_context *ctx, std::vector<std::string> dac_devs):
		DeviceGeneric(ctx, "")
	{
		m_dac_devices.push_back(new DeviceOut(ctx, dac_devs.at(0)));
		m_dac_devices.push_back(new DeviceOut(ctx, dac_devs.at(1)));

		m_m2k_fabric = make_shared<DeviceGeneric>(ctx, "m2k-fabric");
		if (!m_m2k_fabric) {
			throw_exception(EXC_INVALID_PARAMETER, "Analog out: Can not find m2k fabric device");
		}
		m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, false, "powerdown", true);

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

	void openAnalogOut()
	{

	}

	void closeAnalogOut()
	{

	}

	std::vector<double> getOversamplingRatio()
	{
		std::vector<double> values = {};
		for (unsigned int i = 0; i < m_dac_devices.size(); i++) {
			double val = m_dac_devices.at(i)->getDoubleValue("oversampling_ratio");
			values.push_back(val);
		}
		return values;
	}

	double getOversamplingRatio(unsigned int chn_idx)
	{
		return m_dac_devices.at(chn_idx)->getDoubleValue("oversampling_ratio");
	}

	std::vector<double> setOversamplingRatio(std::vector<double> oversampling_ratio)
	{
		std::vector<double> values = {};
		for (unsigned int i = 0; i < oversampling_ratio.size(); i++) {
			double val = m_dac_devices.at(i)->setDoubleValue(oversampling_ratio.at(i),
									 "oversampling_ratio");
			values.push_back(val);
		}
		return values;
	}

	double setOversamplingRatio(unsigned int chn_idx, double oversampling_ratio)
	{
		return m_dac_devices.at(chn_idx)->setDoubleValue(oversampling_ratio,
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
		return m_dac_devices.at(chn_idx)->getDoubleValue("sampling_frequency");
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
		return m_dac_devices.at(chn_idx)->setDoubleValue(samplerate,
								 "sampling_frequency");
	}

	void setSyncedDma(bool en, int chn = -1)
	{
		if (chn < 0) {
			for (auto dac : m_dac_devices) {
				dac->setBoolValue(en, "dma_sync");
			}
		} else {
			if (chn >= m_dac_devices.size()) {
				throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
			}
			m_dac_devices.at(chn)->setBoolValue(en, "dma_sync");
		}
	}

	bool getSyncedDma(int chn = -1)
	{
		if (chn >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
		}

		return m_dac_devices.at(chn)->getBoolValue("dma_sync");
	}

	void setCyclic(bool en)
	{
		for (unsigned int i = 0; i < m_dac_devices.size(); i++) {
			m_cyclic.at(i) = en;
		}
	}

	void setCyclic(unsigned int chn, bool en)
	{
		if (chn >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
		}
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

	void push(std::vector<short> const &data, unsigned int chnIdx)
	{
		if (chnIdx >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
		}
		m_m2k_fabric->setBoolValue(chnIdx, true, "powerdown", true);
		setSyncedDma(true, chnIdx);

		size_t size = data.size();
		std::vector<short> raw_data_buffer = {};
		for (unsigned int i = 0; i < size; i++) {
			raw_data_buffer.push_back(processSample(data.at(i), chnIdx, true));
		}
		m_dac_devices.at(chnIdx)->push(raw_data_buffer, 0, getCyclic(chnIdx));

		setSyncedDma(false, chnIdx);
		m_m2k_fabric->setBoolValue(chnIdx, false, "powerdown", true);
	}

	void push(std::vector<double> const &data, unsigned int chnIdx)
	{
		if (chnIdx >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out: No such channel");
		}
		m_m2k_fabric->setBoolValue(chnIdx, true, "powerdown", true);
		setSyncedDma(true, chnIdx);

		size_t size = data.size();
		std::vector<short> raw_data_buffer = {};
		for (unsigned int i = 0; i < size; i++) {
			raw_data_buffer.push_back(processSample(data.at(i), chnIdx, false));
		}
		m_dac_devices.at(chnIdx)->push(raw_data_buffer, 0, getCyclic(chnIdx));

		setSyncedDma(false, chnIdx);
		m_m2k_fabric->setBoolValue(chnIdx, false, "powerdown", true);
	}

	void push(std::vector<std::vector<short>> const &data)
	{
		std::vector<std::vector<short>> data_buffers;
		m_m2k_fabric->setBoolValue(0, true, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, true, "powerdown", true);
		setSyncedDma(true);

		for (unsigned int chn = 0; chn < data.size(); chn++) {
			size_t size = data.at(chn).size();
			std::vector<short> raw_data_buffer = {};
			for (unsigned int i = 0; i < size; i++) {
				raw_data_buffer.push_back(processSample(data.at(chn).at(i), chn, true));
			}
			m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
		}

		setSyncedDma(false);
		m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, false, "powerdown", true);
	}

	void push(std::vector<std::vector<double>> const &data)
	{
		std::vector<std::vector<short>> data_buffers;
		m_m2k_fabric->setBoolValue(0, true, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, true, "powerdown", true);
		setSyncedDma(true);

		for (unsigned int chn = 0; chn < data.size(); chn++) {
			size_t size = data.at(chn).size();
			std::vector<short> raw_data_buffer = {};
			for (unsigned int i = 0; i < size; i++) {
				raw_data_buffer.push_back(processSample(data.at(chn).at(i), chn, false));
			}
			m_dac_devices.at(chn)->push(raw_data_buffer, 0, getCyclic(chn));
			data_buffers.push_back(raw_data_buffer);
		}

		setSyncedDma(false);
		m_m2k_fabric->setBoolValue(0, false, "powerdown", true);
		m_m2k_fabric->setBoolValue(1, false, "powerdown", true);
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
		if (chn >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Analog Out stop: No such channel");
		}
		m_m2k_fabric->setBoolValue(chn, true, "powerdown", true);
		m_dac_devices.at(chn)->stop();
	}

	short processSample(double value, unsigned int channel, bool raw)
	{
		if (raw) {
			short raw_value = value;
			raw_value = (-raw_value) << 4;

			// This should go away once channel type gets
			// changed from 'le:S16/16>>0' to 'le:S12/16>>4'
			return raw_value;
		} else {
			return convertVoltsToRaw(value, m_calib_vlsb.at(channel),
						 getFilterCompensation(
							 getSampleRate(channel)));
		}
	}

	void enableChannel(unsigned int chnIdx, bool enable)
	{
		if (chnIdx >= m_dac_devices.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "M2kAnalogOut: No such channel");
		}

		m_dac_devices.at(chnIdx)->enableChannel(0, enable);
	}

private:
	std::shared_ptr<DeviceGeneric> m_m2k_fabric;
	std::vector<double> m_calib_vlsb;
	std::vector<bool> m_cyclic;

	std::map<double, double> m_filter_compensation_table;
	std::vector<DeviceOut*> m_dac_devices;
};
