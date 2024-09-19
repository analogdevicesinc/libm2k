/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software; you can redistribute it and/or modify
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

#include "digital_out_binary.h"

DigitalOutBinary::DigitalOutBinary(libm2k::digital::M2kDigital *digital, int bufferSize, std::vector<unsigned int> &channels,
				   bool cyclic) : DigitalOutGenerator(digital, bufferSize, channels, cyclic) {}

void DigitalOutBinary::getSamples(bool &keepReading)
{
	int retVal = 0;
	std::vector<unsigned char> unprocessedSample(sizeof(uint16_t));
	retVal = std::fread(&unprocessedSample[0], 1, sizeof(uint16_t), stdin);
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
