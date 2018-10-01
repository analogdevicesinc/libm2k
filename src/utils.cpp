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

#include "utils.hpp"
#include <iio.h>
#include <regex>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "libm2k/m2kexceptions.hpp"

#include <thread>
#include <chrono>
#include <iostream>

using namespace std;
using namespace libm2k::utils;


bool Utils::iioDevHasAttribute(iio_device* dev, std::string const& attr)
{
	unsigned int nb_attr = iio_device_get_attrs_count(dev);
	const char* attr_name;
	for (unsigned int i = 0; i < nb_attr; i++) {
		attr_name = iio_device_get_attr(dev, i);
		std::size_t found = std::string(attr_name).find(attr);
		if (found != std::string::npos) {
			return true;
		}
	}
	return false;
}

bool Utils::iioChannelHasAttribute(iio_channel* chn, std::string const& attr)
{
	unsigned int nb_attr = iio_channel_get_attrs_count(chn);
	const char* attr_name;
	for (unsigned int i = 0; i < nb_attr; i++) {
		attr_name = iio_channel_get_attr(chn, i);
		std::size_t found = std::string(attr_name).find(attr);
		if (found != std::string::npos) {
			return true;
		}
	}
	return false;
}


std::vector<std::string> Utils::getIioDevByGlobalAttrs(iio_context* ctx, std::vector<std::string> attr_list)
{
	iio_device* dev = nullptr;
	std::vector<std::string> dev_list;

	unsigned int nb_dev = iio_context_get_devices_count(ctx);
	for(unsigned int i_dev = 0; i_dev < nb_dev; i_dev++) {
		bool dev_match = true;
		dev = iio_context_get_device(ctx, i_dev);

		for(unsigned int i_attr = 0; i_attr < attr_list.size(); i_attr++) {
			if (!iioDevHasAttribute(dev, attr_list.at(i_attr))) {
				dev_match = false;
				break;
			}
		}

		if (dev_match) {
			dev_list.push_back(std::string(iio_device_get_name(dev)));
		}
	}

	return dev_list;
}

std::vector<std::pair<std::string, std::string>> Utils::getIioDevByChannelAttrs(
		iio_context* ctx, std::vector<std::string> attr_list)
{
	iio_device* dev = nullptr;
	iio_channel* chn = nullptr;
	std::vector<std::pair<std::string, std::string>> dev_chn_list;
	unsigned int nb_chn = 0;
	unsigned int nb_dev = iio_context_get_devices_count(ctx);

	for (unsigned int i_dev = 0; i_dev < nb_dev; i_dev++) {
		bool dev_match = true;
		dev = iio_context_get_device(ctx, i_dev);
		nb_chn = iio_device_get_channels_count(dev);

		for (unsigned int i_chn = 0; i_chn < nb_chn; i_chn++) {
			bool chn_match = true;
			chn = iio_device_get_channel(dev, i_chn);

			/* Check if the current channel has all the required attributes */
			for (unsigned int i_attr = 0; i_attr < attr_list.size(); i_attr++) {
				if (!iioChannelHasAttribute(chn, attr_list.at(i_attr))) {
					chn_match = false;
					break;
				}
			}

			if (chn_match) {
				auto d_name = iio_device_get_name(dev);
				auto c_name = iio_channel_get_id(chn);
				if (c_name && d_name) {
					dev_chn_list.push_back(make_pair(std::string(d_name),
									 std::string(c_name)));
				}
			}
		}

		/* Check if the device has all the required attributes as global attributes */
		for(unsigned int i_attr = 0; i_attr < attr_list.size(); i_attr++) {
			if (!iioDevHasAttribute(dev, attr_list.at(i_attr))) {
				dev_match = false;
				break;
			}
		}

		if (dev_match) {
			dev_chn_list.push_back(make_pair(std::string(iio_device_get_name(dev)), ""));
		}
	}
	return dev_chn_list;
}

bool Utils::isInputChannel(iio_context* ctx, std::string const& d_name, std::string const& c_name)
{
	iio_device* dev = iio_context_find_device(ctx, d_name.c_str());
	iio_channel* chn = nullptr;
	bool isInput = false;
	if (dev) {
		chn = iio_device_find_channel(dev, c_name.c_str(), false);
		if (chn) {
			isInput = true;
		} else {
			isInput = false;
		}
	}

	return isInput;
}

int Utils::getChannelUnit(iio_context* ctx, std::string const& d_name, std::string const& c_name)
{
	struct iio_device* dev = iio_context_find_device(ctx, d_name.c_str());
	struct iio_channel* chn;
	if (!dev) {
		return NO_UNIT;
	}

	chn = iio_device_find_channel(dev, c_name.c_str(), false);
	if (!chn) {
		return NO_UNIT;
	}

	if (iioChannelHasAttribute(chn, "offset") &&
			iioChannelHasAttribute(chn, "raw") &&
			iioChannelHasAttribute(chn, "scale")) {
		return DEGREES;
	}
	return VOLTS;
}

