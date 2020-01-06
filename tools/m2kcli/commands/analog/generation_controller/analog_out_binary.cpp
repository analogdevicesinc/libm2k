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

#include <cstring>
#include "analog_out_binary.h"
#include <iostream>

AnalogOutBinary::AnalogOutBinary(libm2k::analog::M2kAnalogOut *analogOut, int bufferSize, std::vector<int> &channels,
				 bool cyclic) : AnalogOutGenerator(analogOut, bufferSize, channels, cyclic)
{
	this->samples = std::vector<double>();
}

void AnalogOutBinary::generate(bool &keepReading)
{
	getSamples(keepReading);
	if (samples.size() == bufferSize) {
		if (channels.size() < 2) {
			analogOut->push(channels[0], samples);
		} else {
			analogOut->pushInterleaved(samples.data(), channels.size(), samples.size());
		}
		if (cyclic) {
			keepReading = false;
		} else {
			samples.clear();
		}
	}
}

void AnalogOutBinary::getSamples(bool &keepReading)
{
	int retVal = 0;
	std::vector<unsigned char> unprocessedSample(sizeof(double));

	retVal = std::fread(&unprocessedSample[0], 1, sizeof(double), stdin);
	if (!retVal) {
		keepReading = false;
		return;
	}
	double sample = 0;
	std::memcpy(&sample, &unprocessedSample[0], sizeof(double));
	samples.push_back(sample);
}

