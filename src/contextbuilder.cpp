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

#include <libm2k/m2k.hpp>
#include <libm2k/lidar.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>
#include <libm2k/config.hpp>
#include <iio.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>


using namespace libm2k::contexts;
using namespace libm2k::utils;

std::vector<Context*> ContextBuilder::s_connectedDevices = {};
std::map<ContextTypes, std::vector<std::string>> ContextBuilder::m_dev_map = {
	{ContextTypes::CtxFMCOMMS, {"cf-ad9361-lpc", "cf-ad9361-dds-core-lpc", "ad9361-phy"}},
	{ContextTypes::CtxM2K, {"m2k-adc", "m2k-dac-a",
				"m2k-dac-b", "m2k-logic-analyzer-rx",
				"m2k-logic-analyzer-tx", "m2k-logic-analyzer"}},
	{ContextTypes::CtxLIDAR, {"ad7091", "ltc2471", "xadc", "ad5627", "ad9528",
				  "7c700000.axi-pulse-capture", "axi-ad9094-hpc"}}
};

std::map<ContextTypes, std::string> ContextBuilder::m_dev_name_map = {
	{ContextTypes::CtxFMCOMMS, "FMMCOMMS"},
	{ContextTypes::CtxM2K, "M2K"},
	{ContextTypes::CtxLIDAR, "LIDAR"},
	{Other, "Generic"}
};

ContextBuilder::ContextBuilder()
{
}

ContextBuilder::~ContextBuilder()
{
}

std::vector<std::string> ContextBuilder::getAllContexts()
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

Context* ContextBuilder::buildContext(ContextTypes type, std::string uri,
			struct iio_context* ctx, bool sync) // enum Device Name
{
	std::string name = m_dev_name_map.at(type);
	switch (type) {
		case CtxM2K: return new M2k(uri, ctx, name, sync);
		case CtxLIDAR: return new Lidar(uri, ctx, name, sync);
		case Other:
		default:
		return new Context(uri, ctx, name, sync);
	}
}

Context* ContextBuilder::contextOpen(const char *uri)
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

	ContextTypes dev_type = ContextBuilder::identifyContext(ctx);

	Context* dev = buildContext(dev_type, std::string(uri), ctx, true);
	s_connectedDevices.push_back(dev);

	return dev;
}

Context* ContextBuilder::contextOpen(struct iio_context* ctx, const char* uri)
{
	for (Context* dev : s_connectedDevices) {
		if (dev->getUri() == std::string(uri)) {
			return dev;
		}
	}

	if (!ctx) {
		return nullptr;
	}

	ContextTypes dev_type = ContextBuilder::identifyContext(ctx);

	Context* dev = buildContext(dev_type, std::string(uri), ctx, true);
	s_connectedDevices.push_back(dev);

	return dev;
}

/* Connect to the first usb device that was found
TODO: try to use the "local" context,
before trying the "usb" one. */
Context* ContextBuilder::contextOpen()
{
	auto lst = getAllContexts();
	if (lst.size() <= 0) {
		return nullptr;
	}
	return contextOpen(lst.at(0).c_str());
}

M2k *ContextBuilder::m2kOpen(struct iio_context* ctx, const char *uri)
{
	auto dev = contextOpen(ctx, uri);
	if (!dev) {
		return nullptr;
	}

	auto m2k = dev->toM2k();
	if (m2k) {
		return m2k;
	}
	return nullptr;
}

M2k *ContextBuilder::m2kOpen(const char *uri)
{
	auto dev = contextOpen(uri);
	if (!dev) {
		return nullptr;
	}

	auto m2k = dev->toM2k();
	if (m2k) {
		return m2k;
	}
	return nullptr;
}

M2k *ContextBuilder::m2kOpen()
{
	auto dev = contextOpen();
	if (!dev) {
		return nullptr;
	}

	auto m2k = dev->toM2k();
	if (m2k) {
		return m2k;
	}
	return nullptr;
}

void ContextBuilder::contextClose(Context* device, bool deinit)
{
	if (deinit) {
		device->deinitialize();
	}

	s_connectedDevices.erase(std::remove(s_connectedDevices.begin(),
					     s_connectedDevices.end(),
					     device), s_connectedDevices.end());
	delete device;
}

void ContextBuilder::contextCloseAll()
{
	while (s_connectedDevices.size() > 0) {
		contextClose(s_connectedDevices.at(0));
	}
}

std::string ContextBuilder::getVersion()
{
	return "v" + std::string(PROJECT_VERSION) +
		"-g" + std::string(PROJECT_VERSION_GIT);
}

ContextTypes ContextBuilder::identifyContext(iio_context *ctx)
{
	ContextTypes type = Other;
	for (auto dev : m_dev_map) {
		bool found = Utils::devicesFoundInContext(ctx, dev.second);
		if (found) {
			return dev.first;
		}
	}
	return type;
}

Context *libm2k::contexts::contextOpen()
{
	return ContextBuilder::contextOpen();
}

Context *libm2k::contexts::contextOpen(const char *uri)
{
	return ContextBuilder::contextOpen(uri);
}

Context *libm2k::contexts::contextOpen(struct iio_context *ctx, const char *uri)
{
	return ContextBuilder::contextOpen(ctx, uri);
}

M2k *libm2k::contexts::m2kOpen(const char *uri)
{
	return ContextBuilder::m2kOpen(uri);
}

M2k *libm2k::contexts::m2kOpen(struct iio_context *ctx, const char *uri)
{
	return ContextBuilder::m2kOpen(ctx, uri);
}

M2k *libm2k::contexts::m2kOpen()
{
	return ContextBuilder::m2kOpen();
}

std::vector<std::string> libm2k::contexts::getAllContexts()
{
	return ContextBuilder::getAllContexts();
}

void libm2k::contexts::contextClose(Context *ctx, bool deinit)
{
	ContextBuilder::contextClose(ctx, deinit);
}

void libm2k::contexts::contextCloseAll()
{
	ContextBuilder::contextCloseAll();
}

std::string libm2k::contexts::getVersion()
{
	return ContextBuilder::getVersion();
}
