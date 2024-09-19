/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * */


#include "analog_out_binary_raw.h"
#include <iostream>

AnalogOutBinaryRaw::AnalogOutBinaryRaw(libm2k::analog::M2kAnalogOut *analogOut, unsigned int bufferSize,
				       std::vector<unsigned int> &channels, bool cyclic) : AnalogOutGenerator(analogOut,
												     bufferSize,
												     channels, cyclic)
{
	this->samples = std::vector<short>();
}

void AnalogOutBinaryRaw::generate(bool &keepReading)
{
	getSamples(keepReading);
	if (samples.size() == bufferSize) {
		for (auto sample : samples) {
			std::cout << sample << std::endl;
		}
		if (channels.size() < 2) {
			analogOut->pushRaw(channels[0], samples);
		} else {
			analogOut->pushRawInterleaved(samples.data(), channels.size(), samples.size());
		}
		if (cyclic) {
			keepReading = false;
		} else {
			samples.clear();
		}
	}
}

void AnalogOutBinaryRaw::getSamples(bool &keepReading)
{
	int retVal = 0;
	std::vector<unsigned char> unprocessedSample(sizeof(short));
	retVal = std::fread(&unprocessedSample[0], 1, sizeof(short), stdin);
	if (!retVal) {
		keepReading = false;
		return;
	}

	uint16_t sample = 0;
	sample |= unprocessedSample[1];
	sample <<= (uint16_t) 8;
	sample |= unprocessedSample[0];
	samples.push_back(sample);
}
