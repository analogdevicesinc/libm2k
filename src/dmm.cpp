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

#include "libm2k/dmm.hpp"
#include "libm2k/m2kexceptions.hpp"
#include <iio.h>
#include <iostream>

using namespace libm2k::analog;

DMM::DMM(struct iio_context *ctx, std::string dev)
{
	m_ctx = ctx;
	if (m_ctx) {
		m_dev  = iio_context_find_device(m_ctx, dev.c_str());
	}

	if (!m_dev) {
		m_dev = nullptr;
		throw no_device_exception("DMM device not found");
	}

	m_dev_name = dev;
	m_nb_channels = iio_device_get_channels_count(m_dev);

	for (unsigned int i = 0; i < m_nb_channels; i++) {
		struct iio_channel* chn = iio_device_get_channel(m_dev, i);
		if (isValidDmmChannel(chn)) {
			std::string id = std::string(iio_channel_get_id(chn));
			m_channel_list.push_back(chn);
		}
	}

	if (m_channel_list.size() != m_nb_channels) {
		m_nb_channels = m_channel_list.size();
	}
}

DMM::~DMM()
{

}

bool DMM::isValidDmmChannel(struct iio_channel* chn)
{
	if (iio_channel_is_output(chn)) {
		return false;
	}

	if (iio_channel_find_attr(chn, "raw")  != NULL ||
			iio_channel_find_attr(chn, "input")  != NULL ||
			iio_channel_find_attr(chn, "processed") != NULL) {

		return true;
	}
	return false;
}

double DMM::getAttrValue(struct iio_channel *chn, std::string attr)
{
	double value = 0;
	unsigned int ret = iio_channel_attr_read_double(
				chn, attr.c_str(), &value);
	if (ret < 0) {
		throw invalid_parameter_exception("Cannot read " + attr);
	}
	return value;
}

DMM::dmm_reading DMM::readChannel(struct iio_channel *chn)
{
	std::string chn_name = iio_channel_get_id(chn);
	try {
		return readChannel(chn_name);
	} catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

struct iio_channel* DMM::getChannel(std::string chn_name)
{
	for (struct iio_channel* chn : m_channel_list) {
		if (iio_channel_get_id(chn) == chn_name) {
			return chn;
		}
	}
	throw invalid_parameter_exception("No channel with this id");
}

std::vector<std::string> DMM::getAllChannels()
{
	std::vector<std::string> channel_names = {};
	for (struct iio_channel* chn : m_channel_list) {
		channel_names.push_back(iio_channel_get_id(chn));
	}
	return channel_names;
}

DMM::dmm_reading DMM::readChannel(std::string chn_name)
{
	dmm_reading result;
	double value = 0;
	std::string key = "";
	struct iio_channel *chn = iio_device_find_channel(m_dev, chn_name.c_str(), false);
	try {
		if (iio_channel_find_attr(chn, "raw") != NULL) {
			value = getAttrValue(chn, "raw");
		} else if (iio_channel_find_attr(chn, "processed") != NULL) {
			value = getAttrValue(chn, "processed");
		} else if (iio_channel_find_attr(chn, "input") != NULL) {
			value = getAttrValue(chn, "input");
		} else {
			throw invalid_parameter_exception("Cannot read channel");
		}

		if (iio_channel_find_attr(chn, "offset") != NULL) {
			value += getAttrValue(chn, "offset");
		}

		if (iio_channel_find_attr(chn, "scale") != NULL) {
			value *= getAttrValue(chn, "scale");
		}

		if (chn_name.find("voltage") != std::string::npos) {
			key = " Volts\n";
			value = value / 1000;
		} else if (chn_name.find("temp") != std::string::npos) {
			key = " \xB0\C\n";
			value = value / 1000;
		} else if (chn_name.find("current") != std::string::npos) {
			key = " Milliampere\n";
		} else if (chn_name.find("accel") != std::string::npos) {
			key = " m/s²\n";
		} else if (chn_name.find("anglvel") != std::string::npos) {
			key = " rad/s\n";
		} else if (chn_name.find("pressure") != std::string::npos) {
			key = " kPa\n";
		} else if (chn_name.find("magn") != std::string::npos) {
			key = " Gauss\n";
		} else {
			key = " \n";
		}

		result.name = chn_name;
		result.unit = key;
		result.value = value;
		return result;

	} catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

std::vector<DMM::dmm_reading> DMM::readAll()
{
	std::vector<dmm_reading> result = {};
	try {
		for (struct iio_channel* chn : m_channel_list) {
			dmm_reading res = readChannel(chn);
			result.push_back(res);
		}
		return result;
	} catch (std::runtime_error& e) {
		throw std::invalid_argument("Cannot read dmm " + m_dev_name);
	}
}

std::string DMM::getDeviceName()
{
	return m_dev_name;
}


