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
#include <vector>
#include <memory>
#include <map>

extern "C" {
	struct iio_context;
}

namespace libm2k {
namespace analog {
class LIBM2K_API M2kAnalogOut
{
public:
	M2kAnalogOut(struct iio_context*, std::vector<std::string> dac_devs, bool sync);
	virtual ~M2kAnalogOut();

	void init();

	/* chn can be 0 or 1, it actually refers to dac-a or dac-b) */
	std::vector<double> getOversamplingRatio();
	double getOversamplingRatio(unsigned int chn);
	std::vector<double> setOversamplingRatio(std::vector<double> oversampling_ratio);
	double setOversamplingRatio(unsigned int chn, double oversampling_ratio);

	std::vector<double> getSampleRate();
	double getSampleRate(unsigned int chn);
	std::vector<double> setSampleRate(std::vector<double> samplerates);
	double setSampleRate(unsigned int chn, double samplerate);

	void setSyncedDma(bool en, int chn = -1);
	bool getSyncedDma(int chn = -1);

	void setCyclic(bool en);
	void setCyclic(unsigned int chn, bool en);
	bool getCyclic(unsigned int chn);

	double setCalibscale(unsigned int index, double calibscale);
	double getCalibscale(unsigned int index);

	double getScalingFactor(unsigned int chn);
	double getFilterCompensation(double samplerate);

	int convertVoltsToRaw(double voltage, double vlsb,
				double filterCompensation);

	void setDacCalibVlsb(unsigned int chn, double vlsb);

	void push(unsigned int chnIdx, std::vector<double> const &data);
	void push(unsigned int chnIdx, std::vector<short> const &data);
	void push(std::vector<std::vector<double>> const &data);
	void push(std::vector<std::vector<short>> const &data);
	void stop();
	void stop(unsigned int chn);

	void enableChannel(unsigned int chnIdx, bool enable);
	bool isChannelEnabled(unsigned int chnIdx);
private:
	class M2kAnalogOutImpl;
	std::unique_ptr<M2kAnalogOutImpl> m_pimpl;
};
}
}

#endif //M2KANALOGOUT_HPP
