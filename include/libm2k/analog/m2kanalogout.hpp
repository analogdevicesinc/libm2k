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

#ifndef M2KANALOGOUT_HPP
#define M2KANALOGOUT_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/analog/genericanalogout.hpp>
#include <libm2k/utils/device.hpp>
#include <vector>
#include <memory>
#include <map>

using namespace libm2k::utils;
namespace libm2k {
namespace analog {
class LIBM2K_API M2kAnalogOut : public Device
{
public:
	M2kAnalogOut(struct iio_context*, std::vector<std::string> dac_devs);
	~M2kAnalogOut();

	void openAnalogOut();
	void closeAnalogOut();

	/* chn can be 0 or 1, it actually refers to dac-a or dac-b) */
	std::vector<double> getOversamplingRatio();
	double getOversamplingRatio(unsigned int chn);
	std::vector<double> setOversamplingRatio(std::vector<double> oversampling_ratio);
	double setOversamplingRatio(unsigned int chn, double oversampling_ratio);

	std::vector<double> getSamplerate();
	double getSamplerate(unsigned int chn);
	std::vector<double> setSamplerate(std::vector<double> samplerates);
	double setSamplerate(unsigned int chn, double samplerate);

	void setSyncedDma(bool en, int chn = -1);
	bool getSyncedDma(int chn = -1);

	void setCyclic(bool en);
	void setCyclic(bool en, unsigned int chn);
	bool getCyclic(unsigned int chn);

	double getScalingFactor(unsigned int chn);
	double getFilterCompensation(double samplerate);

	int convertVoltsToRaw(double voltage, double vlsb,
				double filterCompensation);

	void setDacCalibVlsb(unsigned int chn, double vlsb);

	void push(std::vector<double> &data, unsigned int chnIdx);
	void push(std::vector<short> &data, unsigned int chnIdx);
	void push(std::vector<std::vector<double>>& data);
	void push(std::vector<std::vector<short>>& data);
	void stop();
	void stop(unsigned int chn);

	void enableChannel(unsigned int chnIdx, bool enable);
private:
	short processSample(double value, unsigned int channel, bool raw);
	std::shared_ptr<Device> m_m2k_fabric;
	std::vector<double> m_calib_vlsb;
	std::vector<bool> m_cyclic;
	bool m_sync_start;

	std::map<double, double> m_filter_compensation_table;
	std::vector<Device*> m_dac_devices;
};
}
}

#endif //M2KANALOGOUT_HPP
