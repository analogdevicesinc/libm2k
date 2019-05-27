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

#ifndef GENERICANALOGIN_HPP
#define GENERICANALOGIN_HPP

#include <libm2k/m2kglobal.hpp>
#include <string>
#include <vector>
#include <memory>

namespace libm2k {
namespace analog {
class LIBM2K_API GenericAnalogIn {
public:
	GenericAnalogIn(struct iio_context* ctx, std::string adc_dev);
	virtual ~GenericAnalogIn();

	virtual std::vector<std::vector<double>> getSamples(unsigned int nb_samples);
	virtual std::vector<std::vector<double>> getSamplesRaw(unsigned int nb_samples);

	double getSampleRate();
	double getSampleRate(unsigned int);
	double setSampleRate(double sampleRate);
	double setSampleRate(unsigned int chn_idx, double sampleRate);
	std::vector<double> getAvailableSampleRates();

	void enableChannel(unsigned int index, bool enable);
	std::string getDeviceName();

private:
	class GenericAnalogInImpl;
	std::unique_ptr<GenericAnalogInImpl> m_pimpl;
};
}
}

#endif // GENERICANALOGIN_HPP
