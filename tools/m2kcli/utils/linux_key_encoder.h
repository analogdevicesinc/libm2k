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

#ifndef LIBM2K_LINUX_KEY_ENCODER_H
#define LIBM2K_LINUX_KEY_ENCODER_H

#include <vector>
#include <cstdint>
#include <map>

class LinuxKeyEncoder {
public:
	LinuxKeyEncoder() = default;

	~LinuxKeyEncoder() = default;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	static std::vector<uint8_t> getLinuxCode(uint8_t windowsKeyCode);

	static bool isExitCode(uint8_t windowsKeyCode);
#else
	static std::vector<uint8_t> getLinuxCode(uint8_t linuxKeyCode);
#endif

private:
	static const std::map<uint8_t, std::vector<uint8_t>> decoder;

};

#endif
