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

	const double* getSamplesInterleaved(unsigned int nb_samples);
	const short* getSamplesRawInterleaved(unsigned int nb_samples);

	double getSampleRate();
	double getSampleRate(unsigned int);
	double setSampleRate(double sampleRate);
	double setSampleRate(unsigned int chn_idx, double sampleRate);
	std::vector<double> getAvailableSampleRates();

	void enableChannel(unsigned int index, bool enable);
	void setKernelBuffersCount(unsigned int count);
	std::string getDeviceName();

	struct IIO_OBJECTS getIioObjects();

private:
	class GenericAnalogInImpl;
	std::unique_ptr<GenericAnalogInImpl> m_pimpl;
};
}
}

#endif // GENERICANALOGIN_HPP
