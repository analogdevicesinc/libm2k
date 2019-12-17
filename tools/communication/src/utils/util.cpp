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

#include "util.h"
#include <libm2k/digital/m2kdigital.hpp>


void setOutputChannel(unsigned int channelIndex, libm2k::digital::M2kDigital *m2KDigital)
{
	m2KDigital->setDirection(channelIndex, libm2k::digital::DIO_OUTPUT);
	m2KDigital->enableChannel(channelIndex, true);
}

void setInputChannel(unsigned int channelIndex, libm2k::digital::M2kDigital *m2KDigital)
{
	m2KDigital->setDirection(channelIndex, libm2k::digital::DIO_INPUT);
	m2KDigital->enableChannel(channelIndex, true);
}

unsigned int getValidSampleRate(unsigned int frequency, unsigned int samplesPerCycle)
{
	unsigned int maxSampleRate = 100000000;
	if (frequency >= maxSampleRate || frequency < 0) {
		return maxSampleRate;
	}
	unsigned int tempSampleRate = frequency * samplesPerCycle;
	unsigned int sampleRateOut = maxSampleRate;
	unsigned int pow2 = 1, pow5;
	for (int i = 1; i <= 8; ++i) {
		pow2 *= 2;
		pow5 = 1;
		for (int j = 0; j <= 8; ++j) {
			unsigned int divisor = pow2 * pow5;

			unsigned int distance = (divisor > tempSampleRate) ? divisor - tempSampleRate : tempSampleRate -divisor;
			if (distance < maxSampleRate) {
				maxSampleRate = distance;
				sampleRateOut = divisor;
			}
			pow5 *= 5;
		}
	}
	return sampleRateOut;
}

void setBit(unsigned short &number, unsigned int index)
{
	number |= (1u << index);
}

bool getBit(unsigned short number, unsigned int index)
{
	return (number & (1u << index)) >> index;
}

void setBit(unsigned char &number, unsigned int index)
{
	number |= (1u << index);
}

void clearBit(unsigned char &number, unsigned int index)
{
	number &= ~(1u << index);
}

bool getBit(unsigned char number, unsigned int index)
{
	return (number & (1u << index)) >> index;
}
