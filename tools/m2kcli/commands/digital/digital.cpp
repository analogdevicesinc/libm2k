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
#include <limits>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <utility>
#include "digital.h"

using namespace libm2k::cli;

Digital::Digital(int argc, char **argv) : Command(argc, argv)
{
	if (context != nullptr) {
		digital = context->getDigital();
	}
}

bool Digital::parseArguments(std::vector<std::pair<std::string, std::string>> &output)
{
	if (context == nullptr) {
		std::cout << helpMessage;
		return false;
	}
	int c;
	bool quiet = false;
	while ((c = getopt_long(argc, argv, "hqc:9:g:G:s:S:",
				options, nullptr)) != -1) {
		switch (c) {
			case 'h':
				std::cout << helpMessage;
				return quiet;
			case 'q':
				quiet = true;
				break;
			case 'c':
				handleCapture();
				break;
			case '9':
				handleGenerate();
				break;
			case 'g':
				handleGet(output);
				break;
			case 'G':
				handleGetChannel(output);
				break;
			case 's':
				handleSet();
				break;
			case 'S':
				handleSetChannel();
			default:
				break;
		}
	}
	return quiet;
}


void Digital::handleCapture()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("buffer_size"))) {
		throw std::runtime_error("Expecting: buffer_size=<size>\n");
	}
	int bufferSize;
	Validator::validate(arguments["buffer_size"], "buffer_size", bufferSize);
	for (int i = 0; i <= 15; i++) {
		digital->setDirection(i, false);
	}

	std::string format;
	std::vector<uint16_t> samples = digital->getSamples(bufferSize);

	if (arguments.count("format")) {
		Validator::validate(arguments["format"], "format", format);
	}
	if (!format.empty() && format == "binary") {
		printSamplesBinaryFormat(samples);
	} else if (format.empty() || format == "csv") {
		printSamplesCsvFormat(samples);
	} else {
		throw std::runtime_error("Unknown format: " + format + '\n');
	}
}

void Digital::handleGenerate()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("channel") && arguments.count("cyclic"))) {
		throw std::runtime_error("Expecting: channel=<index>,<index>... cyclic=<value>\n");
	}

	std::vector<int> channels;
	Validator::validate(arguments["channel"], "channel", channels);

	bool cyclic;
	Validator::validate(arguments["cyclic"], "cyclic", cyclic);

	for (int &channel : channels) {
		digital->setDirection(channel, true);
		digital->enableChannel(channel, true);
	}
	digital->setCyclic(cyclic);

	std::string format;
	std::string file;
	if (arguments.count("format")) {
		Validator::validate(arguments["format"], "format", format);
	}
	if (arguments.count("file")) {
		Validator::validate(arguments["file"], "file", file);
	}

	std::vector<uint16_t> samples;
	if (!format.empty() && format == "binary") {
		getSamplesBinaryFormat(file, samples);
		digital->push(samples);

	} else if (format.empty() || format == "csv") {
		getSamplesCsvFormat(file, samples);
		digital->push(samples);
	} else {
		throw std::runtime_error("Unknown format: " + format + '\n');
	}

	std::cout << "Press ENTER to stop the generation... ";
	std::cin.clear();
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	digital->stop();
}

void Digital::handleGet(std::vector<std::pair<std::string, std::string>> &output)
{
	int index = optind - 1;
	while (index < argc && *argv[index] != '-') {
		std::string argument(argv[index]);
		if (argument == "sampling_frequency_in") {
			addOutputMessage(output, "sampling_frequency_in", std::to_string(digital->getSampleRateIn()));
		} else if (argument == "sampling_frequency_out") {
			addOutputMessage(output, "sampling_frequency_out", std::to_string(digital->getSampleRateOut()));
		} else if (argument == "trigger_delay") {
			addOutputMessage(output, "trigger_delay",
					 std::to_string(digital->getTrigger()->getDigitalDelay()));
		} else if (argument == "trigger_mode") {
			addOutputMessage(output, "trigger_mode",
					 std::string(triggerMode[digital->getTrigger()->getDigitalMode()]));
		} else if (argument == "all") {
			addOutputMessage(output, "sampling_frequency_in", std::to_string(digital->getSampleRateIn()));
			addOutputMessage(output, "sampling_frequency_out", std::to_string(digital->getSampleRateOut()));
			addOutputMessage(output, "trigger_delay",
					 std::to_string(digital->getTrigger()->getDigitalDelay()));
			addOutputMessage(output, "trigger_mode",
					 std::string(triggerMode[digital->getTrigger()->getDigitalMode()]));
		} else {
			throw std::runtime_error("Invalid attribute: " + argument + '\n');
		}
		index++;
	}
}

