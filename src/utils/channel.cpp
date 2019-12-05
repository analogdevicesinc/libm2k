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

#include "private/channel_impl.cpp"

using namespace libm2k::utils;

Channel::Channel(iio_device *device, unsigned int channel) :
	m_pimpl(std::shared_ptr<ChannelImpl>(new ChannelImpl(device, channel)))
{
}

Channel::Channel(iio_device *device, std::string channel_name, bool output) :
	m_pimpl(std::shared_ptr<ChannelImpl>(new ChannelImpl(device, channel_name, output)))
{
}

Channel::~Channel()
{
}

std::string Channel::getName()
{
	return m_pimpl->getName();
}

std::string Channel::getId()
{
	return m_pimpl->getId();
}

unsigned int Channel::getIndex()
{
	return m_pimpl->getIndex();
}

bool Channel::isOutput()
{
	return m_pimpl->isOutput();
}

bool Channel::isEnabled()
{
	return m_pimpl->isEnabled();
}

bool Channel::hasAttribute(std::string attr)
{
	return m_pimpl->hasAttribute(attr);
}

void Channel::enableChannel(bool enable)
{
	m_pimpl->enableChannel(enable);
}

uintptr_t Channel::getFirst(iio_buffer *buffer)
{
	return m_pimpl->getFirst(buffer);
}

void *Channel::getFirstVoid(iio_buffer *buffer)
{
	return m_pimpl->getFirstVoid(buffer);
}

bool Channel::isValid()
{
	return m_pimpl->isValid();
}

iio_channel *Channel::getChannel()
{
	return m_pimpl->getChannel();
}

void Channel::write(struct iio_buffer* buffer, std::vector<short> const &data)
{
    m_pimpl->write(buffer, data);
}

void Channel::write(struct iio_buffer* buffer, std::vector<unsigned short> const &data)
{
	m_pimpl->write(buffer, data);
}

void Channel::write(struct iio_buffer* buffer, std::vector<double> const &data)
{
	m_pimpl->write(buffer, data);
}

void Channel::write(struct iio_buffer* buffer, double *data, unsigned int nb_samples)
{
	m_pimpl->write(buffer, data, nb_samples);
}

void Channel::write(struct iio_buffer* buffer, short *data, unsigned int nb_samples)
{
	m_pimpl->write(buffer, data, nb_samples);
}

void Channel::write(struct iio_buffer* buffer, unsigned short *data, unsigned int nb_samples)
{
	m_pimpl->write(buffer, data, nb_samples);
}

void Channel::convert(int16_t *avg, int16_t *src)
{
	m_pimpl->convert(avg, src);
}

void Channel::convert(double *avg, int16_t *src)
{
	m_pimpl->convert(avg, src);
}

double Channel::getDoubleValue(std::string attr)
{
	return m_pimpl->getDoubleValue(attr);
}

void Channel::setDoubleValue(std::string attr, double val)
{
	m_pimpl->setDoubleValue(attr, val);
}

void Channel::setLongValue(std::string attr, long long val)
{
	m_pimpl->setLongValue(attr, val);
}

long long Channel::getLongValue(std::string attr)
{
	return m_pimpl->getLongValue(attr);
}

void Channel::setBoolValue(std::string attr, bool val)
{
	m_pimpl->setBoolValue(attr, val);
}

bool Channel::getBoolValue(std::string attr)
{
	return m_pimpl->getBoolValue(attr);
}

void Channel::setStringValue(std::string attr, std::string val)
{
	m_pimpl->setStringValue(attr, val);
}

std::string Channel::getStringValue(std::string attr)
{
	return m_pimpl->getStringValue(attr);
}
