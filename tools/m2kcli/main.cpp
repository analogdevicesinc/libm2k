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

#include <iostream>
#include "commands/command.h"
#include "commands/analog/analog_in.h"
#include "commands/analog/analog_out.h"
#include "commands/digital/digital.h"
#include "commands/analog/power_supply.h"
#include "commands/communication/spi.h"
#include "commands/communication/i2c.h"
#include <libm2k/contextbuilder.hpp>

static const char *const helpMessage = "Usage:\n"
				       "m2kcli [-h | --help] [-v | --version]\n"
				       "       <command> [<args>]\n"
				       "\n"
				       "Control the ADALM2000\n"
				       "\n"
				       "Optional arguments:\n"
				       "  -h, --help            show this help message and exit\n"
				       "  -v, --version         show the libm2k version and exit\n"
				       "\n"
				       "Commands:\n"
				       "  These commands represent the components of the ADALM2000\n"
				       "\n"
				       "    analog-in           control the analogical input component\n"
				       "    analog-out          control the analogical output component\n"
				       "    digital             control the digital component\n"
				       "    power-supply        control the power supply\n"
				       "    spi			control the functionality of spi\n"
				       "    i2c			control the functionality of i2c\n";

int main(int argc, char **argv)
{
	bool quiet;
	std::vector<std::pair<std::string, std::string>> output;
	libm2k::cli::Command *command;
	try {
		if (argc < 2 || std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
			std::cout << helpMessage;
			return 0;
		}

		if (argc < 2 || std::string(argv[1]) == "--version" || std::string(argv[1]) == "-v") {
			std::cout << libm2k::contexts::getVersion() << std::endl;
			return 0;
		}

		if (std::string(argv[1]) == "analog-in") {
			libm2k::cli::AnalogIn analogIn(argc, argv);
			command = &analogIn;
		} else if (std::string(argv[1]) == "analog-out") {
			libm2k::cli::AnalogOut analogOut(argc, argv);
			command = &analogOut;
		} else if (std::string(argv[1]) == "digital") {
			libm2k::cli::Digital digital(argc, argv);
			command = &digital;
		} else if (std::string(argv[1]) == "power-supply") {
			libm2k::cli::PowerSupply powerSupply(argc, argv);
			command = &powerSupply;
		} else if (std::string(argv[1]) == "spi") {
			libm2k::cli::Spi spi(argc, argv);
			command = &spi;
		} else if (std::string(argv[1]) == "i2c") {
			libm2k::cli::I2c i2c(argc, argv);
			command = &i2c;
		} else {
			throw std::runtime_error("m2kcli: '" + std::string(argv[1]) +
						 "' is not a m2kcli command. See 'm2kcli --help'.\n");
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
		libm2k::contexts::contextClose(command->getContext(), true);
	}
	return 0;
}
