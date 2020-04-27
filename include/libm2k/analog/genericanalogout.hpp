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
	virtual ~GenericAnalogOut() {}

	virtual double getSampleRate() = 0;
	virtual double getSampleRate(unsigned int) = 0;
	virtual double setSampleRate(double sampleRate) = 0;
	virtual double setSampleRate(unsigned int chn_idx, double sampleRate) = 0;
	virtual std::vector<double> getAvailableSampleRates() = 0;

	virtual void setCyclic(bool en) = 0;
	virtual void setCyclic(unsigned int chn, bool en) = 0;
	virtual bool getCyclic(unsigned int chn) = 0;

	virtual void push(unsigned int chn_idx, std::vector<double> const &data) = 0;
	virtual void pushRaw(unsigned int chn_idx, std::vector<short> const &data) = 0;

	virtual void push(unsigned int chn_idx, double *data, unsigned int nb_samples) = 0;
	virtual void pushRaw(unsigned int chn_idx, short *data, unsigned int nb_samples) = 0;

	virtual void stop() = 0;

	virtual std::string getName() = 0;
	virtual void enableChannel(unsigned int chnIdx, bool enable) = 0;
	virtual bool isChannelEnabled(unsigned int chnIdx) = 0;
};
}
}


#endif //GENERICANALOGOUT_HPP
