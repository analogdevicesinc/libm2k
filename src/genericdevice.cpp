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

#include "utils.hpp"
#include <iostream>


using namespace libm2k::devices;
using namespace libm2k::utils;

std::vector<AnalogIn*> GenericDevice::s_instancesAnalogIn = {};

GenericDevice::GenericDevice(std::string uri, struct iio_context *ctx, std::string name)
{
//	m_ctx = iio_create_context_from_uri(uri.c_str());
//	if (!m_ctx) {
//		throw no_device_exception("No device found for uri: " + uri);
//	}
	m_ctx = ctx;
	m_uri = uri;
}

GenericDevice::~GenericDevice()
{
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

//	for (auto x: Utils::getAllDevices(ctx)) {
//		std::cout << x << std::endl;
//	}

//	std::string hw_name = identifyDevice(ctx);
//	return new M2K(uri, ctx, hw_name);

	return nullptr;
}

/**
 * @brief GenericDevice::analogInOpen
 * Should search devices with "raw" attr or "adc"
 * @return
 */
AnalogIn* GenericDevice::analogInOpen()
{
//	unsigned int nb_devices = iio_context_get_devices_count(m_ctx);
//	for (unsigned int i = 0; i < nb_devices; i++) {
//		auto dev = iio_context_get_device(m_ctx, i);
//		std::cout << iio_device_get_name(dev) << std::endl;
//	}

//	for (auto el : Utils::getIioDevByGlobalAttrs(m_ctx, {"raw"})) {
//		std::cout << "elem global attr: " << el << std::endl;
//	}

	for (auto el : Utils::getIioDevByChannelAttrs(m_ctx, {"raw"})) {
		if (std::string(el.first).find("logic") == std::string::npos) {
			if (Utils::isInputChannel(m_ctx, el.first, el.second)) {
				if (getAnalogInInstance(el.first) == nullptr) {
					try {
						AnalogIn* aIn = new AnalogIn(m_ctx, el.first, false);
						s_instancesAnalogIn.push_back(aIn);
						std::cout << "elem channel attr: " << el.first << " " << el.second << std::endl;
					} catch(no_device_exception& e)  {
						std::cout << e.what() << std::endl;
					}
				}
			}
		}
	}
//	return new AnalogIn(m_ctx, , false);
	return nullptr;
}

/**
 * @brief GenericDevice::analogInClose
 */
void GenericDevice::analogInClose(AnalogIn* analogInDev)
{

}

AnalogIn* GenericDevice::getAnalogInInstance(std::string d_name)
{
	AnalogIn* device = nullptr;;
	for (AnalogIn* d : s_instancesAnalogIn) {
		if (d->getIioDeviceName() == d_name) {
			device = d;
			break;
		}
	}
	return device;
}

void GenericDevice::blinkLed()
{
	Utils::blinkLed(m_ctx, 4, true);
}

