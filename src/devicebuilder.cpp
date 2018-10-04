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

#include "libm2k/devicebuilder.hpp"
#include "libm2k/m2kexceptions.hpp"
#include "libm2k/genericdevice.hpp"
#include "installed_devices.hpp"
#include <iio.h>
#include "utils.hpp"
#include <iostream>
#include <vector>

using namespace libm2k::devices;
using namespace libm2k::utils;

std::vector<GenericDevice*> DeviceBuilder::s_connectedDevices = {};

DeviceBuilder::DeviceBuilder()// : m_pimpl(new M2KImpl())
{
	std::cout << "Constructor \n";
}

DeviceBuilder::~DeviceBuilder()
{
	std::cout << "Destructor \n";
}

std::vector<std::string> DeviceBuilder::listDevices()
{
	struct iio_context_info **info;
	unsigned int nb_contexts;
	std::vector<std::string> uris;

	for (Utils::ini_device_struct device_info : devices_info) {
		if (device_info.hw_name == "") {
			std::cout << "error" << std::endl;
		} else {
			std::cout << device_info.hw_name << " " << device_info.key_val_pairs.at(0).first << std::endl;
			for (auto i : device_info.key_val_pairs.at(0).second) {
				std::cout << i << ", ";
			}
		}
	}

	struct iio_scan_context *scan_ctx = iio_create_scan_context("usb", 0);

	if (!scan_ctx) {
		std::cout << "Unable to create scan context!" << std::endl;
		return uris;
	}

	ssize_t ret = iio_scan_context_get_info_list(scan_ctx, &info);

	if (ret < 0) {
		std::cout << "Unable to scan!" << std::endl;
		goto out_destroy_context;
	}

	nb_contexts = static_cast<unsigned int>(ret);

	for (unsigned int i = 0; i < nb_contexts; i++)
		uris.push_back(std::string(iio_context_info_get_uri(info[i])));

	iio_context_info_list_free(info);
out_destroy_context:
	iio_scan_context_destroy(scan_ctx);
	return uris;
}

GenericDevice* DeviceBuilder::deviceOpen(const char *uri)
{
	struct iio_context* ctx = iio_create_context_from_uri(uri);
	if (!ctx) {
		throw no_device_exception("No device found for uri: " + *uri);
	}
	std::string dev_name = DeviceBuilder::identifyDevice(ctx);
	std::cout << dev_name << std::endl;

	GenericDevice* dev = buildDevice(dev_name, std::string(uri), ctx);
	s_connectedDevices.push_back(dev);
	return dev;
}

void DeviceBuilder::deviceClose(GenericDevice* device)
{
	for (auto dev : s_connectedDevices) {
		delete dev;
	}
	s_connectedDevices.clear();
}

std::string DeviceBuilder::identifyDevice(iio_context *ctx)
{
	std::string device_name = "";
	for (auto &devices_ini_file : devices_ini_file_path) {
		for (auto &iniconf : Utils::parseIniFile(devices_ini_file)) {
			device_name = iniconf.hw_name;
			auto device_list = Utils::valuesForIniConfigKey(iniconf, "compatible-devices");
			bool found = Utils::devicesFoundInContext(ctx, device_list);
			if (found) {
				return device_name;
			}
		}
	}
}
