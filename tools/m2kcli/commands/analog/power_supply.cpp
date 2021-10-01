/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * */

#include "power_supply.h"

using namespace libm2k::cli;

PowerSupply::PowerSupply(int argc, char **argv) : Command(argc, argv)
{
	if (context != nullptr) {
		powerSupply = context->getPowerSupply();
	}
}

bool PowerSupply::parseArguments(std::vector<std::pair<std::string, std::string>> &output)
{
	if (context == nullptr) {
		std::cout << helpMessage;
		return false;
	}
	int c, option_index = 0;
	bool quiet = false;
	while ((c = getopt_long(argc, argv, "hqCc:9:",
				options, &option_index)) != -1) {
		switch (c) {
			case 'h':
				std::cout << helpMessage;
				return quiet;
			case 'q':
				quiet = true;
				break;
			case 'C':
				handleCalibration();
				break;
			case 'c':
				handleCapture(output);
				break;
			case '9':
				handleGenerate();
				break;
			default:
				break;
		}
	}
	return quiet;
}

void PowerSupply::handleCalibration()
{
	std::cout << "Calibrating . . .";
	std::cout.flush();
	bool calibration = context->calibrateADC();
	if (calibration) {
		std::cout << "\rCalibration done.\n";
	} else {
		std::cout << "\rCalibration failed.\n";
	}
}

void PowerSupply::handleCapture(std::vector<std::pair<std::string, std::string>> &output)
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!arguments.count("channel")) {
		throw std::runtime_error("Expecting: channel=<index>...\n");
	}

	std::vector<unsigned int> channels;
	Validator::validate(arguments["channel"], "channel", channels);

	for (auto &channel : channels) {
		powerSupply->enableChannel(channel, true);
		addOutputMessage(output, ("voltage_channel_" + std::to_string(channel)).c_str(),
				 std::to_string(powerSupply->readChannel(channel)));
	}
}

void PowerSupply::handleGenerate()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("channel") && arguments.count("value"))) {
		throw std::runtime_error("Expecting: channel=<index>... value=<value>\n");
	}

	std::vector<unsigned int> channels;
	Validator::validate(arguments["channel"], "channel", channels);

	double value;
	Validator::validate(arguments["value"], "value", value);

	for (auto &channel : channels) {
		powerSupply->enableChannel(channel, true);
		powerSupply->pushChannel(channel, value);
	}
}

const struct option PowerSupply::options[] = {
	{"help",        no_argument,       nullptr, 'h'},
	{"quiet",       no_argument,       nullptr, 'q'},
	{"calibration", no_argument,       nullptr, 'C'},
	{"capture",     required_argument, nullptr, 'c'},
	{"generate",    required_argument, nullptr, '9'},
	{nullptr, 0,                       nullptr, 0},
};

const char *const PowerSupply::helpMessage = "Usage:\n"
					     "m2kcli analog-in <uri>\n"
					     "                 [-h | --help]\n"
					     "                 [-q | --quiet]\n"
					     "                 [-C | --calibrate]\n"
					     "                 [-c | --capture channel=<index>...]\n"
					     "                 [-9 | --generate channel=<index>,... value=<value>]\n"
					     "\n"
					     "Pinout:\n"
					     "\u25A1 \u25A1 \u25A1 \u25A4 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1\n"
					     "\u25A1 \u25A1 \u25A1 \u25A5 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1\n"
					     "\n"
					     "Positional arguments:\n"
					     "  uri                   describe the context location \n"
					     "                        auto | ip:192.168.2.1 | usb:XX.XX.X\n"
					     "Optional arguments:\n"
					     "  -h, --help            show this help message and exit\n"
					     "  -q, --quiet           return result only\n"
					     "  -c, --capture channel=<index>...\n"
					     "                        print the voltage of the given channel\n"
					     "  -9, --generate channel=<index>,... value=<value>\n"
					     "                        push the voltage on the given channels\n";
