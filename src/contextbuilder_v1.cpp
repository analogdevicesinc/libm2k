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

#include "m2k_impl.hpp"
#include "generic_impl.hpp"
#include <libm2k/contextbuilder.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/logger.hpp>
#include <libm2k/utils/utils.hpp>
#include <libm2k/version.hpp>
#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <regex>

#ifndef LIBIIO_V1
#include <iio.h>
#else
#include <iio/iio.h>
#endif


using namespace libm2k::context;
using namespace libm2k::utils;

std::vector<Context*> ContextBuilder::s_connectedDevices = {};
std::map<ContextTypes, std::vector<std::string>> ContextBuilder::m_dev_map = {
	{ContextTypes::CtxFMCOMMS, {"cf-ad9361-lpc", "cf-ad9361-dds-core-lpc", "ad9361-phy"}},
	{ContextTypes::CtxM2K, {"m2k-adc", "m2k-dac-a",
				"m2k-dac-b", "m2k-logic-analyzer-rx",
				"m2k-logic-analyzer-tx", "m2k-logic-analyzer"}},
};

std::map<ContextTypes, std::string> ContextBuilder::m_dev_name_map = {
	{ContextTypes::CtxFMCOMMS, "FMMCOMMS"},
	{ContextTypes::CtxM2K, "M2K"},
	{Other, "Generic"}
};

bool ContextBuilder::m_disable_logging = true;
std::map<std::string, int> ContextBuilder::reference_count = {};

ContextBuilder::ContextBuilder()
{
}

ContextBuilder::~ContextBuilder()
{
}

std::vector<struct libm2k::CONTEXT_INFO*> ContextBuilder::getContextsInfo()
{
	struct iio_scan *scan_ctx;
	int err;
	size_t results;
	unsigned int nb_contexts;
	std::regex re(":| \\(Analog Devices Inc. |\\), serial=");
	const sregex_token_iterator end;
	std::vector<struct libm2k::CONTEXT_INFO*> contexts_info;

	scan_ctx = iio_scan(NULL, "usb");
	err = iio_err(scan_ctx);
	if (err) {
		std::cout << "Unable to create scan context!" << std::endl;
		return contexts_info;
	}

	results = iio_scan_get_results_count(scan_ctx);

	if (results == 0) {
		std::cout << "Unable to scan!" << std::endl;
		goto out_destroy_context;
	}

	nb_contexts = static_cast<unsigned int>(results);

	for (unsigned int i = 0; i < nb_contexts; i++)
	{
		std::string description = std::string(iio_scan_get_description(scan_ctx, i));
		std::sregex_token_iterator ptr(description.begin(), description.end(), re, -1);
		auto *ctx_info = new struct libm2k::CONTEXT_INFO();
		ctx_info->uri = std::string(iio_scan_get_uri(scan_ctx, i));
		ctx_info->manufacturer = "Analog Devices Inc.";
		if (ptr != end) {
			ctx_info->id_vendor = *ptr++;
		}
		if (ptr != end) {
			ctx_info->id_product = *ptr++;
		}
		if (ptr != end) {
			ctx_info->product = *ptr++;
		}
		if (ptr != end) {
			ctx_info->serial = *ptr++;
		}
		contexts_info.push_back(ctx_info);
	}

out_destroy_context:
	iio_scan_destroy(scan_ctx);
	return contexts_info;
}

std::vector<std::string> ContextBuilder::getAllContexts()
{
	std::vector<std::string> uris;

	auto contexts_info = getContextsInfo();
	uris.reserve(contexts_info.size());

	for (auto ctx_info: contexts_info) {
		uris.push_back(ctx_info->uri);
		delete ctx_info;
	}
	return uris;
}

