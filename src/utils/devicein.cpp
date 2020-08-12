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

#include "devicein.hpp"
#include "utils/buffer.hpp"
#include "utils/channel.hpp"
#include <libm2k/utils/utils.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/context.hpp>
#include <algorithm>
#include <string>

using namespace std;
using namespace libm2k::utils;
using namespace libm2k::context;


/** Represents an iio_device **/
DeviceIn::DeviceIn(struct iio_context* context, std::string dev_name) :
	DeviceGeneric(context, dev_name)
{
	m_channel_list = m_channel_list_in;
}

void DeviceIn::initializeBuffer(unsigned int nb_samples)
{
	m_buffer->initializeBuffer(nb_samples, false, false);
}

void DeviceIn::cancelBuffer()
{
	if (!m_buffer) {
		throw_exception(m2k_exception::make("Device: not buffer capable").type(libm2k::EXC_INVALID_PARAMETER).build());
	}
	m_buffer->cancelBuffer();
}


DeviceIn::~DeviceIn()
{
	m_channel_list.clear();
}

std::vector<unsigned short> DeviceIn::getSamplesShort(unsigned int nb_samples)
{
	if (!m_buffer) {
		throw_exception(m2k_exception::make("Device: Cannot refill; device not buffer capable").type(libm2k::EXC_INVALID_PARAMETER).build());
		return std::vector<unsigned short>();
	}
	m_buffer->setChannels(m_channel_list);
	return m_buffer->getSamples(nb_samples);

}

const unsigned short* DeviceIn::getSamplesP(unsigned int nb_samples)
{
	if (!m_buffer) {
		throw_exception(m2k_exception::make("Device: Cannot refill; device not buffer capable").type(libm2k::EXC_INVALID_PARAMETER).build());
		return nullptr;
	}
	m_buffer->setChannels(m_channel_list);
	return m_buffer->getSamplesP(nb_samples);

}

std::vector<std::vector<double> > DeviceIn::getSamples(unsigned int nb_samples,
						       const std::function<double(int16_t, unsigned int)> &process)
{
	if (!m_buffer) {
		throw_exception(m2k_exception::make("Device: Cannot refill; device not buffer capable").type(libm2k::EXC_INVALID_PARAMETER).build());
		std::vector<std::vector<double>>();
	}
	m_buffer->setChannels(m_channel_list);
	return m_buffer->getSamples(nb_samples, process);
}

void* DeviceIn::getSamplesRawInterleavedVoid(unsigned int nb_samples)
{
	if (!m_buffer) {
		throw_exception(m2k_exception::make("Device: Cannot refill; device not buffer capable").type(libm2k::EXC_INVALID_PARAMETER).build());
		return nullptr;
	}
	m_buffer->setChannels(m_channel_list);
	return m_buffer->getSamplesRawInterleavedVoid(nb_samples);
}

const double *DeviceIn::getSamplesInterleaved(unsigned int nb_samples,
					      const std::function<double(int16_t, unsigned int)> &process)
{
	if (!m_buffer) {
		throw_exception(m2k_exception::make("Device: Cannot refill; device not buffer capable").type(libm2k::EXC_INVALID_PARAMETER).build());
		return nullptr;
	}
	m_buffer->setChannels(m_channel_list);
	return m_buffer->getSamplesInterleaved(nb_samples, process);
}

void DeviceIn::getSamples(std::vector<std::vector<double> > &data, unsigned int nb_samples,
			  const std::function<double(int16_t, unsigned int)> &process)
{
	if (!m_buffer) {
		throw_exception(m2k_exception::make("Device: Cannot refill; device not buffer capable").type(libm2k::EXC_INVALID_PARAMETER).build());
	}
	m_buffer->setChannels(m_channel_list);
	m_buffer->getSamples(data, nb_samples, process);
}

void DeviceIn::getSamples(std::vector<unsigned short> &data, unsigned int nb_samples)
{
	if (!m_buffer) {
		throw_exception(m2k_exception::make("Device: Cannot refill; device not buffer capable").type(libm2k::EXC_INVALID_PARAMETER).build());
	}
	m_buffer->setChannels(m_channel_list);
	m_buffer->getSamples(data, nb_samples);
}

const short *DeviceIn::getSamplesRawInterleaved(unsigned int nb_samples)
{
	if (!m_buffer) {
		throw_exception(m2k_exception::make("Device: Cannot refill; device not buffer capable").type(libm2k::EXC_INVALID_PARAMETER).build());
		return nullptr;
	}
	m_buffer->setChannels(m_channel_list);
	return m_buffer->getSamplesRawInterleaved(nb_samples);
}

void DeviceIn::flushBuffer()
{
	if (!m_buffer) {
		throw_exception(m2k_exception::make("Device: Cannot refill; device not buffer capable").type(libm2k::EXC_INVALID_PARAMETER).build());
	}
	m_buffer->flushBuffer();
}

struct libm2k::IIO_OBJECTS DeviceIn::getIioObjects()
{
	IIO_OBJECTS iio_object = {};
	iio_object.buffers_rx.push_back(m_buffer->getBuffer());

	for (auto chn : m_channel_list) {
		iio_object.channels_in.push_back(chn->getChannel());
	}
	iio_object.devices.push_back(m_dev);
	iio_object.context = m_context;
	return iio_object;
}
