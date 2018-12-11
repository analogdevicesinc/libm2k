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

#include "m2kglobal.hpp"
#include <string>
#include <vector>

extern "C" {
	struct iio_context;
	struct iio_device;
	struct iio_channel;
	struct iio_buffer;
}

namespace libm2k {
namespace analog {
class LIBM2K_API GenericAnalogOut {
public:
	GenericAnalogOut(struct iio_context* ctx, std::string dac_dev);
	virtual ~GenericAnalogOut();

	virtual void openAnalogOut();
	virtual void closeAnalogOut();

	double getSampleRate();
	double getSampleRate(unsigned int);
	double setSampleRate(double sampleRate);
	double setSampleRate(unsigned int chn_idx, double sampleRate);
	std::vector<double> getAvailableSamplerates();

	struct iio_context* getContext();
	void enableChannel(unsigned int index, bool enable);
	bool isChannelEnabled(unsigned int index);
	std::string getDeviceName();


	virtual void push(std::vector<short>& data, bool cyclic = true,
			  unsigned int chn_idx = 0);
	virtual void stopOutput();
	virtual short processSample(double value, bool raw);

	virtual void setupBeforeBuffer();
	virtual void setupAfterBuffer();

protected:
	struct iio_context* m_ctx;
	struct iio_device* m_dev;
	struct iio_buffer* m_buffer;
	std::string m_dev_name;
	unsigned int m_nb_channels;
	std::vector<struct iio_channel*> m_channel_list;
};
}
}


#endif //GENERICANALOGOUT_HPP
