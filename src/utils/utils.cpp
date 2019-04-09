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

#include "libm2k/utils/utils.hpp"
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

std::vector<ini_device_struct> Utils::parseIniFile(std::string path)
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
				throw_exception(EXC_INVALID_PARAMETER, "Invalid configuration file: " + path);
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
	size_t delim = line.find("=");
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

std::unordered_set<std::string> Utils::getAllDevices(iio_context *ctx)
{
	unsigned int nb_devices = iio_context_get_devices_count(ctx);
	std::unordered_set<std::string> device_list;
	for (unsigned int i = 0; i < nb_devices; ++i) {
		auto dev = iio_context_get_device(ctx, i);
		device_list.emplace(std::string(iio_device_get_name(dev)));
	}
	return device_list;
}

std::vector<string> Utils::valuesForIniConfigKey(const ini_device_struct &iniconf, const string &key)
{
	for (const auto &key_value_pair : iniconf.key_val_pairs) {
		if (key_value_pair.first == key) {
			return key_value_pair.second;
		}
	}
	return std::vector<std::string>();
}

bool Utils::devicesFoundInContext(iio_context *ctx, std::vector<string> device_list)
{
	auto context_devices = getAllDevices(ctx);
	bool device_found = true;
	for (const auto &device : device_list) {
		if (context_devices.find(device) == context_devices.end()) {
			device_found = false;
			break;
		}
	}
	return device_found;
}

std::string Utils::getHardwareRevision(struct iio_context *ctx)
{
	const char *hw_rev_attr_val = iio_context_get_attr_value(ctx,
			"hw_model");
	std::string rev;

	if (hw_rev_attr_val) {
		std::string const s = hw_rev_attr_val;
		std::string const key = "Rev.";
		int n = s.find(key);

		n += key.length();
		rev =  s.substr(n, 1);
	} else {
		rev = "A";
	}

	return rev;
}

double Utils::average(double *data, size_t numElements)
{
	double sum = 0;

	for (size_t i = 0; i < numElements; i++)
		sum += data[i];

	return (sum / (double)numElements);
}

std::vector<double> Utils::getAvailableSamplerates(iio_device *dev)
{
	std::vector<double> values;
	std::vector<std::string> str_values;
	char buf[1024];
	int ret;

	ret = iio_device_attr_read(dev, "sampling_frequency_available",
				   buf, sizeof(buf));

	if (ret > 0) {
		str_values = Utils::split(buf, " ");

		for (auto it : str_values) {
			__try {
				values.push_back(std::stod(it));
			} __catch (exception_type &e) {
				std::cout << "Not a valid samplerate " << e.what();
			}

		}
	}

	if (values.empty()) {
		ret = iio_device_attr_read(dev, "sampling_frequency",
					   buf, sizeof(buf));

		if (!ret) {
			__try {
				values.push_back(std::stoul(buf));
			} __catch (exception_type &e) {
				std::cout << "Not a valid samplerate " << e.what();
			}
		}
	}

	std::sort(values.begin(), values.end());
	return values;
}

DEVICE_DIRECTION Utils::getIioDeviceDirection(struct iio_device* dev)
{
	DEVICE_DIRECTION dir = NO_DIRECTION;

	unsigned int chn_count = iio_device_get_channels_count(dev);
	for (unsigned int i = 0; i < chn_count; i++) {
		auto chn = iio_device_get_channel(dev, i);
		if (iio_channel_is_output(chn)) {
			if (dir == INPUT) {
				dir = BOTH;
			} else if (dir != BOTH){
				dir = OUTPUT;
			}
		} else {
			if (dir == OUTPUT) {
				dir = BOTH;
			} else if (dir != BOTH){
				dir = INPUT;
			}
		}
	}
	return dir;
}