Context* ContextBuilder::buildContext(ContextTypes type, std::string uri,
			struct iio_context* ctx, bool sync, bool ownsContext) // enum Device Name
{
	std::string name = m_dev_name_map.at(type);
	switch (type) {
		case CtxM2K:
		{
			auto m2k = new M2kImpl(uri, ctx, name, sync);
			m2k->setContextOwnership(ownsContext);
			return m2k;
		}
		case Other:
		default:
		{
			auto generic = new GenericImpl(uri, ctx, name, sync);
			generic->setContextOwnership(ownsContext);
			return generic;
		}
	}
}

void ContextBuilder::incrementReferenceCount(std::string uri)
{
	if (reference_count.find(uri) != reference_count.end()) {
		reference_count[uri]++;
	} else {
		reference_count.emplace(uri, 1);
	}
}

void ContextBuilder::decrementReferenceCount(std::string uri)
{
	if (reference_count.find(uri) != reference_count.end()) {
		reference_count[uri]--;
	}
}

bool ContextBuilder::checkLastReference(std::string uri)
{
	if (reference_count.find(uri) != reference_count.end()) {
		return (reference_count[uri] == 0);
	}
	return false;
}

Context* ContextBuilder::searchInConnectedDevices(std::string uri)
{
	for (auto dev : s_connectedDevices) {
		if (dev->getUri() == uri) {
			return dev;
		}
	}
	return nullptr;
}

Context* ContextBuilder::contextOpen(const char *uri)
{
	int err;
	Context* dev = nullptr;
	if (m_disable_logging) {
		enableLogging(false);
	}
	LIBM2K_LOG(INFO, "libm2k version: " + getVersion());
	// use saved device is possible
	dev = searchInConnectedDevices(uri);
	if (dev) {
		incrementReferenceCount(uri);
		return dev;
	}
	// create and save device during first call
	struct iio_context* ctx = iio_create_context(NULL, uri);
	err = iio_err(ctx);
	if (err) {
		return nullptr;
	}
	unsigned int ctx_major, ctx_minor;
	ctx_major = iio_context_get_version_major(ctx);
	ctx_minor = iio_context_get_version_minor(ctx);
//	iio_context_get_version(ctx, &ctx_major, &ctx_minor, ctx_git_tag);
	LIBM2K_LOG(INFO, "libiio version: " + to_string(ctx_major) + "." +
			 to_string(ctx_minor));

	const struct iio_attr *attr_libusb = iio_context_find_attr(ctx, "usb,libusb");
	if (attr_libusb !=  NULL) {
		const char *libusb_version = iio_attr_get_static_value(attr_libusb);
		LIBM2K_LOG(INFO, "libusb version: " + std::string(libusb_version));
	}

	const struct iio_attr *attr_fw_version = iio_context_find_attr(ctx, "fw_version");
	if (attr_fw_version !=  NULL) {
		const char *hw_fw_version = iio_attr_get_static_value(attr_fw_version);
		LIBM2K_LOG(INFO, "Firmware version: " + std::string(hw_fw_version));
	}

	ContextTypes dev_type = ContextBuilder::identifyContext(ctx);

	dev = buildContext(dev_type, std::string(uri), ctx, true, true);
	s_connectedDevices.push_back(dev);
	incrementReferenceCount(uri);

	return dev;
}

