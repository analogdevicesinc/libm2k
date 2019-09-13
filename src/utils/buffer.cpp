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
	m_pimpl(std::shared_ptr<BufferImpl>(new BufferImpl(dev)))
{
}

Buffer::~Buffer()
{
}

/* push on multiple channels
 * should call setChannels(channels) before using the
 * push methods
 */
void Buffer::push(std::vector<std::vector<short>> const &data)
{
	m_pimpl->push(data);
}

void Buffer::setChannels(std::vector<Channel*> channels)
{
	m_pimpl->setChannels(channels);
}

//push on a certain channel
void Buffer::push(std::vector<unsigned short> const &data, unsigned int channel,
		  bool cyclic, bool multiplex)
{
	m_pimpl->push(data, channel, cyclic, multiplex);
}

void Buffer::push(std::vector<short> const &data, unsigned int channel,
                  bool cyclic, bool multiplex)
{
    m_pimpl->push(data, channel, cyclic, multiplex);
}

void Buffer::push(unsigned short *data, unsigned int channel, unsigned int nb_samples,
		  bool cyclic, bool multiplex)
{
	m_pimpl->push(data, channel, nb_samples, cyclic, multiplex);
}

void Buffer::push(std::vector<double> const &data, unsigned int channel, bool cyclic)
{
	m_pimpl->push(data, channel, cyclic);
}

void Buffer::push(double *data, unsigned int channel, unsigned int nb_samples, bool cyclic)
{
	m_pimpl->push(data, channel, nb_samples, cyclic);
}

void Buffer::push(short *data, unsigned int channel, unsigned int nb_samples, bool cyclic)
{
	m_pimpl->push(data, channel, nb_samples, cyclic);
}

std::vector<unsigned short> Buffer::getSamples(unsigned int nb_samples)
{
	return m_pimpl->getSamples(nb_samples);
}

const unsigned short* Buffer::getSamplesP(unsigned int nb_samples)
{
	return m_pimpl->getSamplesP(nb_samples);
}

std::vector<std::vector<double>> Buffer::getSamples(unsigned int nb_samples,
				std::function<double(int16_t, unsigned int)> process)
{
	return m_pimpl->getSamples(nb_samples, process);
}

const double *Buffer::getSamplesInterleaved(unsigned int nb_samples, std::function<double (int16_t, unsigned int)> process)
{
	return m_pimpl->getSamplesInterleaved(nb_samples, process);
}

const short *Buffer::getSamplesRawInterleaved(unsigned int nb_samples)
{
	return m_pimpl->getSamplesRawInterleaved(nb_samples);
}

void Buffer::stop()
{
	m_pimpl->stop();
}

void Buffer::setCyclic(bool enable)
{
	m_pimpl->setCyclic(enable);
}
