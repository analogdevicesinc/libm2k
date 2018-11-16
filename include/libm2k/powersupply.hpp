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

#ifndef POWERSUPPLY_HPP
#define POWERSUPPLY_HPP

#include <libm2k/m2kglobal.hpp>
#include <vector>
#include <string>

extern "C" {
	struct iio_context;
	struct iio_device;
	struct iio_channel;
}

namespace libm2k {
namespace analog {
class LIBM2K_API PowerSupply {
public:
	PowerSupply(struct iio_context* ctx, std::string write_dev = "",
		     std::string read_dev = "");
	virtual ~PowerSupply();

	virtual void enableChannel(unsigned int chn, bool en) = 0;
	virtual void enableAll(bool en) = 0;
	virtual double readChannel(unsigned int chn) = 0;
	virtual void pushChannel(unsigned int chn, double value) = 0;

	struct iio_context* getContext();
	struct iio_device *getWriteDevice();
	struct iio_device *getReadDevice();
protected:
	std::vector<struct iio_channel*> m_write_channels;
	std::vector<struct iio_channel*> m_read_channels;
	std::vector<bool> m_channels_enabled;
	unsigned int m_nb_write_channels;
	unsigned int m_nb_read_channels;
private:
	struct iio_context* m_ctx;
	struct iio_device* m_dev_write;
	struct iio_device* m_dev_read;
	std::string m_dev_write_name;
	std::string m_dev_read_name;
};
}
}

#endif //POWERSUPPLY_HPP
