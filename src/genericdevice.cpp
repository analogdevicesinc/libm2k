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
#include <iio.h>
#include <iostream>
#include <memory>

using namespace libm2k::analog;
using namespace libm2k::digital;
using namespace libm2k::devices;
using namespace libm2k::utils;

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
	m_instancesAnalogIn.clear();

//	for (int i = 0; i < s_instancesAnalogOut.size(); i++) {
//	}
	m_instancesAnalogOut.clear();

	m_instancesDMM.clear();
	m_instancesPowerSupply.clear();

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
					(getIioDeviceType(dev) == Utils::ANALOG) &&
					getIioDeviceDirection(dev) == Utils::INPUT) {
				try {
					auto aIn = new GenericAnalogIn(m_ctx, dev);
					m_instancesAnalogIn.push_back(aIn);
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
					(getIioDeviceType(dev) == Utils::ANALOG) &&
					getIioDeviceDirection(dev) == Utils::OUTPUT) {
				auto aOut = new GenericAnalogOut(m_ctx, dev);
				m_instancesAnalogOut.push_back(aOut);
			}
		} catch (std::runtime_error &e) {
			throw std::runtime_error(e.what());
		}
	}
}

void GenericDevice::scanAllDigital()
{

}

GenericAnalogIn* GenericDevice::getAnalogIn(unsigned int index)
{
	if (index < m_instancesAnalogIn.size()) {
		return m_instancesAnalogIn.at(index);
	} else {
//		throw no_device_exception("No such analog in");
		return nullptr;
	}
}

GenericAnalogIn* GenericDevice::getAnalogIn(std::string dev_name)
{
	for (GenericAnalogIn* d : m_instancesAnalogIn) {
		if (d->getDeviceName() == dev_name) {
			return d;
		}
	}
	return nullptr;
}

GenericAnalogOut* GenericDevice::getAnalogOut(unsigned int index)
{
	if (index < m_instancesAnalogOut.size()) {
		return m_instancesAnalogOut.at(index);
	} else {
		return nullptr;
	}
}

GenericAnalogOut* GenericDevice::getAnalogOut(std::string dev_name)
{
	for (GenericAnalogOut* d : m_instancesAnalogOut) {
		if (d->getName() == dev_name) {
			return d;
		}
	}
	return nullptr;
}

DMM* GenericDevice::getDMM(unsigned int index)
{
	if (index < m_instancesDMM.size()) {
		return m_instancesDMM.at(index);
	} else {
		return nullptr;
	}
}

DMM* GenericDevice::getDMM(std::string dev_name)
{
	for (DMM* d : m_instancesDMM) {
		if (d->getDeviceName() == dev_name) {
			return d;
		}
	}
	return nullptr;
}

std::vector<DMM*> GenericDevice::getAllDmm()
{
	return m_instancesDMM;
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

libm2k::utils::Utils::DEVICE_TYPE GenericDevice::getIioDeviceType(std::string dev_name)
{
	auto dev = iio_context_find_device(m_ctx, dev_name.c_str());
	if (!dev) {
		throw no_device_exception("No device found with name: " + dev_name);
	}

	auto chn = iio_device_get_channel(dev, 0);
	if (!chn) {
		return Utils::NONE;
	}

	const struct iio_data_format* data_format = iio_channel_get_data_format(chn);
	if (data_format->bits == 1) {
		return Utils::DIGITAL;
	} else {
		return Utils::ANALOG;
	}
}

libm2k::utils::Utils::DEVICE_DIRECTION GenericDevice::getIioDeviceDirection(std::string dev_name)
{
	Utils::DEVICE_DIRECTION dir = Utils::NO_DIRECTION;
	auto dev = iio_context_find_device(m_ctx, dev_name.c_str());
	if (!dev) {
		throw no_device_exception("No device found with name: " + dev_name);
	}

	auto chn_count = iio_device_get_channels_count(dev);
	for (unsigned int i = 0; i < chn_count; i++) {
		auto chn = iio_device_get_channel(dev, i);
		if (iio_channel_is_output(chn)) {
			if (dir == Utils::INPUT) {
				dir = Utils::BOTH;
			} else if (dir != Utils::BOTH){
				dir = Utils::OUTPUT;
			}
		} else {
			if (dir == Utils::OUTPUT) {
				dir = Utils::BOTH;
			} else if (dir != Utils::BOTH){
				dir = Utils::INPUT;
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
			if (getIioDeviceDirection(dev.first) != Utils::OUTPUT) {
				if (!getDMM(dev.first)) {
					auto dmm = new DMM(m_ctx, dev.first);
					m_instancesDMM.push_back(dmm);
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

std::string GenericDevice::getContextAttributes()
{
	if (!m_ctx) {
		return "";
		//throw error?
	}
	std::string descr = std::string(iio_context_get_description(m_ctx));
	return descr;
//	int attr_no = iio_context_get_attrs_count(m_ctx);
//	for (int i = 0; i < attr_no; i++) {

//	}
}

libm2k::devices::M2K* GenericDevice::toM2k()
{
	libm2k::devices::M2K* dev = dynamic_cast<libm2k::devices::M2K*>(this);
	if(dev) {
		return dev;
	} else {
		return nullptr;
	}
}
