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

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iio.h>
#include <vector>
#include <string>

namespace libm2k {
namespace utils {
class Channel
{
public:
	Channel(struct iio_device* device, unsigned int channel = 0);
	Channel(struct iio_device* device, std::string channel_name, bool output);
	~Channel();

	std::string getName();
	std::string getId();
	unsigned int getIndex();
	bool isOutput();
	void write(struct iio_buffer* buffer, std::vector<short> &data);
	void write(struct iio_buffer* buffer, std::vector<double> &data);
	void enableChannel(bool enable);
private:
	struct iio_device *m_device;
	struct iio_channel *m_channel;

};
}
}
#endif //CHANNEL_HPP
