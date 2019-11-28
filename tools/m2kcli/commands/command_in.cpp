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

#include <iomanip>
#include "command_in.h"

using namespace libm2k::cli;

void CommandIn::printSamplesCsvFormat(std::vector<uint16_t> &samples)
{
	for (unsigned short &sample : samples) {
		std::cout << sample << std::endl;
	}
}

void CommandIn::printSamplesCsvFormat(std::vector<std::vector<double>> &samples)
{
	int nbChannels = samples.size() - 1;
	for (unsigned long i = 0; i < samples[0].size(); i++) {
		for (int j = 0; j < nbChannels; j++) {
			std::cout << std::setprecision(16) << samples[j][i] << ',';
		}
		std::cout << std::setprecision(16) << samples[nbChannels][i] << std::endl;
	}
}

void CommandIn::printSamplesBinaryFormat(std::vector<uint16_t> &samples)
{
	char *data = reinterpret_cast<char *>(samples.data());
	for (unsigned long i = 0; i < samples.size() * 2; ++i) {
		std::cout << data[i];
	}
}

void CommandIn::printSamplesBinaryFormat(std::vector<double> &samples)
{
	char *data = reinterpret_cast<char *>(samples.data());
	for (unsigned long i = 0; i < samples.size() * sizeof(double); ++i) {
		std::cout << std::setprecision(16) << data[i];
	}
}
