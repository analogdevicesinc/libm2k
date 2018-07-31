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

#include "../include/libm2k/M2K.h"
#include "iio.h"
#include <iostream>

using namespace m2k;
using namespace std;


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

DeviceBuilder* DeviceBuilder::deviceOpen(const char *uri)
{
        auto ctx = iio_create_context_from_uri(uri);
        if (ctx) {
                std::cout << "all good \n";
                const char *name = new char[100];
                const char *value = new char[100];
                iio_context_get_attr(ctx, 4, &name, &value);
                std::cout << name << " " <<  value <<  "\n";
        }
        auto dev = new DeviceBuilder();
        return dev;
}

void DeviceBuilder::deviceClose(DeviceBuilder* device)
{
        std::cout << "Delete iio ctx \n";
}
