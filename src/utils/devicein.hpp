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

#ifndef LIBIIO_V1
#include <iio.h>
#else
#include <iio/iio.h>
#endif

#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <memory>
#include <libm2k/m2kglobal.hpp>
#include "devicegeneric.hpp"
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
	~DeviceIn();

	std::vector<unsigned short> getSamplesShort(unsigned int nb_samples);
	const unsigned short* getSamplesP(unsigned int nb_samples);
	std::vector<std::vector<double> > getSamples(unsigned int nb_samples,
					const std::function<double (int16_t, unsigned int)> &process);
	const double *getSamplesInterleaved(unsigned int nb_samples,
					const std::function<double (int16_t, unsigned int)> &process);
	const short *getSamplesRawInterleaved(unsigned int nb_samples);
	void* getSamplesRawInterleavedVoid(unsigned int nb_samples);

	void getSamples(std::vector<std::vector<double>> &data, unsigned int nb_samples,
			const std::function<double (int16_t, unsigned int)> &process);
	void getSamples(std::vector<unsigned short> &data, unsigned int nb_samples);

	void initializeBuffer(unsigned int nb_samples);
	void cancelBuffer();
	void flushBuffer();
	struct IIO_OBJECTS getIioObjects();
private:
	std::vector<Channel*> m_channel_list;
};
}
}


#endif //DEVICEADC_HPP
