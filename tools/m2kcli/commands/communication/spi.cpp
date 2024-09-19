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


#include "spi.h"
#include <libm2k/tools/spi_extra.hpp>

using namespace libm2k::cli;

Spi::Spi(int argc, char **argv) : Command(argc, argv)
{
	spiDesc = nullptr;
	writeOnly = true;
}

Spi::~Spi()
{
	if (spiDesc != nullptr) {
		spi_remove(spiDesc);
	}
}

bool Spi::parseArguments(std::vector<std::pair<std::string, std::string>> &output)
{
	if (context == nullptr) {
		std::cout << helpMessage;
		return false;
	}
	int c, option_index = 0;
	while ((c = getopt_long(argc, argv, "hi:w:",
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
			default:
				break;
		}
	}
	return false;
}

void Spi::handleInit()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("clk") && arguments.count("mosi") && arguments.count("frequency")
	      && arguments.count("cs") && arguments.count("mode") && arguments.count("bit_numbering")
	      && arguments.count("cs_polarity"))) {
		throw std::runtime_error(
			"Expecting: frequency=<value> clk=<value> mosi=<index> cs=<index> mode=<value> bit_numbering=<value> cs_polarity=<value>\n");
	}

	int frequency;
	Validator::validate(arguments["frequency"], "frequency", frequency);

	int clk;
	Validator::validate(arguments["clk"], "clk", clk);

	int mosi;
	Validator::validate(arguments["mosi"], "mosi", mosi);

	int cs;
	Validator::validate(arguments["cs"], "cs", cs);

	int mode;
	Validator::validate(arguments["mode"], "mode", mode);

	std::string bitNumbering;
	Validator::validate(arguments["bit_numbering"], "bit_numbering", bitNumbering);

	bool cs_polarity;
	Validator::validate(arguments["cs_polarity"], "cs_polarity", cs_polarity);

	int miso = 0xFF;
	if (arguments.count("miso")) {
		writeOnly = false;
		Validator::validate(arguments["miso"], "miso", miso);
	}

	m2k_spi_init m2KSpiInit;
	m2KSpiInit.clock = clk;
	m2KSpiInit.mosi = mosi;
	m2KSpiInit.miso = miso;
	m2KSpiInit.context = context;
	if (bitNumbering == "MSB") {
		m2KSpiInit.bit_numbering = MSB;
	} else if (bitNumbering == "LSB") {
		m2KSpiInit.bit_numbering = LSB;
	} else {
		throw std::runtime_error("Invalid bit numbering: '" + std::string(bitNumbering) + "'.\n");
	}

	m2KSpiInit.cs_polarity = static_cast<enum cs_polarity>(cs_polarity);
	spi_init_param spiInitParam;
	spiInitParam.max_speed_hz = frequency;
	spiInitParam.mode = static_cast<spi_mode>(mode);
	spiInitParam.chip_select = cs;
	spiInitParam.extra = (void *) &m2KSpiInit;

	int32_t retVal;
	if (writeOnly) {
		retVal = spi_init_write_only(&spiDesc, &spiInitParam);
	} else {
		retVal = spi_init(&spiDesc, &spiInitParam);
	}

	if (retVal != 0) {
		throw std::runtime_error("Could not initiate SPI\n");
	}
}

void Spi::handleWrite()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("data"))) {
		throw std::runtime_error("Expecting: data=<value>,...\n");
	}

	std::vector<uint8_t> data;
	Validator::validate(arguments["data"], "data", data);

	if (writeOnly) {
		spi_write_only(spiDesc, data.data(), data.size());
	} else {
		spi_write_and_read(spiDesc, data.data(), data.size());
		for (auto &byte : data) {
			std::cout << "0x" << std::hex << std::uppercase << (int) byte << std::endl;
		}
	}
}

const struct option Spi::options[] = {
	{"help",  no_argument,       nullptr, 'h'},
	{"init",  required_argument, nullptr, 'i'},
	{"write", required_argument, nullptr, 'w'},
	{nullptr, 0,                 nullptr, 0},
};

const char *const Spi::helpMessage = "Usage:\n"
				     "m2kcli spi <uri>\n"
				     "           [-h | --help]\n"
				     "           [-i | --init [frequency=<value> clk=<value> mosi=<index> [miso=<index>] cs=<index> mode=<value> bit_numbering=<value>]\n"
				     "           [-w | --write data=<value>,...]\n"
				     "\n"
				     "Positional arguments:\n"
				     "  uri                   describe the context location \n"
				     "Optional arguments:\n"
				     "  -h, --help            show this help message and exit\n"
				     "  -i, --init [frequency=<value> clk=<value> mosi=<index> [miso=<index>] cs=<index> mode=<value> bit_numbering=<value>]\n"
				     "                        initiate spi\n"
				     "                        frequency - int\n"
				     "                        clk - index of any digital pin\n"
				     "                        mosi - index of any digital pin\n"
				     "                        miso - index of any digital pin; if miso is absent -> write only\n"
				     "                        cs - index of any digital pin\n"
				     "                        mode - {0 | 1 | 2 | 3}\n"
				     "                        bit_numbering - {MSB | LSB}\n"
				     "  -w, --write [data=<value>,...]\n"
				     "                        write the given data and print the read data\n"
				     "                        data - list of bytes, comma separated values\n";
