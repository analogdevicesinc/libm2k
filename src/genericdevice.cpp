/*
 * Copyright 2018 Analog Devices, Inc.
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

#include "libm2k/genericdevice.hpp"
#include "libm2k/m2kexceptions.hpp"
#include "libm2k/genericanalogin.hpp"

#include "utils.hpp"
#include <iostream>

using namespace libm2k::analog;
using namespace libm2k::devices;
using namespace libm2k::utils;

std::vector<GenericAnalogIn*> GenericDevice::s_instancesAnalogIn = {};

GenericDevice::GenericDevice(std::string uri, struct iio_context *ctx, std::string name)
{
	m_ctx = ctx;
	m_uri = uri;

	/* Initialize the AnalogIn list */
	scanAllAnalogIn();
}

GenericDevice::~GenericDevice()
{
	for (auto aIn : s_instancesAnalogIn) {
		delete aIn;
	}
	s_instancesAnalogIn.clear();
	if (m_ctx) {
		iio_context_destroy(m_ctx);
	}
}

GenericDevice* GenericDevice::getDevice(std::string uri)
{
	struct iio_context* ctx = iio_create_context_from_uri(uri.c_str());
	if (!ctx) {
		throw no_device_exception("No device found for uri: " + uri);
	}
	return nullptr;
}

void GenericDevice::scanAllAnalogIn()
{
	auto dev_list = Utils::getAllDevices(m_ctx);
	for (auto dev : dev_list) {
		try {
			if (isIioDeviceBufferCapable(dev) &&
					(getIioDeviceType(dev) == ANALOG) &&
					getIioDeviceDirection(dev) == INPUT) {
				try {
					auto aIn = new GenericAnalogIn(m_ctx, dev);
					s_instancesAnalogIn.push_back(aIn);
				} catch (std::runtime_error& e) {
					std::cout << e.what() << std::endl;
				}
			}
		} catch (no_device_exception& e) {
			std::cout << e.what() << "\n";
		}
	}
}

GenericAnalogIn *GenericDevice::getAnalogIn(int index)
{
	if (index < s_instancesAnalogIn.size()) {
		return s_instancesAnalogIn.at(index);
	} else {
		throw no_device_exception("No such analog in");
	}
}

GenericAnalogIn *GenericDevice::getAnalogIn(std::string dev_name)
{
	for (GenericAnalogIn* d : s_instancesAnalogIn) {
		if (d->getDeviceName() == dev_name) {
			return d;
		}
	}
	throw no_device_exception("No such analog in");
}

bool GenericDevice::isIioDeviceBufferCapable(std::string dev_name)
{
	unsigned int dev_count = iio_device_get_buffer_attrs_count(
				iio_context_find_device(m_ctx, dev_name.c_str()));
	if (dev_count > 0) {
		return true;
	} else {
		return false;
	}
}

GenericDevice::DEVICE_TYPE GenericDevice::getIioDeviceType(std::string dev_name)
{
	auto dev = iio_context_find_device(m_ctx, dev_name.c_str());
	if (!dev) {
		throw no_device_exception("No device found with name: " + dev_name);
	}

	auto chn = iio_device_get_channel(dev, 0);
	if (!chn) {
		return GenericDevice::NONE;
	}

	const struct iio_data_format* data_format = iio_channel_get_data_format(chn);
	if (data_format->bits == 1) {
		return GenericDevice::DIGITAL;
	} else {
		return GenericDevice::ANALOG;
	}
}

GenericDevice::DEVICE_DIRECTION GenericDevice::getIioDeviceDirection(std::string dev_name)
{
	DEVICE_DIRECTION dir = NO_DIRECTION;
	auto dev = iio_context_find_device(m_ctx, dev_name.c_str());
	if (!dev) {
		throw no_device_exception("No device found with name: " + dev_name);
	}

	auto chn_count = iio_device_get_channels_count(dev);
	for (int i = 0; i < chn_count; i++) {
		auto chn = iio_device_get_channel(dev, i);
		if (iio_channel_is_output(chn)) {
			if (dir == INPUT) {
				dir = BOTH;
			} else if (dir != BOTH){
				dir = OUTPUT;
			}
		} else {
			if (dir == OUTPUT) {
				dir = BOTH;
			} else if (dir != BOTH){
				dir = INPUT;
			}
		}
	}
	return dir;
}

void GenericDevice::blinkLed()
{
	Utils::blinkLed(m_ctx, 4, true);
}

iio_context *GenericDevice::ctx()
{
	return m_ctx;
}

