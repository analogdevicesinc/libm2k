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

#include "m2kglobal.hpp"
#include <string>
#include <vector>

extern "C" {
	struct iio_context;
	struct iio_device;
	struct iio_channel;
}

namespace libm2k {
namespace analog {
class LIBM2K_API GenericAnalogIn {
public:
	GenericAnalogIn(struct iio_context* ctx, std::string adc_dev);
	virtual ~GenericAnalogIn();

	virtual std::vector<std::vector<double>> getSamples(int nb_samples);
	virtual void openAnalogIn();
	virtual void closeAnalogIn();

	virtual double getSampleRate();
	virtual double getSampleRate(unsigned int);
	virtual double setSampleRate(double sampleRate);
	virtual double setSampleRate(unsigned int chn_idx, double sampleRate);

	virtual double processSample(int16_t sample, unsigned int channel);

	struct iio_context* getContext();
	void enableChannel(unsigned int index, bool enable);
	std::string getDeviceName();

	//iterator
	//if we use a stl container just forward definitions to that container
	typedef std::vector<std::vector<double>>::iterator iterator;

	iterator begin()
	{
		return m_data.begin();
	}

	iterator end()
	{
		return m_data.end();
	}

	std::vector<double>* data()
	{
		return m_data.data();
	}

protected:
	struct iio_context* m_ctx;
	struct iio_device* m_dev;
	std::vector<struct iio_channel*> m_channel_list;
	std::string m_dev_name;
	std::vector<std::vector<double>> m_data;
	unsigned int m_nb_channels;
	bool m_cyclic;
};
}
}

#endif // GENERICANALOGIN_HPP
