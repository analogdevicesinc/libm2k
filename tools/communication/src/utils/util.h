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

#ifndef LIBM2K_UTIL_H
#define LIBM2K_UTIL_H

#include <libm2k/digital/m2kdigital.hpp>

void setOutputChannel(unsigned int channelIndex, libm2k::digital::M2kDigital *m2KDigital);

void setInputChannel(unsigned int channelIndex, libm2k::digital::M2kDigital *m2KDigital);

unsigned int getValidSampleRate(unsigned int frequency, unsigned int samplesPerCycle);

bool getAverageValue(std::vector<unsigned short> &samples, unsigned int &start, unsigned int numberOfSamples,
		     unsigned int bitIndex);

void setBit(unsigned short &number, unsigned int index);

void setBit(char &number, unsigned int index);

void setBit(unsigned char &number, unsigned int index);

bool getBit(unsigned short number, unsigned int index);

bool getBit(unsigned char number, unsigned int index);

void clearBit(unsigned char &number, unsigned int index);


#endif //LIBM2K_UTIL_H
