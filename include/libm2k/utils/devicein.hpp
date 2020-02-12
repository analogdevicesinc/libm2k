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
#include <libm2k/enums.hpp>

using namespace std;

namespace libm2k {
namespace utils {
class Channel;
class Buffer;

class DeviceIn : public DeviceGeneric
{
public:
	DeviceIn(struct iio_context* context, std::string dev_name = "");
	virtual ~DeviceIn();

	virtual std::vector<unsigned short> getSamples(unsigned int nb_samples);
	virtual const unsigned short* getSamplesP(unsigned int nb_samples);
	virtual std::vector<std::vector<double> > getSamples(unsigned int nb_samples,
					std::function<double (int16_t, unsigned int)> process);
	virtual const double *getSamplesInterleaved(unsigned int nb_samples,
					std::function<double (int16_t, unsigned int)> process);
	virtual const short *getSamplesRawInterleaved(unsigned int nb_samples);
	void* getSamplesRawInterleavedVoid(unsigned int nb_samples);

	virtual void cancelBuffer();
	virtual void flushBuffer();
	virtual struct IIO_OBJECTS getIioObjects();
private:
	class DeviceInImpl;
	std::unique_ptr<DeviceInImpl> m_pimpl;
};
}
}


#endif //DEVICEADC_HPP
