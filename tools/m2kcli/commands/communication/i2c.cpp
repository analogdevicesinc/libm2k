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

#include "i2c.h"
#include <libm2k/tools/i2c_extra.hpp>

using namespace libm2k::cli;

I2c::I2c(int argc, char **argv) : Command(argc, argv)
{
	i2cDesc = nullptr;
	writeOnly = false;
}

I2c::~I2c()
{
	if (i2cDesc != nullptr) {
		i2c_remove(i2cDesc);
	}
}

bool I2c::parseArguments(std::vector<std::pair<std::string, std::string>> &output)
{
	if (context == nullptr) {
		std::cout << helpMessage;
		return false;
	}
	int c, option_index = 0;
	while ((c = getopt_long(argc, argv, "hi:w:r:",
				options, &option_index)) != -1) {
		switch (c) {
			case 'h':
				std::cout << helpMessage;
				return false;
			case 'i':
				handleInit();
				break;
			case 'w':
				handleWrite();
				break;
			case 'r':
				handleRead();
				break;
			default:
				break;
		}
	}
	return false;
}

void I2c::handleInit()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("address") && arguments.count("scl") && arguments.count("sda") &&
	      arguments.count("frequency"))) {
		throw std::runtime_error("Expecting: frequency=<value> address=<value> scl=<index> sda=<index>\n");
	}

	int frequency;
	Validator::validate(arguments["frequency"], "frequency", frequency);

	int address;
	Validator::validate(arguments["address"], "address", address);

	int scl;
	Validator::validate(arguments["scl"], "scl", scl);

	int sda;
	Validator::validate(arguments["sda"], "sda", sda);

	if (arguments.count("write_only")) {
		Validator::validate(arguments["write_only"], "write_only", writeOnly);
	}

	m2k_i2c_init m2KI2CInit;
	m2KI2CInit.scl = scl;
	m2KI2CInit.sda = sda;
	m2KI2CInit.context = context;

	i2c_init_param i2CInitParam;
	i2CInitParam.max_speed_hz = frequency;
	i2CInitParam.slave_address = address;
	i2CInitParam.extra = (void *) &m2KI2CInit;

	uint32_t retVal;

	if (writeOnly) {
		retVal = i2c_init_write_only(&i2cDesc, &i2CInitParam);
	} else {
		retVal = i2c_init(&i2cDesc, &i2CInitParam);
	}

	if (retVal != 0) {
		throw std::runtime_error("Could not initiate I2C\n");
	}
}

void I2c::handleWrite()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("data") && arguments.count("option"))) {
		throw std::runtime_error("Expecting: data=<value>,... option=<value>\n");
	}

	std::vector<uint8_t> data;
	Validator::validate(arguments["data"], "data", data);

	int option;
	Validator::validate(arguments["option"], "option", option);

	if (writeOnly) {
		i2c_write_only(i2cDesc, data.data(), data.size(), option);
	} else {
		i2c_write(i2cDesc, data.data(), data.size(), option);
	}
}

void I2c::handleRead()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("bytes_number") && arguments.count("option"))) {
		throw std::runtime_error("Expecting: bytes_number=<value> option=<value>\n");
	}

	int bytesNumber;
	Validator::validate(arguments["bytes_number"], "bytes_number", bytesNumber);

	int option;
	Validator::validate(arguments["option"], "option", option);

	std::vector<uint8_t> data(bytesNumber);
	i2c_read(i2cDesc, data.data(), bytesNumber, option);
	for (auto &byte : data) {
		std::cout << "0x" << std::hex << std::uppercase<< (int) byte << std::endl;
	}
}

const struct option I2c::options[] = {
	{"help",  no_argument,       nullptr, 'h'},
	{"init",  required_argument, nullptr, 'i'},
	{"write", required_argument, nullptr, 'w'},
	{"read",  required_argument, nullptr, 'r'},
	{nullptr, 0,                 nullptr, 0},
};

const char *const I2c::helpMessage = "Usage:\n"
				     "m2kcli i2c <uri>\n"
				     "           [-h | --help]\n"
				     "           [-i | --init frequency=<value> address=<value> scl=<index> sda=<index>]\n"
				     "           [-w | --write data=<value>,... option=<value> [write_only=<value>]]\n"
				     "           [-r | --read bytes_number=<value> option=<value>]\n"
				     "\n"
				     "Positional arguments:\n"
				     "  uri                   describe the context location \n"
				     "Optional arguments:\n"
				     "  -h, --help            show this help message and exit\n"
				     "  -i, --init [frequency=<value> address=<value> scl=<index> sda=<index> [write_only=<value>]]\n"
				     "                        initiate i2c\n"
				     "                        frequency - int\n"
				     "                        address - 7/10 bit address\n"
				     "                        scl - index of any digital pin\n"
				     "                        sda - index of any digital pin\n"
				     "                        write_only - 1 (true)\n"
				     "                                   - 0 (false) : default\n"
				     "  -w, --write [data=<value>,... option=<value>]\n"
				     "                        write the given data\n"
				     "                        data - list of bytes, comma separated values\n"
				     "                        option - 3-bit number; only one addressing bit must be set; optional repeated start\n"
				     "                            bit 0 - 7-bit addressing\n"
				     "                            bit 1 - repeated start\n"
				     "                            bit 2 - 10-bit addressing\n"
				     "  -r, --read [bytes_number=<value> option=<value>]\n"
				     "                        read a given number of bytes\n"
				     "                        bytes_number - int\n"
				     "                        option - 3-bit number; only one addressing bit must be set; optional repeated start\n"
				     "                            bit 0 - 7-bit addressing\n"
				     "                            bit 1 - repeated start\n"
				     "                            bit 2 - 10-bit addressing\n";
