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

#include <sstream>
#include <thread>
#include <chrono>
#include "analog_out.h"
#include <utils/command_out_generator.h>
#include <commands/analog/generation_controller/analog_out_binary_raw.h>
#include "commands/analog/generation_controller/analog_out_binary.h"
#include "commands/analog/generation_controller/analog_out_csv.h"
#include "commands/analog/generation_controller/analog_out_csv_raw.h"

using namespace libm2k::cli;

AnalogOut::AnalogOut(int argc, char **argv) : Command(argc, argv)
{
	if (context != nullptr) {
		analogOut = context->getAnalogOut();
	}
}

bool AnalogOut::parseArguments(std::vector<std::pair<std::string, std::string>> &output)
{
	if (context == nullptr) {
		std::cout << helpMessage;
		return false;
	}
	int c;
	bool quiet = false;
	while ((c = getopt_long(argc, argv, "hqC9:G:S:",
				options, nullptr)) != -1) {
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
			case '9':
				handleGenerate();
				break;
			case 'G':
				handleGetChannel(output);
				break;
			case 'S':
				handleSetChannel();
				break;
			default:
				break;
		}
	}
	return quiet;
}

void AnalogOut::handleCalibration()
{
	std::cout << "Calibrating . . .";
	std::cout.flush();
	bool calibration = context->calibrateDAC();
	if (calibration) {
		std::cout << "\rCalibration done.\n";
	} else {
		std::cout << "\rCalibration failed.\n";
	}
}

void AnalogOut::handleGenerate()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("channel") && arguments.count("buffer_size") && arguments.count("cyclic") &&
	      arguments.count("raw"))) {
		throw std::runtime_error(
			"Expecting: channel=<index>... buffer_size=<size> cyclic=<value> raw=<value>\n");
	}

	std::vector<unsigned int> channels;
	Validator::validate(arguments["channel"], "channel", channels);

	bool cyclic;
	Validator::validate(arguments["cyclic"], "cyclic", cyclic);


	int buffer_size = 256;
	if (arguments.count("buffer_size")) {
		Validator::validate(arguments["buffer_size"], "buffer_size", buffer_size);
	}

	bool raw;
	Validator::validate(arguments["raw"], "raw", raw);

	for (auto &channel : channels) {
		analogOut->enableChannel(channel, true);
	}
	analogOut->setCyclic(cyclic);

	std::string format;
	if (arguments.count("format")) {
		Validator::validate(arguments["format"], "format", format);
	}

	bool keepReading = true;
	std::unique_ptr<CommandOutGenerator> generator;

	if (!format.empty() && format == "binary") {
		if (raw) {
			generator = std::unique_ptr<AnalogOutBinaryRaw>(
				new AnalogOutBinaryRaw(analogOut, buffer_size, channels, cyclic));
		} else {
			generator = std::unique_ptr<AnalogOutBinary>(
				new AnalogOutBinary(analogOut, buffer_size, channels, cyclic));
		}
	} else if (format.empty() || format == "csv") {
		if (raw) {
			generator = std::unique_ptr<AnalogOutCSVRaw>(
				new AnalogOutCSVRaw(analogOut, buffer_size, channels, cyclic));
		} else {
			generator = std::unique_ptr<AnalogOutCSV>(
				new AnalogOutCSV(analogOut, buffer_size, channels, cyclic));
		}
	} else {
		throw std::runtime_error("Unknown format: " + format + '\n');

	}

	while (keepReading) {
		generator->generate(keepReading);
	}

	while (cyclic) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	analogOut->stop();
}

void AnalogOut::handleGetChannel(std::vector<std::pair<std::string, std::string>> &output)
{
	std::vector<unsigned int> channels;
	Validator::validate(std::string(optarg), "channel", channels);
	int index = optind;
	while (index < argc && *argv[index] != '-') {
		std::string argument(argv[index]);
		if (argument == "sampling_frequency") {
			for (auto &channel : channels) {
				addOutputMessage(output,
						 ("sampling_frequency_channel_" + std::to_string(channel)).c_str(),
						 std::to_string(analogOut->getSampleRate(channel)));
			}
		} else if (argument == "oversampling_ratio") {
			for (auto &channel : channels) {
				addOutputMessage(output,
						 ("oversampling_ratio_channel_" + std::to_string(channel)).c_str(),
						 std::to_string(analogOut->getOversamplingRatio(channel)));
			}
		} else if (argument == "all") {
			for (auto &channel : channels) {
				addOutputMessage(output,
						 ("sampling_frequency_channel_" + std::to_string(channel)).c_str(),
						 std::to_string(analogOut->getSampleRate(channel)));
				addOutputMessage(output,
						 ("oversampling_ratio_channel_" + std::to_string(channel)).c_str(),
						 std::to_string(analogOut->getOversamplingRatio(channel)));
			}
		} else {
			throw std::runtime_error("Invalid attribute: " + argument + '\n');
		}
		index++;
	}
}

