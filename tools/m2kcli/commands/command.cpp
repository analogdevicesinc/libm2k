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

#include "command.h"
#include <libm2k/contextbuilder.hpp>

using namespace libm2k::cli;


Command::Command(int argc, char **argv)
{
	this->argc = argc;
	this->argv = argv;
	bool help = false;
	for (int i = 2; i < argc; i++) {
		std::string argument(argv[i]);
		if (argument == "--help" || argument == "-h") {
			context = nullptr;
			help = true;
			break;
		} else if (std::string(argv[i]) == "auto") {
			context = libm2k::devices::m2kOpen();
			break;
		} else if (Validator::validateUri(argv[i])) {
			context = libm2k::devices::m2kOpen(argv[i]);
			break;
		}
	}
	if (context == nullptr && !help) {
		throw std::runtime_error("Could not establish a connection to m2k. Please check the URI\n");
	}
}

std::vector<std::string> Command::getArguments()
{
	std::vector<std::string> arguments;
	arguments.emplace_back(optarg);
	int i = 0;
	while (optind + i < argc && *argv[optind + i] != '-') {
		arguments.emplace_back(argv[optind + i]);
		i++;
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

libm2k::devices::M2k *Command::getContext()
{
	return context;
}
