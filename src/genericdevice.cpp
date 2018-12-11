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
#include "libm2k/genericanalogout.hpp"
#include "libm2k/powersupply.hpp"
#include "libm2k/dmm.hpp"
#include "libm2k/m2k.hpp"

#include "utils.hpp"
#include <iio.h>
#include <iostream>
#include <memory>

using namespace libm2k::analog;
using namespace libm2k::digital;
using namespace libm2k::devices;
using namespace libm2k::utils;

std::vector<std::shared_ptr<GenericAnalogIn>> GenericDevice::s_instancesAnalogIn = {};
std::vector<std::shared_ptr<GenericAnalogOut>> GenericDevice::s_instancesAnalogOut = {};
std::vector<std::shared_ptr<DMM>> GenericDevice::s_instancesDMM = {};
std::vector<std::shared_ptr<PowerSupply>> GenericDevice::s_instancesPowerSupply = {};
std::vector<std::shared_ptr<GenericDigital>> GenericDevice::s_instancesDigital = {};

GenericDevice::GenericDevice(std::string uri, struct iio_context *ctx, std::string name)
{
	m_ctx = ctx;
	m_uri = uri;

	/* Initialize the AnalogIn list */
	scanAllAnalogIn();

	/* Initialize the AnalogIn list */
	scanAllAnalogOut();

	/* Initialize the DMM list */
	scanAllDMM();

	/* Initialize the power supply list */
	scanAllPowerSupply();
}

GenericDevice::~GenericDevice()
{
//	for (auto aIn : s_instancesAnalogIn) {
//	}
	s_instancesAnalogIn.clear();

//	for (int i = 0; i < s_instancesAnalogOut.size(); i++) {
//	}
	s_instancesAnalogOut.clear();

	s_instancesDMM.clear();
	s_instancesPowerSupply.clear();

	if (m_ctx) {
		iio_context_destroy(m_ctx);
		std::cout << "destroying IIO context\n";
	}
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
					auto aIn = std::make_shared<GenericAnalogIn>(m_ctx, dev);
					s_instancesAnalogIn.push_back(aIn);
				} catch (std::runtime_error& e) {
					std::cout << e.what() << std::endl;
				}
			}
		} catch (std::runtime_error& e) {
			throw std::runtime_error(e.what());
		}
	}
}

void GenericDevice::scanAllAnalogOut()
{
	auto dev_list = Utils::getAllDevices(m_ctx);
	for (auto dev : dev_list) {
		try {
			if (isIioDeviceBufferCapable(dev) &&
					(getIioDeviceType(dev) == ANALOG) &&
					getIioDeviceDirection(dev) == OUTPUT) {
				auto aOut = std::make_shared<GenericAnalogOut>(m_ctx, dev);
				s_instancesAnalogOut.push_back(aOut);
			}
		} catch (std::runtime_error &e) {
			throw std::runtime_error(e.what());
		}
	}
}

void GenericDevice::scanAllDigital()
{

}

std::shared_ptr<GenericAnalogIn> GenericDevice::getAnalogIn(unsigned int index)
{
	if (index < s_instancesAnalogIn.size()) {
		return s_instancesAnalogIn.at(index);
	} else {
//		throw no_device_exception("No such analog in");
		return nullptr;
	}
}

std::shared_ptr<GenericAnalogIn> GenericDevice::getAnalogIn(std::string dev_name)
{
	for (std::shared_ptr<GenericAnalogIn> d : s_instancesAnalogIn) {
		if (d->getDeviceName() == dev_name) {
			return d;
		}
	}
	return nullptr;
}

std::shared_ptr<GenericAnalogOut> GenericDevice::getAnalogOut(unsigned int index)
{
	if (index < s_instancesAnalogOut.size()) {
		return s_instancesAnalogOut.at(index);
	} else {
		return nullptr;
	}
}

std::shared_ptr<GenericAnalogOut> GenericDevice::getAnalogOut(std::string dev_name)
{
	for (std::shared_ptr<GenericAnalogOut> d : s_instancesAnalogOut) {
		if (d->getDeviceName() == dev_name) {
			return d;
		}
	}
	return nullptr;
}

std::shared_ptr<DMM> GenericDevice::getDMM(unsigned int index)
{
	if (index < s_instancesDMM.size()) {
		return s_instancesDMM.at(index);
	} else {
		return nullptr;
	}
}

std::shared_ptr<DMM> GenericDevice::getDMM(std::string dev_name)
{
	for (std::shared_ptr<DMM> d : s_instancesDMM) {
		if (d->getDeviceName() == dev_name) {
			return d;
		}
	}
	return nullptr;
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
	for (unsigned int i = 0; i < chn_count; i++) {
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

void GenericDevice::scanAllDMM()
{
	auto dev_list = Utils::getIioDevByChannelAttrs(m_ctx, {"raw", "scale"});
	for (auto dev : dev_list) {
		try {
			if (getIioDeviceDirection(dev.first) != OUTPUT) {
				if (!getDMM(dev.first)) {
					auto dmm = std::make_shared<DMM>(m_ctx, dev.first);
					s_instancesDMM.push_back(dmm);
				}
			}
		} catch (std::runtime_error &e) {
			std::cout << e.what() << std::endl;
		}
	}
}

std::string GenericDevice::getUri()
{
	return m_uri;
}

void GenericDevice::scanAllPowerSupply()
{
}

void GenericDevice::blinkLed()
{
	Utils::blinkLed(m_ctx, 4, true);
}

iio_context *GenericDevice::ctx()
{
	return m_ctx;
}

std::shared_ptr<libm2k::devices::M2K> GenericDevice::toM2k()
{
	std::shared_ptr<libm2k::devices::M2K> dev =
		std::dynamic_pointer_cast<libm2k::devices::M2K>(shared_from_this());
	if(dev) {
		return dev;
	} else {
		return nullptr;
	}
}
