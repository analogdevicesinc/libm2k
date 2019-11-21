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

#include <algorithm>
#include "command.h"
#include <libm2k/contextbuilder.hpp>

using namespace libm2k::cli;


Command::Command(int argc, char **argv)
{
	this->argc = argc;
	this->argv = argv;
	context = nullptr;
	bool help = false;
	if (argc < 3) {
		help = true;
		return;
	}
	for (int i = 2; i < argc; i++) {
		std::string argument(argv[i]);
		if (argument == "--help" || argument == "-h") {
			help = true;
			break;
		} else if (std::string(argv[i]) == "auto") {
			context = libm2k::contexts::m2kOpen();
			break;
		} else if (Validator::validateUri(argv[i])) {
			context = libm2k::contexts::m2kOpen(argv[i]);
			break;
		}
	}
	if (context == nullptr && !help) {
		throw std::runtime_error("Could not establish a connection to m2k. Please check the URI\n");
	}
}

libm2k::contexts::M2k *Command::getContext()
{
	return context;
}

std::vector<std::string> Command::getArguments()
{
	std::vector<std::string> arguments;
	if (optarg) {
		arguments.emplace_back(optarg);
		int i = 0;
		while (optind + i < argc && *argv[optind + i] != '-') {
			arguments.emplace_back(argv[optind + i]);
			i++;
		}
	}
	return arguments;
}

void
Command::addOutputMessage(std::vector<std::pair<std::string, std::string>> &output, const char *key, std::string value)
{
	std::pair<std::string, std::string> outputMessage;
	outputMessage.first = std::string(key);
	outputMessage.second = std::move(value);
	output.push_back(outputMessage);
}

unsigned int Command::getIndexOfElement(std::string element, const std::vector<const char *> &list)
{
	auto it = std::find(list.begin(), list.end(), element);
	if (it == list.end()) {
		return -1;
	}
	return distance(list.begin(), it);;
}