void Digital::handleGetChannel(std::vector<std::pair<std::string, std::string>> &output)
{
	std::vector<int> channels;
	Validator::validate(std::string(optarg), "channel", channels);
	int index = optind;
	while (index < argc && *argv[index] != '-') {
		std::string argument(argv[index]);
		if (argument == "value") {
			for (int &channel : channels) {
				addOutputMessage(output, ("value_channel_" + std::to_string(channel)).c_str(),
						 std::to_string(digital->getValueRaw(channel)));
			}
		} else if (argument == "output_mode") {
			for (int &channel : channels) {
				addOutputMessage(output, ("output_mode_channel_" + std::to_string(channel)).c_str(),
						 std::string(outputMode[digital->getOutputMode(channel)]));
			}
		} else if (argument == "trigger_condition") {
			for (int &channel : channels) {
				addOutputMessage(output, ("trigger_condition_channel_" +
							  std::to_string(channel)).c_str(), std::string(
					triggerCondition[digital->getTrigger()->getDigitalCondition(channel)]));
			}
		} else if (argument == "all") {
			for (int &channel : channels) {
				addOutputMessage(output, ("value_channel_" + std::to_string(channel)).c_str(),
						 std::to_string(digital->getValueRaw(channel)));
				addOutputMessage(output, ("output_mode_channel_" + std::to_string(channel)).c_str(),
						 std::string(outputMode[digital->getOutputMode(channel)]));
				addOutputMessage(output, ("trigger_condition_channel_" +
							  std::to_string(channel)).c_str(), std::string(
					triggerCondition[digital->getTrigger()->getDigitalCondition(channel)]));
			}
		} else {
			throw std::runtime_error("Invalid attribute: " + argument + '\n');
		}
		index++;
	}
}

void Digital::handleSet()
{
	int index = optind - 1;
	while (index < argc && *argv[index] != '-') {
		std::string argument(argv[index]);
		if (argument.find('=') == std::string::npos) {
			throw std::invalid_argument("Expecting the following format: <attribute>=<value>\n");
		}
		std::istringstream iss(argument);
		std::string value;
		getline(iss, argument, '=');
		getline(iss, value, '=');

		if (argument == "sampling_frequency_in") {
			digital->setSampleRateIn(std::stod(value));
		} else if (argument == "sampling_frequency_out") {
			digital->setSampleRateOut(std::stod(value));
		} else if (argument == "trigger_delay") {
			digital->getTrigger()->setDigitalDelay(std::stoi(value));
		} else if (argument == "trigger_mode") {
			int enumIndex = std::distance(triggerMode.begin(),
						      std::find(triggerMode.begin(), triggerMode.end(),
								value));
			digital->getTrigger()->setDigitalMode(
				static_cast<libm2k::digital::DIO_TRIGGER_MODE>(enumIndex));
		} else {
			throw std::runtime_error("Invalid attribute: " + argument + '\n');
		}
		index++;
	}
}

void Digital::handleSetChannel()
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

		if (argument == "value") {
			for (int &channel : channels) {
				digital->setValueRaw(static_cast<libm2k::digital::DIO_CHANNEL>(channel),
						     std::stoi(value));
			}
		} else if (argument == "output_mode") {
			int enumIndex = std::distance(outputMode.begin(),
						      std::find(outputMode.begin(), outputMode.end(),
								value));
			for (int &channel : channels) {
				digital->setOutputMode(static_cast<libm2k::digital::DIO_CHANNEL>(channel),
						       static_cast<libm2k::digital::DIO_MODE>(enumIndex));
			}
		} else if (argument == "trigger_condition") {
		} else if (argument == "triggerCondition") {
			int enumIndex = std::distance(triggerCondition.begin(),
						      std::find(triggerCondition.begin(),
								triggerCondition.end(), value));
			for (int &channel : channels) {
				digital->getTrigger()->setDigitalCondition(channel,
									   static_cast<libm2k::analog::M2K_TRIGGER_CONDITION>(enumIndex));
			}
		} else {
			throw std::runtime_error("Invalid attribute: " + argument + '\n');
		}
		index++;
	}
}

