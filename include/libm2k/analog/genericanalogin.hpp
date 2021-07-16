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
	virtual ~GenericAnalogIn() {}

	virtual const double* getSamplesInterleaved(unsigned int nb_samples) = 0;
	virtual const short* getSamplesRawInterleaved(unsigned int nb_samples) = 0;

	virtual double getSampleRate() = 0;
	virtual double getSampleRate(unsigned int) = 0;
	virtual double setSampleRate(double sampleRate) = 0;
	virtual double setSampleRate(unsigned int chn_idx, double sampleRate) = 0;
	virtual std::vector<double> getAvailableSampleRates() = 0;
	virtual std::vector<double> getAvailableSampleRates(unsigned int chn_idx) = 0;
	virtual double getMaximumSamplerate() = 0;
	virtual double getMaximumSamplerate(unsigned int chn_idx) = 0;

	virtual void enableChannel(unsigned int index, bool enable) = 0;
	virtual bool isChannelEnabled(unsigned int chnIdx) = 0;
	virtual void setKernelBuffersCount(unsigned int count) = 0;
	virtual std::string getName() = 0;

	virtual struct IIO_OBJECTS getIioObjects() = 0;
	virtual unsigned int getNbChannels() = 0;
};
}
}

#endif // GENERICANALOGIN_HPP
