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

#include <libm2k/utils/utils.hpp>
#include <libm2k/logger.hpp>
#include <libm2k/context.hpp>
#include <algorithm>
#include <cstring>

#include <iostream>

using namespace std;
using namespace libm2k;
using namespace libm2k::utils;
using namespace libm2k::context;


/** Represents an iio_device **/
DeviceOut::DeviceOut(struct iio_context* context, std::string dev_name) :
	DeviceGeneric(context, dev_name)
{
	m_channel_list = m_channel_list_out;
	for (Channel *chn : m_channel_list) {
		if (chn->hasAttribute("scale") &&
				chn->hasAttribute("raw") &&
				chn->hasAttribute("phase") &&
				chn->hasAttribute("frequency")) {
			m_dds_channel_list.push_back(chn);
		} else {
			m_buffered_channel_list.push_back(chn);
		}
	}
}

DeviceOut::~DeviceOut()
{
	m_channel_list.clear();
	m_dds_channel_list.clear();
	m_buffered_channel_list.clear();
}

void DeviceOut::initializeBuffer(unsigned int size, bool cyclic)
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Device: Cannot push; device not buffer capable", libm2k::EXC_RUNTIME_ERROR);
	}
	m_buffer->initializeBuffer(size, cyclic, true);
}

void DeviceOut::cancelBuffer()
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Device: not buffer capable", libm2k::EXC_INVALID_PARAMETER);
	}
	m_buffer->cancelBuffer();
}

void DeviceOut::stop()
{
	if (m_buffer) {
		m_buffer->stop();
	}
}

void DeviceOut::setKernelBuffersCount(unsigned int count)
{
	if (m_dev) {
		int ret = iio_device_set_kernel_buffers_count(m_dev, count);
		if (ret != 0) {
			THROW_M2K_EXCEPTION("Device: Cannot set the number of kernel buffers", libm2k::EXC_RUNTIME_ERROR, ret);
		}
		const char *deviceName = iio_device_get_name(m_dev);
		LIBM2K_LOG(INFO,
                   libm2k::buildLoggingMessage({deviceName}, ("Set kernel buffers count: " + std::to_string(count)).c_str()));
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

unsigned int DeviceOut::getNbBufferedChannels()
{
	return m_buffered_channel_list.size();
}

unsigned int DeviceOut::getNbDdsChannels()
{
	return m_dds_channel_list.size();
}

void DeviceOut::enableBufferedChannel(unsigned int chnIdx, bool enable)
{
	if (chnIdx >= getNbBufferedChannels()) {
		THROW_M2K_EXCEPTION("No buffered channel for this index.", libm2k::EXC_OUT_OF_RANGE);
	}
	m_buffered_channel_list.at(chnIdx)->enableChannel(enable);
}

void DeviceOut::enableDdsChannel(unsigned int chnIdx, bool enable)
{
	if (chnIdx >= getNbDdsChannels()) {
		THROW_M2K_EXCEPTION("No buffered channel for this index.", libm2k::EXC_OUT_OF_RANGE);
	}
	m_dds_channel_list.at(chnIdx)->enableChannel(enable);
}

bool DeviceOut::isBufferedChannelEnabled(unsigned int chnIdx)
{
	if (chnIdx >= getNbBufferedChannels()) {
		THROW_M2K_EXCEPTION("No buffered channel for this index.", libm2k::EXC_OUT_OF_RANGE);
		return false;
	}
	return m_buffered_channel_list.at(chnIdx)->isEnabled();
}

bool DeviceOut::isDdsChannelEnabled(unsigned int chnIdx)
{
	if (chnIdx >= getNbDdsChannels()) {
		THROW_M2K_EXCEPTION("No buffered channel for this index.", libm2k::EXC_OUT_OF_RANGE);
		return false;
	}
	return m_dds_channel_list.at(chnIdx)->isEnabled();
}

void DeviceOut::push(const void *data, unsigned int channel, unsigned int nb_samples,
		bool cyclic, bool multiplex)
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Device: Cannot push; device not buffer capable", libm2k::EXC_RUNTIME_ERROR);
	}
	m_buffer->setChannels(m_buffered_channel_list);
	m_buffer->push(data, channel, nb_samples, cyclic, multiplex);
}

void DeviceOut::pushInterleaved(const void *data, unsigned int nb_samples_per_channel, bool cyclic, bool multiplex)
{
	if (!m_buffer) {
		THROW_M2K_EXCEPTION("Device: Cannot push; device not buffer capable", libm2k::EXC_RUNTIME_ERROR);
	}
	m_buffer->setChannels(m_buffered_channel_list);
	m_buffer->pushInterleaved(data, nb_samples_per_channel, cyclic, multiplex);
}