void AnalogOut::handleSetChannel()
{
	std::vector<unsigned int> channels;
	Validator::validate(std::string(optarg), "channel", channels);
	int index = optind;
	while (index < argc && *argv[index] != '-') {
		std::string argument(argv[index]);
		if (argument.find('=') == std::string::npos) {
			throw std::runtime_error("Expecting the following format: <attribute>=<value>\n");
		}
		std::istringstream iss(argument);
		std::string value;
		getline(iss, argument, '=');
		getline(iss, value, '=');

		if (argument == "sampling_frequency") {
			for (auto &channel : channels) {
				analogOut->setSampleRate(channel, std::stod(value));
			}
		} else if (argument == "oversampling_ratio") {
			for (auto &channel : channels) {
				analogOut->setOversamplingRatio(channel, std::stod(value));
			}
		} else if (argument == "kernel_buffers") {
			for (auto &channel : channels) {
				analogOut->setKernelBuffersCount(channel, std::stoi(value));
			}
		} else {
			throw std::runtime_error("Invalid attribute: " + argument + '\n');
		}
		index++;
	}
}

const struct option AnalogOut::options[] = {
	{"help",        no_argument,       nullptr, 'h'},
	{"quiet",       no_argument,       nullptr, 'q'},
	{"calibrate",   no_argument,       nullptr, 'C'},
	{"generate",    required_argument, nullptr, '9'},
	{"get-channel", required_argument, nullptr, 'G'},
	{"set-channel", required_argument, nullptr, 'S'},
	{nullptr, 0,                       nullptr, 0},
};


const char *const AnalogOut::helpMessage = "Usage:\n"
					   "m2kcli analog-out [uri]\n"
					   "                  [-h | --help]\n"
					   "                  [-q | --quiet]\n"
					   "                  [-C | --calibrate]\n"
					   "                  [-9 | --generate channel=<index>,... cyclic=<value> raw=<value> [buffer_size=<size>] [format=<type>]]\n"
					   "                  [-G | --get-channel channel=<index> <attribute> ...]\n"
					   "                  [-S | --set-channel channel=<index> <attribute>=<value> ...]\n"
					   "\n"
					   "Pinout:\n"
					   "\u25A1 \u25A1 \u25A1 \u25A1 \u25A4 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1\n"
					   "\u25A1 \u25A1 \u25A1 \u25A1 \u25A5 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1\n"
					   "Positional arguments:\n"
					   "  uri                   describe the context location \n"
					   "                        auto | ip:192.168.2.1 | usb:XX.XX.X\n"
					   "Optional arguments:\n"
					   "  -h, --help            show this help message and exit\n"
					   "  -q, --quiet           return result only\n"
					   "  -C, --calibrate       calibrate the DAC\n"
					   "  -9, --generate channel=<index>,... cyclic=<value> raw=<value> [buffer_size=<size>] [format=<type>]\n"
					   "                        generate a signal\n"
					   "                        the samples are read from stdin\n"
					   "                        one channel: channel=<index>\n"
					   "                        many channels: channel=<index>,<index>,<index>...\n"
					   "                        channel - {0 | 1}\n"
					   "                        buffer_size - size of the output buffer; default is 256\n"
					   "                        cyclic - 0 (disable)\n"
					   "                               - 1 (enable)\n"
					   "                        raw - 0 (processed values)\n"
					   "                            - 1 (raw values)\n"
					   "                        format - {csv | binary}; default is csv\n"
					   "  -G, --get-channel channel=<index> [<attribute> ...]\n"
					   "                        return the value of the attributes corresponding to the given channel\n"
					   "                        attributes:\n"
					   "                            sampling_frequency\n"
					   "                            oversampling_ratio\n"
					   "                            all\n"
					   "  -S, --set-channel channel=<index> [<attribute>=<value> ...]\n"
					   "                        set the value of the specified attributes corresponding to the given channel\n"
					   "                        attributes: \n"
					   "                            sampling_frequency - {750 | 7500 | 75000 | 750000 | 7500000 | 75000000}\n"
					   "                            oversampling_ratio - int\n"
					   "                            kernel_buffers - int\n";
