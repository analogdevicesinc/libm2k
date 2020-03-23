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

#include "analog_out_csv.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include "tools/m2kcli/utils/validator.h"

AnalogOutCSV::AnalogOutCSV(libm2k::analog::M2kAnalogOut *analogOut, unsigned int bufferSize, std::vector<unsigned int> &channels,
			   bool cyclic) : AnalogOutGenerator(analogOut, bufferSize, channels, cyclic)
{
	this->samples = std::vector<std::vector<double>>(2, std::vector<double>());
}

void AnalogOutCSV::generate(bool &keepReading)
{
	getSamples(keepReading);
	if (samples.at(0).size() == bufferSize) {
		if (channels.size() < 2) {
			analogOut->push(channels[0], samples[channels[0]]);
		} else {
			analogOut->push(samples);
		}
		if (cyclic) {
			keepReading = false;
		} else {
			samples.at(0).clear();
			samples.at(1).clear();
		}
	}
}

void AnalogOutCSV::getSamples(bool &keepReading)
{
	std::string value, line;
	if (!std::getline(std::cin, line)) {
		keepReading = false;
		return;
	}
	std::stringstream s(line);
	line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
	std::replace(line.begin(), line.end(), ';', ',');
	for (auto &i : samples) {
		if (getline(s, value, ',')) {
			double sample;
			Validator::validate(value, sample);
			i.push_back(sample);
		} else {
			i.push_back(0);
		}
	}
}
