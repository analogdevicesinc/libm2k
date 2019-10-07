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

#include <iostream>
#include "commands/command.h"
#include "commands/analog/analog_in.h"
#include "commands/analog/analog_out.h"
#include "commands/digital/digital.h"
#include "commands/analog/power_supply.h"
#include <libm2k/contextbuilder.hpp>

static const char *const helpMessage = "Usage:\n"
				       "m2kcli [-h | --help]\n"
				       "       <command> [<args>]\n"
				       "\n"
				       "Control the ADALM2000\n"
				       "\n"
				       "Optional arguments:\n"
				       "  -h, --help            show this help message and exit\n"
				       "\n"
				       "Commands:\n"
				       "  These commands represent the components of the ADALM2000\n"
				       "\n"
				       "    analog-in           control the analogical input component\n"
				       "    analog-out          control the analogical output component\n"
				       "    digital             control the digital component\n"
				       "    power-supply        control the power supply\n";

int main(int argc, char **argv)
{
	bool quiet;
	std::vector<std::pair<std::string, std::string>> output;
	libm2k::cli::Command *command;
	try {
		if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
			std::cout << helpMessage;
			return 0;
		}

		if (std::string(argv[1]) == "analog-in") {
			command = new libm2k::cli::AnalogIn(argc, argv);
		} else if (std::string(argv[1]) == "analog-out") {
			command = new libm2k::cli::AnalogOut(argc, argv);
		} else if (std::string(argv[1]) == "digital") {
			command = new libm2k::cli::Digital(argc, argv);
		} else if (std::string(argv[1]) == "power-supply") {
			command = new libm2k::cli::PowerSupply(argc, argv);
		} else {
			throw std::runtime_error("Invalid command.\n");
		}
		quiet = command->parseArguments(output);
	}
	catch (std::runtime_error &e) {
		if (std::string(e.what()).find('\n') != std::string::npos) {
			std::cout << e.what();
		} else {
			std::cout << e.what() << std::endl;
		}
		return -1;
	}
	catch (std::exception &e) {
		if (std::string(e.what()).find('\n') != std::string::npos) {
			std::cout << e.what();
		} else {
			std::cout << e.what() << std::endl;
		}
		return -1;
	}

	if (quiet) {
		for (auto &it : output) {
			std::cout << it.second << std::endl;
		}
	} else {
		for (auto &it : output) {
			std::cout << it.first << " : " << it.second << std::endl;
		}
	}
	if (command->getContext() != nullptr) {
		libm2k::devices::deviceClose(command->getContext(), true);
	}
	return 0;
}