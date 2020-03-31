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
#include "commands/communication/uart.h"
#include "commands/communication/uart_terminal.h"
#include <libm2k/contextbuilder.hpp>
#include <csignal>
#include <thread>

static const char *const helpMessage = "Usage:\n"
				       "m2kcli [-h | --help] [-v | --version]\n"
				       "       <command> [<args>]\n"
				       "\n"
				       "Control the ADALM2000\n"
				       "\n"
				       "Optional arguments:\n"
				       "  -h, --help            show this help message and exit\n"
				       "  -v, --version         show the libm2k version and exit\n"
				       "  -s, --scan            retrieve all available USB URIs\n"
				       "  -i, --identify <uri>  identify the m2k based on its URI\n"
				       "\n"
				       "Commands:\n"
				       "  These commands represent the components of the ADALM2000\n"
				       "\n"
				       "    analog-in           control the analogical input component\n"
				       "    analog-out          control the analogical output component\n"
				       "    digital             control the digital component\n"
				       "    power-supply        control the power supply\n"
				       "    spi			control the functionality of spi\n"
				       "    i2c			control the functionality of i2c\n"
				       "    uart		control the functionality of uart\n"
				       "    uart-terminal	control the functionality of uart streaming\n";

libm2k::cli::Command *command;
void destroy(int sigNumber);

int main(int argc, char **argv)
{
	std::signal(SIGABRT, destroy);
	std::signal(SIGINT, destroy);
	std::signal(SIGTERM, destroy);
	std::signal(SIGSEGV, destroy);
#if !defined(_WIN32) && !defined(__CYGWIN__)
	std::signal(SIGHUP, destroy);
	std::signal(SIGPIPE, destroy);
#endif

	bool quiet;
	std::vector<std::pair<std::string, std::string>> output;
	try {
		if (argc < 2 || std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
			std::cout << helpMessage;
			return 0;
		}

		if (argc < 2 || std::string(argv[1]) == "--version" || std::string(argv[1]) == "-v") {
			std::cout << libm2k::context::getVersion() << std::endl;
			return 0;
		}

		if (argc < 2 || std::string(argv[1]) == "--scan" || std::string(argv[1]) == "-s") {
			auto contexts = libm2k::context::getAllContexts();
			for (auto ctx : contexts) {
				std::cout << ctx << std::endl;
			}
			return 0;
		}

		if (argc < 2 || std::string(argv[1]) == "--identify" || std::string(argv[1]) == "-i") {
			if (argc < 3) {
				throw std::runtime_error("Please provide an URI\n");
			}
			libm2k::context::M2k *context = libm2k::context::m2kOpen(argv[2]);
			if (context == nullptr) {
				throw std::runtime_error("Could not establish a connection to m2k. Please check the URI\n");
			}

			for (unsigned int i = 0; i < 15; ++i) {
				context->setLed(true);
				std::this_thread::sleep_for (std::chrono::milliseconds(100));
				context->setLed(false);
				std::this_thread::sleep_for (std::chrono::milliseconds(100));
			}
			context->setLed(true);
			libm2k::context::contextClose(context, false);
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
		} else if (std::string(argv[1]) == "uart") {
			libm2k::cli::Uart uart(argc, argv);
			command = &uart;
		} else if (std::string(argv[1]) == "uart-terminal") {
			libm2k::cli::UartTerminal uartTerminal(argc, argv);
			command = &uartTerminal;
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
		libm2k::context::contextClose(command->getContext(), false);
	}
	return 0;
}

void destroy(int sigNumber)
{
	libm2k::context::contextClose(command->getContext(), false);
	std::cout << std::endl;
	exit(sigNumber);
}
