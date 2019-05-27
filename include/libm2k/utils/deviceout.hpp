/*
 * Copyright 2016 Analog Devices, Inc.
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

#ifndef DEVICEOUT_HPP
#define DEVICEOUT_HPP
#include <iio.h>
#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <memory>
#include <libm2k/m2kglobal.hpp>
#include <libm2k/utils/devicegeneric.hpp>

using namespace std;

namespace libm2k {
namespace utils {
class Channel;
class Buffer;

class DeviceOut : public DeviceGeneric
{
public:
	DeviceOut(struct iio_context* context, std::string dev_name = "", bool input = false);
	virtual ~DeviceOut();

	virtual void push(std::vector<short> const &data, unsigned int channel,
		  bool cyclic = true, bool multiplex = false);
	virtual void push(std::vector<double> const &data, unsigned int channel, bool cyclic = true);
	virtual void stop();

private:
	class DeviceOutImpl;
	std::unique_ptr<DeviceOutImpl> m_pimpl;
};
}
}


#endif //DEVICE_HPP
