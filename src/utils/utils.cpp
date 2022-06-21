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

#include "libm2k/m2kexceptions.hpp"
#include "libm2k/utils/utils.hpp"
#include <regex>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <thread>
#include <chrono>
#include <cctype>
#include <sstream>
#include <locale>

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

			if ((deviceName.empty()) && (!section.empty())) {
				deviceName = section;
				device.hw_name = deviceName;
				continue;

			} else if ((!deviceName.empty()) && (!section.empty())) {
				/* Found a new section, save the current device struct
				 * and create another one */
				devs.push_back(device);
				deviceName = section;
				device.hw_name = deviceName;
				device.key_val_pairs = {};
				continue;

			} else if (!deviceName.empty()) {
				auto kv_pair = parseIniPair(line);
				if ((!kv_pair.first.empty()) && (!kv_pair.second.empty())) {
					device.key_val_pairs.push_back(kv_pair);
				}

			} else {
				THROW_M2K_EXCEPTION("Invalid configuration file: " + path, libm2k::EXC_INVALID_PARAMETER);
			}
		}
		if (!device.key_val_pairs.empty()) {
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
		for (unsigned int i = 0; i < values.size(); i++) {
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
		auto name = iio_device_get_name(dev);
		if (name) {
			device_list.emplace(std::string(name));
		} else {
			auto id = iio_device_get_id(dev);
			device_list.emplace(std::string(id));
		}
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
	std::string rev = "";

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

std::string Utils::getFirmwareVersion(struct iio_context *ctx)
{
	const char *hw_fw_version = iio_context_get_attr_value(ctx,
			"fw_version");

	if (!hw_fw_version) {
		hw_fw_version = "";
	}
	return hw_fw_version;
}

double Utils::average(double *data, size_t numElements)
{
	double sum = 0;

	for (size_t i = 0; i < numElements; i++)
		sum += data[i];

	return (sum / (double)numElements);
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

int Utils::compareVersions(std::string v1, std::string v2)
{
	while ((v1.find(".") != std::string::npos) ||
			(v2.find(".") != std::string::npos)) {
		if (v1.find(".") != std::string::npos) {
			v1.erase(v1.find("."), 1);
		}
		if (v2.find(".") != std::string::npos) {
			v2.erase(v2.find("."), 1);
		}
	}
	if (v1.size() < v2.size()) {
		std::string surplus("0", (v2.size() - v1.size()));
		v1.append(surplus);
	} else if (v2.size() < v1.size()) {
		std::string surplus("0", (v1.size() - v2.size()));
		v2.append(surplus);
	}

	return v1.compare(v2);
}

bool Utils::compareNatural(const std::string& a, const std::string& b){
	if (a == b) {
		return (a < b);
	} else if (a.empty()) {
		return true;
	} else if (b.empty()) {
		return false;
	} else if (std::isdigit(a[0]) && !std::isdigit(b[0])) {
		return true;
	} else if (!std::isdigit(a[0]) && std::isdigit(b[0])) {
		return false;
	} else if (!std::isdigit(a[0]) && !std::isdigit(b[0])) {
		if (a[0] == b[0]) {
			return compareNatural(a.substr(1), b.substr(1));
		}
		return (a < b);
	}

	std::istringstream string_stream_a(a);
	std::istringstream string_stream_b(b);
	int int_a, int_b;
	std::string a_new, b_new;

	string_stream_a >> int_a;
	string_stream_b >> int_b;
	if (int_a != int_b) {
		return (int_a < int_b);
	}

	std::getline(string_stream_a, a_new);
	std::getline(string_stream_b, b_new);
	return (compareNatural(a_new, b_new));
}

double Utils::safeStod(const std::string& to_convert)
{
	double converted_value = 0.0;
	std::istringstream in_s(to_convert);
	in_s.imbue(std::locale("C"));
	in_s >> converted_value;
	return converted_value;
}
