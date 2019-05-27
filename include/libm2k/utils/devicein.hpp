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

#ifndef DEVICEIN_HPP
#define DEVICEIN_HPP
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

class DeviceIn : public DeviceGeneric
{
public:
	DeviceIn(struct iio_context* context, std::string dev_name = "", bool input = false);
	virtual ~DeviceIn();

	virtual std::vector<unsigned short> getSamples(unsigned int nb_samples);
	virtual std::vector<std::vector<double> > getSamples(unsigned int nb_samples,
					std::function<double (int16_t, unsigned int)> process);

private:
	class DeviceInImpl;
	std::unique_ptr<DeviceInImpl> m_pimpl;
};
}
}


#endif //DEVICEADC_HPP