const struct option Digital::options[] = {
	{"help",        no_argument,       nullptr, 'h'},
	{"quiet",       no_argument,       nullptr, 'q'},
	{"capture",     required_argument, nullptr, 'c'},
	{"generate",    required_argument, nullptr, '9'},
	{"get",         required_argument, nullptr, 'g'},
	{"get-channel", required_argument, nullptr, 'G'},
	{"set",         required_argument, nullptr, 's'},
	{"set-channel", required_argument, nullptr, 'S'},
	{nullptr, 0,                       nullptr, 0},
};

const std::vector<const char *> Digital::triggerMode{"or", "and"};
const std::vector<const char *> Digital::triggerCondition{"rising_edge", "falling_edge", "low_level", "high_level",
							  "any_edge", "no_trigger"};
const std::vector<const char *> Digital::outputMode{"open_drain", "push_pull"};

const char *const Digital::helpMessage = "Usage:\n"
					 "m2kcli digital <uri>\n"
					 "               [-h | --help]\n"
					 "               [-q | --quiet]\n"
					 "               [-c | --capture buffer_size=<size> [format=<type>]]\n"
					 "               [-9 | --generate channel=<index>,... cyclic=<value> [format=<type>] [file=<path>]]\n"
					 "               [-g | --get <attribute> ...]\n"
					 "               [-G | --get-channel channel=<index> <attribute> ...]\n"
					 "               [-s | --set <attribute>=<value> ...]\n"
					 "               [-S | --set-channel channel=<index> <attribute>=<value> ...]\n"
					 "\n"
					 "Pinout:\n"
					 "\u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A0 \u25A0 \u25A0 \u25A0 \u25A0 \u25A0 \u25A0 \u25A0\n"
					 "\u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A0 \u25A0 \u25A0 \u25A0 \u25A0 \u25A0 \u25A0 \u25A0\n"
					 "\n"
					 "Positional arguments:\n"
					 "  uri                   describe the context location \n"
					 "                        auto | ip:192.168.2.1 | usb:XX.XX.X\n"
					 "Optional arguments:\n"
					 "  -h, --help            show this help message and exit\n"
					 "  -q, --quiet           return result only\n"
					 "  -c, --capture buffer_size=<size> [format=<type>]\n"
					 "                        print a specific number of samples\n"
					 "                        format - {csv | binary}; default csv\n"
					 "  -9, --generate channel=<index>,... cyclic=<value> [format=<type>] [file=<path>]\n"
					 "                        generate a signal\n"
					 "                        by default the samples are read from stdin\n"
					 "                        after writing all samples press CTRL+D(Unix) or CTRL+Z(Windows)\n"
					 "                        one channel: channel=<index>\n"
					 "                        many channels: channel=<index>,<index>,<index>...\n"
					 "                        channel - {0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15}\n"
					 "                        cyclic - 0 (disable)\n"
					 "                               - 1 (enable)\n"
					 "                        format - {csv | binary}; default csv\n"
					 "                        file - the path of the file containing the samples\n"
					 "  -g, --get [<attribute>...]\n"
					 "                        return the value of the specified global attributes\n"
					 "                        attribute:\n"
					 "                            sampling_frequency_in\n"
					 "                            sampling_frequency_out\n"
					 "                            trigger_delay\n"
					 "                            trigger_mode\n"
					 "                            all\n"
					 "  -G, --get-channel channel=<index> [<attribute> ...]\n"
					 "                        return the value of the attributes corresponding to the given channel\n"
					 "                        attributes:\n"
					 "                            value\n"
					 "                            output_mode\n"
					 "                            triggerCondition\n"
					 "                            all\n"
					 "  -s, --set [<attribute>=<value>...]\n"
					 "                        set the value of the specified global attributes\n"
					 "                        attribute:\n"
					 "                            sampling_frequency_in - double\n"
					 "                            sampling_frequency_out - double\n"
					 "                            trigger_delay - int\n"
					 "                            trigger_mode - {or | and}\n"
					 "  -S, --set-channel channel=<index> [<attribute>=<value> ...]\n"
					 "                        set the value of the specified attributes corresponding to the given channel\n"
					 "                        attributes: \n"
					 "                            value - {0 | 1}\n"
					 "                            output_mode - {open_drain | push_pull}\n"
					 "                            trigger_condition - {rising_edge | falling_edge | low_level | high_level | any_edge | no_trigger}\n";
