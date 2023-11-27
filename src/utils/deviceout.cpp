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

#include "deviceout.hpp"
#include "utils/buffer.hpp"
#include "utils/channel.hpp"
#include <libm2k/utils/utils.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/logger.hpp>
#include <libm2k/context.hpp>
#include <algorithm>
#include <cstring>

using namespace std;
using namespace libm2k;
using namespace libm2k::utils;
using namespace libm2k::context;


/** Represents an iio_device **/
DeviceOut::DeviceOut(struct iio_context* context, std::string dev_name) :
	DeviceGeneric(context, dev_name)
{
	m_channel_list = m_channel_list_out;
}

DeviceOut::~DeviceOut()
{
	m_channel_list.clear();
}

void DeviceOut::initializeBuffer(unsigned int size, bool cyclic)
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Device: Cannot push; device not buffer capable", libm2k::EXC_RUNTIME_ERROR);
	}
	m_buffer->initializeBuffer(size, cyclic, true);
}

void DeviceOut::push(std::vector<short> const &data, unsigned int channel,
		     bool cyclic, bool multiplex, bool enableFlag)
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Device: Cannot push; device not buffer capable", libm2k::EXC_RUNTIME_ERROR);
	}
	m_buffer->setChannels(m_channel_list);
	m_buffer->push(data, channel, cyclic, multiplex, enableFlag);
}


void DeviceOut::cancelBuffer()
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Device: not buffer capable", libm2k::EXC_INVALID_PARAMETER);
	}
	m_buffer->cancelBuffer();
}


void DeviceOut::push(std::vector<unsigned short> const &data, unsigned int channel,
		     bool cyclic, bool multiplex, bool enableFlag)
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Device: Cannot push; device not buffer capable", libm2k::EXC_RUNTIME_ERROR);
	}
	m_buffer->setChannels(m_channel_list);
	m_buffer->push(data, channel, cyclic, multiplex, enableFlag);
}


void DeviceOut::push(unsigned short *data, unsigned int channel, unsigned int nb_samples,
		     bool cyclic, bool multiplex, bool enableFlag)
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Device: Cannot push; device not buffer capable", libm2k::EXC_RUNTIME_ERROR);
	}
	m_buffer->setChannels(m_channel_list);
	m_buffer->push(data, channel, nb_samples, cyclic, multiplex, enableFlag);
}

void DeviceOut::push(std::vector<double> const &data, unsigned int channel, bool cyclic, bool enableFlag)
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Device: Cannot push; device not buffer capable", libm2k::EXC_RUNTIME_ERROR);
	}
	m_buffer->setChannels(m_channel_list);
	m_buffer->push(data, channel, cyclic, enableFlag);
}

void DeviceOut::push(double *data, unsigned int channel, unsigned int nb_samples, bool cyclic, bool enableFlag)
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Device: Cannot push; device not buffer capable", libm2k::EXC_RUNTIME_ERROR);
	}
	m_buffer->setChannels(m_channel_list);
	m_buffer->push(data, channel, nb_samples, cyclic, enableFlag);
}

void DeviceOut::push(short *data, unsigned int channel, unsigned int nb_samples, bool cyclic, bool enableFlag)
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Device: Cannot push; device not buffer capable", libm2k::EXC_RUNTIME_ERROR);
	}
	m_buffer->setChannels(m_channel_list);
	m_buffer->push(data, channel, nb_samples, cyclic, enableFlag);
}

void DeviceOut::stop()
{
	if (m_buffer) {
		m_buffer->stop();
	}
}

struct libm2k::IIO_OBJECTS DeviceOut::getIioObjects()
{
	IIO_OBJECTS iio_object = {};
	iio_object.buffers_tx.push_back(m_buffer->getBuffer());

	for (auto chn : m_channel_list) {
		iio_object.channels_out.push_back(chn->getChannel());
	}
	iio_object.devices.push_back(m_dev);
	iio_object.context = m_context;
	return iio_object;
}
