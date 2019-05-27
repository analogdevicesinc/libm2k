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

#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>
#include <iio.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>


using namespace libm2k::devices;
using namespace libm2k::utils;

std::vector<Context*> ContextBuilder::s_connectedDevices = {};
std::map<DeviceTypes, std::vector<std::string>> ContextBuilder::m_dev_map = {
	{DeviceTypes::DevFMCOMMS, {"cf-ad9361-lpc", "cf-ad9361-dds-core-lpc", "ad9361-phy"}},
	{DeviceTypes::DevM2K, {"m2k-adc", "m2k-dac-a",
			       "m2k-dac-b", "m2k-logic-analyzer-rx",
			       "m2k-logic-analyzer-tx", "m2k-logic-analyzer"}}
};

std::map<DeviceTypes, std::string> ContextBuilder::m_dev_name_map = {
	{DeviceTypes::DevFMCOMMS, "FMMCOMMS"},
	{DeviceTypes::DevM2K, "M2K"},
	{Other, "Generic"}
};

ContextBuilder::ContextBuilder()// : m_pimpl(new M2KImpl())
{
	std::cout << "Constructor \n";
}

ContextBuilder::~ContextBuilder()
{
	std::cout << "Destructor \n";
}

std::vector<std::string> ContextBuilder::listDevices()
{
	struct iio_context_info **info;
	unsigned int nb_contexts;
	std::vector<std::string> uris;

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

Context* ContextBuilder::buildDevice(DeviceTypes type, std::string uri,
			struct iio_context* ctx) // enum Device Name
{
	std::string name = m_dev_name_map.at(type);
	switch (type) {
		case DevM2K: return new M2k(uri, ctx, name);

		case Other:
		default:
		return new Context(uri, ctx, name);
	}
}

Context* ContextBuilder::deviceOpen(const char *uri)
{
	for (Context* dev : s_connectedDevices) {
		if (dev->getUri() == std::string(uri)) {
			return dev;
		}
	}

	struct iio_context* ctx = iio_create_context_from_uri(uri);
	if (!ctx) {
		return nullptr;
	}
	std::cout << "creating IIO context\n";

	DeviceTypes dev_type = ContextBuilder::identifyDevice(ctx);
	std::cout << m_dev_name_map.at(dev_type) << std::endl;

	Context* dev = buildDevice(dev_type, std::string(uri), ctx);
	s_connectedDevices.push_back(dev);

	return dev;
}

/* Connect to the first usb device that was found
TODO: try to use the "local" context,
before trying the "usb" one. */
Context* ContextBuilder::deviceOpen()
{
	auto lst = listDevices();
	if (lst.size() <= 0) {
		return nullptr;
	}
	return deviceOpen(lst.at(0).c_str());
}

M2k *ContextBuilder::m2kOpen(const char *uri)
{
	auto dev = deviceOpen(uri);
	auto m2k = dev->toM2k();
	if (m2k) {
		return m2k;
	}
	return nullptr;
}

M2k *ContextBuilder::m2kOpen()
{
	auto dev = deviceOpen();
	auto m2k = dev->toM2k();
	if (m2k) {
		return m2k;
	}
	return nullptr;
}

void ContextBuilder::deviceClose(Context* device)
{
	s_connectedDevices.erase(std::remove(s_connectedDevices.begin(),
					     s_connectedDevices.end(),
					     device), s_connectedDevices.end());
	delete device;
}

void ContextBuilder::deviceCloseAll()
{
	while (s_connectedDevices.size() > 0) {
		deviceClose(s_connectedDevices.at(0));
	}
}

DeviceTypes ContextBuilder::identifyDevice(iio_context *ctx)
{
	DeviceTypes type = Other;
	for (auto dev : m_dev_map) {
		bool found = Utils::devicesFoundInContext(ctx, dev.second);
		if (found) {
			return dev.first;
		}
	}
	return type;
}
