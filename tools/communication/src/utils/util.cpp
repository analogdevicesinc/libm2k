/*
 * Copyright 2019 Analog Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
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
	if (frequency >= maxSampleRate) {
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

bool getAverageValue(std::vector<unsigned short> &samples, unsigned int &start, unsigned int numberOfSamples,
		     unsigned int bitIndex)
{
	unsigned short sum = 0;
	for (unsigned int i = 0; i < numberOfSamples; ++i, ++start) {
		if (getBit(samples[start], bitIndex)) {
			sum++;
		}
	}
	unsigned int i = 1;
	unsigned short tempSum = sum;
	bool previousSample = getBit(samples[start], bitIndex);
	for(; i < numberOfSamples - 1; ++i) {
		bool currentSample = getBit(samples[start + i], bitIndex);
		if (previousSample != currentSample) {
			break;
		}
		if(currentSample) {
			tempSum++;
		}
		previousSample = currentSample;
	}
	if (i < numberOfSamples - 1) {
		sum = tempSum;
		start += i;
		numberOfSamples += i;
	}
	return sum > numberOfSamples / 2;
}

void setBit(unsigned short &number, unsigned int index)
{
	number |= (1u << index);
}

void setBit(char &number, unsigned int index)
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
