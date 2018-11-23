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

#ifndef GENERICDIGITALIN_HPP
#define GENERICDIGITALIN_HPP

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
namespace digital {
class LIBM2K_API GenericDigital {
public:
	GenericDigital(struct iio_context* ctx, std::string logic_dev);
	virtual ~GenericDigital();

	enum DIO_DIRECTION {
		DIO_INPUT = 0,
		DIO_OUTPUT = 1,
	};

	enum level {
		LOW = 0,
		HIGH = 1,
	};

	struct channel {
		struct iio_channel* m_channel;
		DIO_DIRECTION m_direction;

	};

	double getSampleRate();
	double setSampleRate(double sampleRate);

	void enableChannel(unsigned int index, bool enable);
	unsigned int nbChannels();
	struct iio_context* ctx();

	typedef std::vector<unsigned short>::iterator iterator;

	iterator begin()
	{
		return m_data.begin();
	}

	iterator end()
	{
		return m_data.end();
	}

	unsigned short* data()
	{
		return m_data.data();
	}

protected:
	struct iio_context* m_ctx;
	struct iio_device* m_dev;
	std::string m_dev_name;
	std::vector<channel> m_channel_list;
	std::vector<unsigned short> m_data;
	unsigned int m_nb_channels;
	bool m_cyclic;
};
}
}

#endif //GENERICDIGITALIN_HPP
