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

#include "libm2k/powersupply.hpp"
#include "libm2k/m2kexceptions.hpp"
#include <iio.h>
#include <iostream>

using namespace libm2k::analog;

PowerSupply::PowerSupply(struct iio_context* ctx, std::string write_dev,
			 std::string read_dev)
{
	m_ctx = ctx;
	if (!m_ctx) {
		throw no_device_exception("Invalid IIO context");
	}

	if (write_dev != "") {
		m_dev_write = iio_context_find_device(m_ctx, write_dev.c_str());
		if (!m_dev_write) {
			m_dev_write = nullptr;
			throw no_device_exception("No device was found for writing");
		}
	}

	if (read_dev != "") {
		m_dev_read = iio_context_find_device(m_ctx, read_dev.c_str());
		if (!m_dev_read) {
			m_dev_read = nullptr;
			throw no_device_exception("No device was found for reading");
		}
	}

	m_dev_read_name = read_dev;
	m_dev_write_name = write_dev;

	m_nb_read_channels = iio_device_get_channels_count(m_dev_read);
	m_nb_write_channels = iio_device_get_channels_count(m_dev_write);
}

PowerSupply::~PowerSupply()
{
	m_read_channels.clear();
	m_write_channels.clear();
}

struct iio_context* PowerSupply::getContext()
{
	return m_ctx;
}

struct iio_device* PowerSupply::getReadDevice()
{
	if (!m_dev_read) {
		throw no_device_exception("No such read device");
	}
	return m_dev_read;
}

struct iio_device* PowerSupply::getWriteDevice()
{
	if (!m_dev_write) {
		throw no_device_exception("No such write device");
	}
	return m_dev_write;
}
