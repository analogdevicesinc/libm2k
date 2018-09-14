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
#include <iio.h>
#include <iostream>

using namespace m2k;
using namespace std;

GenericDevice::GenericDevice(std::string uri)
{
	std::cout << "Creating device + ctx for "<< uri << "\n";
	m_ctx = iio_create_context_from_uri(uri.c_str());
	if (m_ctx) {
		std::cout << "all good \n";
		const char *name = new char[100];
		const char *value = new char[100];
		iio_context_get_attr(m_ctx, 4, &name, &value);
		std::cout << name << " " <<  value <<  "\n";
	}
	m_uri = uri;

}

GenericDevice::~GenericDevice()
{
	iio_context_destroy(m_ctx);
	std::cout << "Destroying device + ctx for " << m_uri << "\n";
}

