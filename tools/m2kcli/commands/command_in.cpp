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
 
#include <iomanip>
#include "command_in.h"

using namespace libm2k::cli;

void CommandIn::printSamplesCsvFormat(std::vector<uint16_t> &samples, unsigned int nb_samples)
{
	for (unsigned i = 0; i < nb_samples; ++i) {
		std::cout << samples.at(i) << std::endl;
	}
}

void CommandIn::printSamplesCsvFormat(std::vector<std::vector<double>> &samples, unsigned int nb_samples,
				      std::vector<unsigned int> &channels)
{
	unsigned int nbChannels = channels.size() - 1;
	for (unsigned long i = 0; i < nb_samples; i++) {
		for (unsigned int j = 0; j < nbChannels; j++) {
			std::cout << std::setprecision(16) << samples[channels[j]][i] << ',';
		}
		std::cout << std::setprecision(16) << samples[channels[nbChannels]][i] << std::endl;
	}
}

void CommandIn::printRawSamplesBinaryFormat(std::vector<std::vector<double>> &samples, unsigned int nb_samples,
					    std::vector<unsigned int> &channels)
{
	char *data;
	if (channels.size() < 2) {
		std::vector<short> tempSamples(samples[channels[0]].begin(), samples[channels[0]].end());
		data = reinterpret_cast<char *>(tempSamples.data());
	} else {
		std::vector<short> interleavedSamples;
		for (unsigned int i = 0; i < samples[0].size(); ++i) {
			interleavedSamples.push_back((short) samples[0].at(i));
			interleavedSamples.push_back((short) samples[1].at(i));
		}
		for (auto sample : interleavedSamples) {
			std::cout << sample << std::endl;
		}
		data = reinterpret_cast<char *>(interleavedSamples.data());
	}

	for (unsigned long i = 0; i < nb_samples * channels.size() * sizeof(short); ++i) {
		std::cout << data[i];
	}
}

void CommandIn::printSamplesBinaryFormat(std::vector<std::vector<double>> &samples, unsigned int nb_samples,
					 std::vector<unsigned int> &channels)
{
	char *data;
	if (channels.size() < 2) {
		data = reinterpret_cast<char *>(samples[channels[0]].data());
	} else {
		std::vector<double> tempSamples;
		for (unsigned int i = 0; i < samples[0].size(); ++i) {
			tempSamples.push_back(samples[0].at(i));
			tempSamples.push_back(samples[1].at(i));
		}
		data = reinterpret_cast<char *>(tempSamples.data());
	}

	for (unsigned long i = 0; i < nb_samples * channels.size() * sizeof(double); ++i) {
		std::cout << data[i];
	}
}

void CommandIn::printSamplesBinaryFormat(std::vector<uint16_t> &samples, unsigned int nb_samples)
{
	char *data = reinterpret_cast<char *>(samples.data());
	for (unsigned long i = 0; i < nb_samples * sizeof(uint16_t); ++i) {
		std::cout << data[i];
	}
}
