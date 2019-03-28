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

#ifndef GENERICANALOGOUT_HPP
#define GENERICANALOGOUT_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/utils/device.hpp>
#include <string>
#include <vector>

using namespace libm2k::utils;
namespace libm2k {
namespace analog {
class LIBM2K_API GenericAnalogOut : public Device {
public:
	GenericAnalogOut(struct iio_context* ctx, std::string dac_dev);
	virtual ~GenericAnalogOut();

	void openAnalogOut();
	void closeAnalogOut();

	double getSamplerate();
	double getSamplerate(unsigned int);
	double setSamplerate(double sampleRate);
	double setSamplerate(unsigned int chn_idx, double sampleRate);
	std::vector<double> getAvailableSamplerates();

	bool isChannelEnabled(unsigned int index);

	void setCyclic(bool en);
	void setCyclic(bool en, unsigned int chn);
	bool getCyclic(unsigned int chn);

	void push(std::vector<double>& data, unsigned int chn_idx = 0);
	void push(std::vector<short>& data, unsigned int chn_idx = 0);
	void stop();

private:
	std::vector<bool> m_cyclic;
};
}
}


#endif //GENERICANALOGOUT_HPP
