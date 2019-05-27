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
#include <memory>
#include <functional>
#include <libm2k/m2kglobal.hpp>

namespace libm2k {
namespace utils {
class Channel;

class Buffer
{
public:
	Buffer(struct iio_device *dev);
	~Buffer();
	void push(std::vector<short> const &data, unsigned int channel = 0,
		  bool cyclic = true, bool multiplex = false);
	void push(std::vector<double> const &data, unsigned int channel = 0, bool cyclic = true);
	void push(std::vector<std::vector<short>> const &data);
	void setChannels(std::vector<Channel*> channels);
	std::vector<unsigned short> getSamples(unsigned int nb_samples);
	std::vector<std::vector<double>> getSamples(unsigned int nb_samples,
					std::function<double(int16_t, unsigned int)> process);
	void stop();
private:

	class BufferImpl;
	std::shared_ptr<BufferImpl> m_pimpl;
};
}
}

#endif //BUFFER_HPP
