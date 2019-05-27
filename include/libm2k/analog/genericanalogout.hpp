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
#include <string>
#include <vector>
#include <memory>

extern "C" {
	struct iio_context;
}

namespace libm2k {
namespace analog {
class LIBM2K_API GenericAnalogOut {
public:
	GenericAnalogOut(struct iio_context* ctx, std::string dac_dev);
	virtual ~GenericAnalogOut();

	double getSampleRate();
	double getSampleRate(unsigned int);
	double setSampleRate(double sampleRate);
	double setSampleRate(unsigned int chn_idx, double sampleRate);
	std::vector<double> getAvailableSampleRates();

	void setCyclic(bool en);
	void setCyclic(unsigned int chn, bool en);
	bool getCyclic(unsigned int chn);

	void push(std::vector<double> const &data, unsigned int chn_idx = 0);
	void push(std::vector<short> const &data, unsigned int chn_idx = 0);
	void stop();

	std::string getName();
	void enableChannel(unsigned int chnIdx, bool enable);

private:
	class GenericAnalogOutImpl;
	std::unique_ptr<GenericAnalogOutImpl> m_pimpl;
};
}
}


#endif //GENERICANALOGOUT_HPP
