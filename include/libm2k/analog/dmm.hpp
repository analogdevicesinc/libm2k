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

#ifndef GENERICDMM_HPP
#define GENERICDMM_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/utils/device.hpp>
#include <vector>
#include <map>
#include <string>

using namespace libm2k::utils;

namespace libm2k {
namespace analog {
class LIBM2K_API DMM : public Device {
public:
	struct dmm_reading {
		std::string name;
		double value;
		std::string unit;
	};

	DMM(struct iio_context *ctx, std::string dev);
	virtual ~DMM();

	std::vector<std::string> getAllChannels();
	dmm_reading readChannel(unsigned int index);
	dmm_reading readChannel(std::string chn_name);
	std::vector<dmm_reading> readAll();

private:
	std::map<std::string, unsigned int> m_channel_id_list;
};
}
}


#endif //GENERICDMM_HPP