Context* ContextBuilder::contextOpen(struct iio_context* ctx, const char* uri)
{
	Context* dev = nullptr;
	if (m_disable_logging) {
		enableLogging(false);
	}
	LIBM2K_LOG(INFO, "libm2k version: " + getVersion());

	unsigned int ctx_major, ctx_minor;
	ctx_major = iio_context_get_version_major(ctx);
	ctx_minor = iio_context_get_version_minor(ctx);
//	iio_context_get_version(ctx, &ctx_major, &ctx_minor, ctx_git_tag);
	LIBM2K_LOG(INFO, "libiio version: " + to_string(ctx_major) + "." +
			 to_string(ctx_minor));

	const struct iio_attr *attr_libusb = iio_context_find_attr(ctx, "usb,libusb");
	if (attr_libusb !=  NULL) {
		const char *libusb_version = iio_attr_get_static_value(attr_libusb);
		LIBM2K_LOG(INFO, "libusb version: " + std::string(libusb_version));
	}

	const struct iio_attr *attr_fw_version = iio_context_find_attr(ctx, "fw_version");
	if (attr_fw_version !=  NULL) {
		const char *hw_fw_version = iio_attr_get_static_value(attr_fw_version);
		LIBM2K_LOG(INFO, "Firmware version: " + std::string(hw_fw_version));
	}

	// use saved device is possible
	dev = searchInConnectedDevices(uri);
	if (dev) {
		incrementReferenceCount(uri);
		return dev;
	}

	if (!ctx) {
		return nullptr;
	}
	// create and save device during first call
	ContextTypes dev_type = ContextBuilder::identifyContext(ctx);

	dev = buildContext(dev_type, std::string(uri), ctx, true);
	s_connectedDevices.push_back(dev);
	incrementReferenceCount(uri);

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
	contextClose(dev);
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
	contextClose(dev);
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
	contextClose(dev);
	return nullptr;
}

void ContextBuilder::contextClose(Context* device, bool deinit)
{
	auto uri = device->getUri();
	if (searchInConnectedDevices(uri) == nullptr) {
		return;
	}
	decrementReferenceCount(uri);
	bool isLastReference = checkLastReference(uri);
	if (!isLastReference) {
		return;
	}

	reference_count.erase(uri);
	s_connectedDevices.erase(std::remove(s_connectedDevices.begin(),
					     s_connectedDevices.end(),
					     device), s_connectedDevices.end());
	try {
		if (deinit) {
			device->deinitialize();
		}
	} catch (std::exception &e ){
		delete device;
		THROW_M2K_EXCEPTION("Context deinit: " + std::string(e.what()), libm2k::EXC_RUNTIME_ERROR);
	}
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
        if (std::string(PROJECT_VERSION_GIT).empty())
                return "v" + std::string(PROJECT_VERSION);
        else
	        return "v" + std::string(PROJECT_VERSION) +
		       "-g" + std::string(PROJECT_VERSION_GIT);
}

void ContextBuilder::enableLogging(bool enable)
{
#ifdef LIBM2K_ENABLE_LOG
    if (enable) {
		FLAGS_minloglevel = 0;
		m_disable_logging = false;
	} else {
		FLAGS_minloglevel = 3;
		m_disable_logging = true;
	}
#else
    if (enable) {
		std::cout << "libm2k built without logging support\n";
    }
#endif
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

Context *libm2k::context::contextOpen()
{
	return ContextBuilder::contextOpen();
}

Context *libm2k::context::contextOpen(const char *uri)
{
	return ContextBuilder::contextOpen(uri);
}

Context *libm2k::context::contextOpen(struct iio_context *ctx, const char *uri)
{
	return ContextBuilder::contextOpen(ctx, uri);
}

M2k *libm2k::context::m2kOpen(const char *uri)
{
	return ContextBuilder::m2kOpen(uri);
}

M2k *libm2k::context::m2kOpen(struct iio_context *ctx, const char *uri)
{
	return ContextBuilder::m2kOpen(ctx, uri);
}

M2k *libm2k::context::m2kOpen()
{
	return ContextBuilder::m2kOpen();
}

std::vector<struct libm2k::CONTEXT_INFO*> libm2k::context::getContextsInfo()
{
	return ContextBuilder::getContextsInfo();
}

std::vector<std::string> libm2k::context::getAllContexts()
{
	return ContextBuilder::getAllContexts();
}

void libm2k::context::contextClose(Context *ctx, bool deinit)
{
	ContextBuilder::contextClose(ctx, deinit);
}

void libm2k::context::contextCloseAll()
{
	ContextBuilder::contextCloseAll();
}

std::string libm2k::context::getVersion()
{
	return ContextBuilder::getVersion();
}

void libm2k::context::enableLogging(bool enable)
{
	return ContextBuilder::enableLogging(enable);
}
