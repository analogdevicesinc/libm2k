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

#include <sstream>
#include "analog_out.h"

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
	if (!(arguments.count("channel") && arguments.count("cyclic") && arguments.count("raw"))) {
		throw std::runtime_error("Expecting: channel=<index>... cyclic=<value> raw=<value>\n");
	}

	std::vector<int> channels;
	Validator::validate(arguments["channel"], "channel", channels);

	bool cyclic;
	Validator::validate(arguments["cyclic"], "cyclic", cyclic);

	bool raw;
	Validator::validate(arguments["raw"], "raw", raw);

	for (int &channel : channels) {
		analogOut->enableChannel(channel, true);
	}
	analogOut->setCyclic(cyclic);

	std::string format;
	std::string file;
	if (arguments.count("format")) {
		Validator::validate(arguments["format"], "format", format);
	}
	if (arguments.count("file")) {
		Validator::validate(arguments["file"], "file", file);
	}

	if (!format.empty() && format == "binary") {
		if (raw) {
			std::vector<uint16_t> samples;
			getSamplesBinaryFormat(file, samples);
			if (channels.size() < 2) {
				std::vector<short> samplesChannel;
				for (auto it = samples.begin(); it != samples.end(); it += 2) {
					samplesChannel.push_back(*it);
				}
				analogOut->pushRaw(channels[0], samplesChannel);
			} else {
				analogOut->pushRawInterleaved(reinterpret_cast<short *>(samples.data()),
							      channels.size(), samples.size() / channels.size());
			}
		} else {
			std::vector<double> samples;
			getSamplesBinaryFormat(file, samples);
			if (channels.size() < 2) {
				std::vector<double> samplesChannel;
				analogOut->push(channels[0], samplesChannel);
			} else {
				analogOut->pushInterleaved(reinterpret_cast<double *>(samples.data()), channels.size(),
							   samples.size() / channels.size());
			}
		}

	} else if (format.empty() || format == "csv") {
		if (raw) {
			std::vector<std::vector<short>> samples(2, std::vector<short>());
			getSamplesCsvFormat(file, samples);
			if (channels.size() < 2) {
				analogOut->pushRaw(channels[0], samples[0]);
			} else {
				analogOut->pushRaw(samples);
			}
		} else {
			std::vector<std::vector<double>> samples(2, std::vector<double>());
			getSamplesCsvFormat(file, samples);
			if (channels.size() < 2) {
				analogOut->push(channels[0], samples[0]);
			} else {
				analogOut->push(samples);
			}
		}
	} else {
		throw std::runtime_error("Unknown format: " + format + '\n');

	}

	std::cout << "Press ENTER to stop the generation... ";
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	analogOut->stop();
}

void AnalogOut::handleGetChannel(std::vector<std::pair<std::string, std::string>> &output)
{
	std::vector<int> channels;
	Validator::validate(std::string(optarg), "channel", channels);
	int index = optind;
	while (index < argc && *argv[index] != '-') {
		std::string argument(argv[index]);
		if (argument == "sampling_frequency") {
			for (int &channel : channels) {
				addOutputMessage(output,
						 ("sampling_frequency_channel_" + std::to_string(channel)).c_str(),
						 std::to_string(analogOut->getSampleRate(channel)));
			}
		} else if (argument == "oversampling_ratio") {
			for (int &channel : channels) {
				addOutputMessage(output,
						 ("oversampling_ratio_channel_" + std::to_string(channel)).c_str(),
						 std::to_string(analogOut->getOversamplingRatio(channel)));
			}
		} else if (argument == "all") {
			for (int &channel : channels) {
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
	std::vector<int> channels;
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
			for (int &channel : channels) {
				analogOut->setSampleRate(channel, std::stod(value));
			}
		} else if (argument == "oversampling_ratio") {
			for (int &channel : channels) {
				analogOut->setOversamplingRatio(channel, std::stod(value));
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
					   "                  [-9 | --generate channel=<index>,... cyclic=<value> raw=<value> [format=<type>] [file=<path>]]\n"
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
					   "  -C, --calibrate       calibrate the DAC"
					   "  -9, --generate channel=<index>,... cyclic=<value> raw=<value>[format=<type>] [file=<path>]\n"
					   "                        generate a signal\n"
					   "                        by default the samples are read from stdin\n"
					   "                        after writing all samples press CTRL+D(Unix) or CTRL+Z(Windows)\n"
					   "                        one channel: channel=<index>\n"
					   "                        many channels: channel=<index>,<index>,<index>...\n"
					   "                        channel - {0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15}\n"
					   "                        cyclic - 0 (disable)\n"
					   "                               - 1 (enable)\n"
					   "                        raw - 0 (processed values)\n"
					   "                            - 1 (raw values)\n"
					   "                        format - {csv | binary}; default csv\n"
					   "                        file - the path of the file containing the samples\n"
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
					   "                            oversampling_ratio - int\n";
