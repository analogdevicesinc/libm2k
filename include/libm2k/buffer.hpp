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

#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <iio.h>
#include <vector>
#include <string>
#include <functional>

namespace libm2k {
namespace utils {
class Channel;

class Buffer
{
public:
	Buffer(struct iio_device *dev);
	~Buffer();
	void push(std::vector<short> &data, unsigned int channel = 0);
	void push(std::vector<double> &data, unsigned int channel = 0);
	void push(std::vector<std::vector<short>> &data);
	void setChannels(std::vector<Channel*> channels);
	std::vector<std::vector<double>> getSamples(int nb_samples,
					std::function<double(int16_t, unsigned int)> process);
	void stop();

private:
	void destroy();
private:
	struct iio_device* m_dev;
	struct iio_buffer* m_buffer;
	std::vector<Channel*> m_channel_list;
	std::vector<std::vector<double>> m_data;
};
}
}

#endif //BUFFER_HPP