double Utils::getIioDevTemp(iio_context* ctx, const std::string& d_name)
{
	double temp = -273.15;

	struct iio_device *dev = iio_context_find_device(ctx, d_name.c_str());

	if (dev) {
		struct iio_channel *chn = iio_device_find_channel(dev, "temp0",
			false);
		if (chn) {
			double offset;
			double raw;
			double scale;

			iio_channel_attr_read_double(chn, "offset", &offset);
			iio_channel_attr_read_double(chn, "raw", &raw);
			iio_channel_attr_read_double(chn, "scale", &scale);

			temp = (raw + offset) * scale / 1000;
		}
	}

	return temp;
}

void Utils::blinkLed(iio_context *ctx, const double duration, bool blocking)
{
	struct iio_device *m2k_fabric = iio_context_find_device(ctx, "m2k-fabric");

	if (!m2k_fabric) {
		//throw
		return;
	}

	struct iio_channel *fabric_channel = iio_device_find_channel(m2k_fabric, "voltage4", true);

	if (!fabric_channel) {
		//throw
		return;
	}

	if (!iioChannelHasAttribute(fabric_channel, "done_led_overwrite_powerdown")) {
		//throw
		return;
	}

	std::cout << "blinking " << std::endl;
	//save current state of the LED
	bool currentValue;
	iio_channel_attr_read_bool(fabric_channel,
				   "done_led_overwrite_powerdown",
				   &currentValue);

	const double blinkInterval = 0.05;
	double remainingTime = duration;
	std::thread th([=](double remainingTime, double blinkInterval, iio_channel *iioch){
		while(remainingTime > 0) {
			bool value;
			iio_channel_attr_read_bool(fabric_channel,
						   "done_led_overwrite_powerdown",
						   &value);
			iio_channel_attr_write_bool(fabric_channel,
						   "done_led_overwrite_powerdown",
						   !value);
			remainingTime -= blinkInterval;
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}, remainingTime, blinkInterval, fabric_channel);

	if (blocking) {
		th.join();
	} else if (th.joinable()) {
		th.detach();
	}
	//restore the state of the LED
	iio_channel_attr_write_bool(fabric_channel,
				   "done_led_overwrite_powerdown",
				   currentValue);
}

std::vector<Utils::ini_device_struct> Utils::parseIniFile(std::string path)
{
	std::string line;
	std::ifstream configFile(path);
	std::string deviceName = "";
	struct ini_device_struct device;
	std::vector<struct ini_device_struct> devs = {};
	device.hw_name = "";
	device.key_val_pairs = {};

	if (configFile.is_open()) {
		while (getline(configFile, line)) {
			auto section = parseIniSection(line);

			if ((deviceName == "") && (section != "")) {
				deviceName = section;
				device.hw_name = deviceName;
				continue;

			} else if ((deviceName != "") && (section != "")) {
				/* Found a new section, save the current device struct
				 * and create another one */
				devs.push_back(device);
				deviceName = section;
				device.hw_name = deviceName;
				device.key_val_pairs = {};
				continue;

			} else if (deviceName != "") {
				auto kv_pair = parseIniPair(line);
				if ((kv_pair.first != "") && (kv_pair.second.size() != 0)) {
					device.key_val_pairs.push_back(kv_pair);
				}

			} else {
				throw invalid_parameter_exception(
					"Invalid configuration file: " + path);
			}
		}
		if (device.key_val_pairs.size() != 0) {
			devs.push_back(device);
		}
		configFile.close();
	}
	return devs;
}

std::string Utils::parseIniSection(std::string line)
{
	std::regex section_reg{R"(\[.+\])"};
	bool section_match = std::regex_match(line, section_reg);
	if (section_match) {
		return line.substr(1, line.size() - 2);
	}
	return "";
}


std::pair<std::string, std::vector<std::string>>
	Utils::parseIniPair(std::string line)
{
	std::string key = "";
	std::vector<std::string> values {};
	std:size_t delim = line.find("=");
	if (delim != std::string::npos) {
		key = line.substr(0, delim);
		line = line.substr(delim+1, line.size()-delim-1);
		line.erase(remove(line.begin(), line.end(), ' '), line.end());
		values = split(line, ",");
		for (int i = 0; i < values.size(); i++) {
			if (values.at(i) == "") {
				values.erase(values.begin() + i);
			}
		}
	}
	return std::pair<std::string, std::vector<std::string>>(key, values);
}

std::vector<std::string> Utils::split(std::string s, std::string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;
	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}
	res.push_back(s.substr(pos_start));
	return res;
}

std::vector<std::string> Utils::getAllDevices(iio_context *ctx)
{
	unsigned int nb_devices = iio_context_get_devices_count(ctx);
	std::vector<std::string> device_list;
	for (unsigned int i = 0; i < nb_devices; ++i) {
		auto dev = iio_context_get_device(ctx, i);
		device_list.push_back(std::string(iio_device_get_name(dev)));
	}
	return device_list;
}

//std::string Utils::getIioDevByPartialName(std::string dev)
//{
//	unsigned int nb_devices = iio_context_get_devices_count(m_ctx);
//	for (unsigned int i = 0; i < nb_devices; i++) {
//		auto dev = iio_context_get_device(m_ctx, i);
//		std::cout << iio_device_get_name(dev) << std::endl;
//	}
//}
