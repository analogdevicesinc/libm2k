/*
 * Copyright 2019 Analog Devices, Inc.
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
 */


#include "linux_key_encoder.h"
#include <stdexcept>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
std::vector<uint8_t> LinuxKeyEncoder::getLinuxCode(uint8_t windowsKeyCode)
{
	if (decoder.find(windowsKeyCode) == decoder.end()) {
		throw std::runtime_error("No such key");
	}
	return decoder.at(windowsKeyCode);
}

bool LinuxKeyEncoder::isExitCode(uint8_t windowsKeyCode)
{
	return (windowsKeyCode == 3 || windowsKeyCode == 26 || windowsKeyCode == 28);
}

#else
std::vector<uint8_t> LinuxKeyEncoder::getLinuxCode(uint8_t linuxKeyCode)
{
	for (auto it = decoder.begin(); it != decoder.end(); ++it)
		if (it->second[2] == linuxKeyCode)
			return it->second;
	throw std::runtime_error("No such key");
}
#endif

const std::map<uint8_t, std::vector<uint8_t>> LinuxKeyEncoder::decoder = {
	{72, {'\033', 91, 65}}, // up
	{75, {'\033', 91, 68}}, // left
	{77, {'\033', 91, 67}}, // right
	{80, {'\033', 91, 66}}, // down
	{71, {'\033', 91, 72}}, //home
	{79, {'\033', 91, 70}}, //end
	{83, {'\033', 91, 51, 126}}, //del
	{82, {'\033', 91, 50, 126}}, // insert
	{73, {'\033', 91, 53, 126}}, // pg up
	{81, {'\033', 91, 54, 126}}, // pg down
};
