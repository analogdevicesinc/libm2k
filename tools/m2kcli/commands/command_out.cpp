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

#include <fstream>
#include <iterator>
#include <algorithm>
#include <sstream>
#include <cstring>
#include "command_out.h"

using namespace libm2k::cli;

void CommandOut::getSamplesCsvFormat(std::string &file, std::vector<uint16_t> &samples)
{
	if (!file.empty()) {
		std::ifstream in(file);
		samples = std::vector<uint16_t>((std::istream_iterator<uint16_t>(in)),
						std::istream_iterator<uint16_t>());
	} else {
		samples = std::vector<uint16_t>(std::istream_iterator<uint16_t>(std::cin), {});
	}
}

void CommandOut::getSamplesCsvFormat(std::string &file, std::vector<std::vector<double>> &samples)
{
	std::istream *in;
	std::ifstream f;
	if (!file.empty()) {
		f.open(file);
		in = &f;
	} else {
		in = &std::cin;
	}
	std::string value;
	for (std::string line; std::getline(*in, line);) {
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
}

void CommandOut::getSamplesCsvFormat(std::string &file, std::vector<std::vector<short>> &samples)
{
	std::istream *in;
	std::ifstream f;
	if (!file.empty()) {
		f.open(file);
		in = &f;
	} else {
		in = &std::cin;
	}
	std::string value;
	for (std::string line; std::getline(*in, line);) {
		std::stringstream s(line);
		line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
		std::replace(line.begin(), line.end(), ';', ',');
		for (auto &i : samples) {
			if (getline(s, value, ',')) {
				uint16_t sample;
				Validator::validate(value, sample);
				i.push_back(sample);
			} else {
				i.push_back(0);
			}
		}
	}
}

void CommandOut::getSamplesBinaryFormat(std::string &file, std::vector<uint16_t> &samples)
{
	std::vector<unsigned char> unprocessedSamples;
	if (!file.empty()) {
		std::ifstream in(file);
		unprocessedSamples = std::vector<unsigned char>(
			(std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	} else {
		unprocessedSamples = std::vector<unsigned char>(
			(std::istreambuf_iterator<char>(std::cin)),
			std::istreambuf_iterator<char>());
	}
	for (auto it = unprocessedSamples.begin(); it != unprocessedSamples.end(); it += 2) {
		uint16_t sample = 0;
		sample |= *(it + 1);
		sample = sample << (uint16_t) 8;
		sample |= *(it);
		samples.push_back(sample);
	}
}

void CommandOut::getSamplesBinaryFormat(std::string &file, std::vector<double> &samples)
{
	std::vector<unsigned char> unprocessedSamples;
	if (!file.empty()) {
		std::ifstream in(file);
		unprocessedSamples = std::vector<unsigned char>(
			(std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	} else {
		unprocessedSamples = std::vector<unsigned char>(
			(std::istreambuf_iterator<char>(std::cin)),
			std::istreambuf_iterator<char>());
	}
	for (unsigned long it = 0; it < unprocessedSamples.size(); it += sizeof(double)) {
		double sample = 0;
		std::memcpy(&sample, &unprocessedSamples[it], sizeof(double));
		samples.push_back(sample);
	}
}
