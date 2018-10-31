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

#include "libm2k/m2kglobal.hpp"
#include "libm2k/genericanalogout.hpp"
#include <vector>
#include <map>

extern "C" {
	struct iio_context;
	struct iio_device;
	struct iio_channel;
}

namespace libm2k {
namespace analog {
class LIBM2K_API M2kAnalogOut : public GenericAnalogOut
{
public:
	M2kAnalogOut(struct iio_context*, std::string dac_dev);
	~M2kAnalogOut();

	void openAnalogOut();
	void closeAnalogOut();

	double getOversamplingRatio();
	double getOversamplingRatio(unsigned int);
	double setOversamplingRatio(double sampleRate);
	double setOversamplingRatio(unsigned int chn_idx, double sampleRate);

	struct iio_channel* getChannel();
	struct iio_channel* getAuxChannel(unsigned int chn_idx);
	struct iio_channel* getFabricChannel(unsigned int chn_idx);

	double getScalingFactor();
	double getFilterCompensation(double samplerate);

	int convertVoltsToRaw(double voltage, double vlsb,
				double filterCompensation);

	void setDacCalibVlsb(double vlsb);

	void push(std::vector<double>& data, bool cyclic = true,
		  unsigned int chn_idx = 0);
	void push(std::vector<short>& data, bool cyclic = true,
		  unsigned int chn_idx = 0);
	void stopOutput();
	short processSample(double value, bool raw);

	void setupBeforeBuffer();
	void setupAfterBuffer();

private:
	struct iio_device* m_ad5625;
	struct iio_device* m_m2k_fabric;
	std::vector<struct iio_channel*> m_m2k_fabric_channels;
	std::vector<struct iio_channel*> m_ad5625_channels;
	double m_dac_calib_vlsb;

	std::map<double, double> m_filter_compensation_table;
};
}
}

#endif //M2KANALOGOUT_HPP
