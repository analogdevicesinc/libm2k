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

#ifndef DMM_HPP
#define DMM_HPP

#include "libm2k/m2kglobal.hpp"
#include <vector>
#include <string>

extern "C" {
	struct iio_context;
	struct iio_device;
	struct iio_channel;
}

namespace libm2k {
namespace analog {
class LIBM2K_API DMM {
public:
	struct dmm_reading {
		std::string name;
		double value;
		std::string unit;
	};

	DMM(struct iio_context *ctx, std::string dev);
	virtual ~DMM();

	struct iio_channel* getChannel(std::string chn_name);
	dmm_reading readChannel(struct iio_channel* chn);
	dmm_reading readChannel(std::string chn_name);
	std::vector<dmm_reading> readAll();
	std::string getDeviceName();

private:
	struct iio_context* m_ctx;
	struct iio_device* m_dev;
	std::string m_dev_name;
	unsigned int m_nb_channels;
	std::vector<struct iio_channel*> m_channel_list;

	double getAttrValue(struct iio_channel* chn, std::string attr);
	bool isValidDmmChannel(struct iio_channel* chn);
};
}
}


#endif //DMM_HPP
