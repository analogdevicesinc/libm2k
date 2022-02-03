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

#ifndef GENERICDMM_IMPL_HPP
#define GENERICDMM_IMPL_HPP

#include <libm2k/analog/dmm.hpp>
#include <libm2k/analog/enums.hpp>
#include "utils/devicein.hpp"
#include <vector>
#include <map>
#include <string>

namespace libm2k {
namespace analog {


struct dmm_info{
	std::string key;
	std::string key_symbol;
	double value = 0;
	double umScale = 1;
};

class DMMImpl : public DMM {
public:
	DMMImpl(struct iio_context *ctx, std::string dev, bool sync);
	virtual ~DMMImpl();

	void reset();
	std::vector<std::string> getAllChannels();

	libm2k::analog::DMM_READING readChannel(unsigned int index);

	libm2k::analog::DMM_READING readChannel(std::string chn_name);

	std::vector<libm2k::analog::DMM_READING> readAll();

	std::string getName();

private:
	std::map<std::string, unsigned int> m_channel_id_list;
	std::string m_dev_name;
	std::vector<libm2k::utils::DeviceIn*> m_device_in_list;
	libm2k::utils::DeviceIn *getDevice(unsigned int index);
	void generateDictionaries();
	std::map<int,dmm_info> m_iioDevices;
	std::map<int,dmm_info> m_hwmonDevices;
};
}
}



#endif //GENERICDMM_IMPL_HPP
