/*
 * Copyright 2019 Analog Devices, Inc.
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


#include "private/buffer_impl.cpp"

Buffer::Buffer(struct iio_device *dev) :
	m_pimpl(std::unique_ptr<BufferImpl>(new BufferImpl(dev)))
{
}

Buffer::~Buffer()
{
}

/* push on multiple channels
 * should call setChannels(channels) before using the
 * push methods
 */
void Buffer::push(std::vector<std::vector<short>> &data)
{
	m_pimpl->push(data);
}

void Buffer::setChannels(std::vector<Channel*> channels)
{
	m_pimpl->setChannels(channels);
}

//push on a certain channel
void Buffer::push(std::vector<short> &data, unsigned int channel,
		  bool cyclic, bool multiplex)
{
	m_pimpl->push(data, channel, cyclic, multiplex);
}

void Buffer::push(std::vector<double> &data, unsigned int channel, bool cyclic)
{
	m_pimpl->push(data, channel, cyclic);
}

std::vector<unsigned short> Buffer::getSamples(unsigned int nb_samples)
{
	return m_pimpl->getSamples(nb_samples);
}

std::vector<std::vector<double>> Buffer::getSamples(unsigned int nb_samples,
				std::function<double(int16_t, unsigned int)> process)
{
	return m_pimpl->getSamples(nb_samples, process);
}

void Buffer::stop()
{
	m_pimpl->stop();
}
