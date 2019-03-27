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
#include <libm2k/channel.hpp>
#include <iio.h>
#include <iostream>

using namespace libm2k::analog;

DMM::DMM(struct iio_context *ctx, std::string dev) :
	Device(ctx, dev, true)
{
	for (unsigned int i = 0; i < getNbChannels(); i++) {
		if (isValidDmmChannel(i)) {
			m_channel_id_list.insert(std::pair<std::string, unsigned int>
						    (getChannel(i)->getId(), i));
		}
	}
}

DMM::~DMM()
{

}

DMM::dmm_reading DMM::readChannel(unsigned int index)
{
	std::string chn_name = "";
	for (auto pair : m_channel_id_list) {
		if (pair.second == index) {
			chn_name = pair.first;
			break;
		}
	}
	try {
		return readChannel(chn_name);
	} catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

std::vector<std::string> DMM::getAllChannels()
{
	std::vector<std::string> ids = {};
	for (auto pair : m_channel_id_list) {
		ids.push_back(pair.first);
	}
	return ids;
}

DMM::dmm_reading DMM::readChannel(std::string chn_name)
{
	dmm_reading result;
	double value = 0;
	std::string key = "";
	unsigned int index = m_channel_id_list.at(chn_name);
	try {
		auto channel = getChannel(index);
		if (channel->hasAttribute("raw")) {
			value = channel->getDoubleValue("raw");
		} else if (channel->hasAttribute("processed")) {
			value = channel->getDoubleValue("processed");
		} else if (channel->hasAttribute("input")) {
			value = channel->getDoubleValue("input");
		} else {
			throw invalid_parameter_exception("DMM: Cannot read channel" + chn_name);
		}

		if (channel->hasAttribute("offset")) {
			value += channel->getDoubleValue("offset");
		}

		if (channel->hasAttribute("scale")) {
			value *= channel->getDoubleValue("scale");
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
		for (auto pair : m_channel_id_list) {
			dmm_reading res = readChannel(pair.first);
			result.push_back(res);
		}
		return result;
	} catch (std::runtime_error& e) {
		throw std::invalid_argument("Cannot read DMM: " + getName());
	}
}

