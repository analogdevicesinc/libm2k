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

#include "uart.h"
#include <libm2k/tools/uart_extra.hpp>
#include <libm2k/m2khardwaretrigger.hpp>
#include <libm2k/digital/m2kdigital.hpp>
#include <fstream>


using namespace libm2k::cli;

Uart::Uart(int argc, char **argv) : Command(argc, argv)
{
	uartDesc = nullptr;
}

Uart::~Uart()
{
	if (uartDesc != nullptr) {
		uart_remove(uartDesc);
	}
}

bool Uart::parseArguments(std::vector<std::pair<std::string, std::string>> &output)
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

void Uart::handleInit()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("device") && arguments.count("baud_rate") && arguments.count("parity") &&
	      arguments.count("bits_number") && arguments.count("stop_bits"))) {
		throw std::runtime_error(
			"Expecting: device=<index> baud_rate=<value> parity=<value> bits_number=<value> stop_bits=<value>\n");
	}

	int device;
	Validator::validate(arguments["device"], "device", device);

	int baud_rate;
	Validator::validate(arguments["baud_rate"], "baud_rate", baud_rate);

	int bits_number;
	Validator::validate(arguments["bits_number"], "bits_number", bits_number);

	std::string stop_bits;
	Validator::validate(arguments["stop_bits"], "stop_bits", stop_bits);
	int enumIndexStop = getIndexOfElement(stop_bits, this->stop_bits);
	if (enumIndexStop == -1) {
		throw std::runtime_error("Invalid number of stop bits: '" + std::string(stop_bits) + "'.\n");
	}

	std::string parity;
	Validator::validate(arguments["parity"], "parity", parity);
	int enumIndexParity = getIndexOfElement(parity, this->parity);
	if (enumIndexParity == -1) {
		throw std::runtime_error("Invalid parity: '" + std::string(parity) + "'.\n");
	}
	m2k_uart_init m2KUartInit;
	m2KUartInit.bits_number = bits_number;
	m2KUartInit.parity = static_cast<uart_parity>(enumIndexParity);
	m2KUartInit.stop_bits = static_cast<uart_stop_bits>(enumIndexStop + 2);
	m2KUartInit.context = context;

	uart_init_param uartInitParam;
	uartInitParam.device_id = device;
	uartInitParam.baud_rate = baud_rate;
	uartInitParam.extra = (void *) &m2KUartInit;

	if (uart_init(&uartDesc, &uartInitParam) == -1) {
		throw std::runtime_error("Could not initiate UART\n");
	}
}

void Uart::handleWrite()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	std::vector<uint8_t> data;
	if (!(arguments.count("data"))) {
		throw std::runtime_error("Expecting: data=<value>\n");
	}

	Validator::validate(arguments["data"], "data", data);
	if (data.empty()) {
		std::string empty_string;
		getText(empty_string, data);
	}

	uart_write(uartDesc, data.data(), data.size());
}

void Uart::handleRead()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("bytes_number"))) {
		throw std::runtime_error("Expecting: bytes_number=<value> format=<value>\n");
	}

	int bytesNumber;
	Validator::validate(arguments["bytes_number"], "bytes_number", bytesNumber);

	std::string format;
	if (arguments.count("format")) {
		Validator::validate(arguments["format"], "format", format);
	}

	std::vector<uint8_t> data(bytesNumber);
	uart_read(uartDesc, data.data(), bytesNumber);

	if (format.empty() || format == "text") {
		for (auto &byte : data) {
			std::cout << (char) byte << std::flush;
		}
	} else if (format == "number") {
		for (auto &byte : data) {
			std::cout << "0x" << std::hex << std::uppercase << (int) byte << std::endl;
		}
	} else {
		throw std::runtime_error("Unknown format: " + format + '\n');
	}
}

const struct option Uart::options[] = {
	{"help",  no_argument,       nullptr, 'h'},
	{"init",  required_argument, nullptr, 'i'},
	{"write", required_argument, nullptr, 'w'},
	{"read",  required_argument, nullptr, 'r'},
	{nullptr, 0,                 nullptr, 0},
};

const std::vector<const char *> Uart::parity{"none", "odd", "even", "mark", "space"};

const std::vector<const char *> Uart::stop_bits{"1", "1.5", "2"};

const char *const Uart::helpMessage = "Usage:\n"
				      "m2kcli uart <uri>\n"
				      "           [-h | --help]\n"
				      "           [-i | --init device=<index> baud_rate=<value> parity=<value> bits_number=<value> stop_bits=<value>]\n"
				      "           [-w | --write data=<value>,...]\n"
				      "           [-r | --read bytes_number=<value> format=<value>]\n"
				      "\n"
				      "Positional arguments:\n"
				      "  uri                   describe the context location \n"
				      "Optional arguments:\n"
				      "  -h, --help            show this help message and exit\n"
				      "  -i, --init [device=<index> baud_rate=<value> parity=<value> bits_number=<value> stop_bits=<value>]\n"
				      "                        initiate uart\n"
				      "                        device - index of any digital pin\n"
				      "                        baud_rate - int\n"
				      "                        parity = {none | odd | even | mark | space}\n"
				      "                        bits_number - {5 | 6 | 7 | 8}\n"
				      "                        stop_bits - {1 | 1.5 | 2}\n"
				      "  -w, --write [data=<value>,... ]\n"
				      "                        write the given data\n"
				      "                        data - list of bytes, comma separated values\n"
				      "                        let data empty (data=) in order to read its content from stdin\n"
				      "  -r, --read [bytes_number=<value> format=<value>]\n"
				      "                        read a given number of bytes\n"
				      "                        bytes_number - int\n"
				      "                        format - {text | number}; default text\n";

void Uart::getText(std::string &file, std::vector<uint8_t> &text)
{
	if (!file.empty()) {
		std::ifstream in(file);
		text = std::vector<uint8_t>(
			(std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	} else {
		text = std::vector<uint8_t>(
			(std::istreambuf_iterator<char>(std::cin)),
			std::istreambuf_iterator<char>());
	}
}
