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

#include "analog_in.h"
#include <sstream>
#include <algorithm>


using namespace libm2k::cli;

AnalogIn::AnalogIn(int argc, char **argv) : Command(argc, argv)
{
	if (context != nullptr) {
		analogIn = context->getAnalogIn();
	}
}

bool AnalogIn::parseArguments(std::vector<std::pair<std::string, std::string>> &output)
{
	if (context == nullptr) {
		std::cout << helpMessage;
		return false;
	}
	int c, option_index = 0;
	bool quiet = false;
	while ((c = getopt_long(argc, argv, "hqCv:c:g:G:s:S:",
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
			case 'v':
				handleVoltage(output);
				break;
			case 'c':
				handleCapture();
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
				break;
			default:
				break;
		}
	}
	return quiet;
}

void AnalogIn::handleCalibration()
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

void AnalogIn::handleVoltage(std::vector<std::pair<std::string, std::string>> &output)
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("channel") && arguments.count("raw"))) {
		throw std::runtime_error("Expecting: channel=<index>... raw=<value>\n");
	}

	std::vector<int> channels;
	Validator::validate(arguments["channel"], "channel", channels);

	bool raw;
	Validator::validate(arguments["raw"], "raw", raw);

	for (int &channel : channels) {
		analogIn->enableChannel(channel, true);
		if (raw) {
			addOutputMessage(output, ("voltage_channel_" + std::to_string(channel)).c_str(),
					 std::to_string(analogIn->getVoltageRaw(channel)));
		} else {
			addOutputMessage(output, ("voltage_channel_" + std::to_string(channel)).c_str(),
					 std::to_string(analogIn->getVoltage(channel)));
		}
	}
}

void AnalogIn::handleCapture()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("channel") && arguments.count("buffer_size") && arguments.count("raw"))) {
		throw std::runtime_error("Expecting: channel=<index>... buffer_size=<value> raw=<value>\n");
	}

	std::vector<int> channels;
	Validator::validate(arguments["channel"], "channel", channels);
	std::sort(channels.begin(), channels.end());

	int bufferSize;
	Validator::validate(arguments["buffer_size"], "buffer_size", bufferSize);

	bool raw;
	Validator::validate(arguments["raw"], "raw", raw);

	for (int &channel : channels) {
		analogIn->enableChannel(channel, true);
	}

	int nb_samples = 0;
	if (arguments.count("nb_samples")) {
		Validator::validate(arguments["nb_samples"], "nb_samples", nb_samples);
	}

	std::string format;
	if (arguments.count("format")) {
		Validator::validate(arguments["format"], "format", format);
	}

	bool keep_capturing = true;
	unsigned int samplePerBuffer = bufferSize;

	while (keep_capturing) {
		if (nb_samples != 0) {
			if (nb_samples <= bufferSize) {
				samplePerBuffer = nb_samples;
				keep_capturing = false;
			}
		}
		if (!format.empty() && format == "binary") {
			std::vector<std::vector<double>> samples;
			if (raw) {
				samples = analogIn->getSamplesRaw(bufferSize);
				printRawSamplesBinaryFormat(samples, samplePerBuffer, channels);
			} else {
				samples = analogIn->getSamples(bufferSize);
				printSamplesBinaryFormat(samples, samplePerBuffer, channels);
			}

		} else if (format.empty() || format == "csv") {
			std::vector<std::vector<double>> samples;
			if (raw) {
				samples = analogIn->getSamplesRaw(bufferSize);
			} else {
				samples = analogIn->getSamples(bufferSize);
			}
			printSamplesCsvFormat(samples, samplePerBuffer, channels);
		} else {
			throw std::runtime_error("Unknown format: " + format + '\n');
		}
		if (nb_samples != 0) {
			nb_samples -= bufferSize;
		}
	}
}

void AnalogIn::handleGet(std::vector<std::pair<std::string, std::string>> &output)
{
	int index = optind - 1;
	while (index < argc && *argv[index] != '-') {
		std::string argument(argv[index]);
		if (argument == "sampling_frequency") {
			addOutputMessage(output, "sampling_frequency", std::to_string(analogIn->getSampleRate()));
		} else if (argument == "oversampling_ratio") {
			addOutputMessage(output, "oversampling_ratio",
					 std::to_string(analogIn->getOversamplingRatio()));
		} else if (argument == "trigger_source") {
			addOutputMessage(output, "trigger_source",
					 std::string(triggerSource[analogIn->getTrigger()->getAnalogSource()]));
		} else if (argument == "trigger_delay") {
			addOutputMessage(output, "trigger_delay",
					 std::to_string(analogIn->getTrigger()->getAnalogDelay()));
		} else if (argument == "all") {
			addOutputMessage(output, "sampling_frequency", std::to_string(analogIn->getSampleRate()));
			addOutputMessage(output, "oversampling_ratio",
					 std::to_string(analogIn->getOversamplingRatio()));
			addOutputMessage(output, "trigger_source",
					 std::string(triggerSource[analogIn->getTrigger()->getAnalogSource()]));
			addOutputMessage(output, "trigger_delay",
					 std::to_string(analogIn->getTrigger()->getAnalogDelay()));
		} else {
			throw std::runtime_error("Invalid attribute: " + argument + '\n');
		}
		index++;
	}
}

void AnalogIn::handleGetChannel(std::vector<std::pair<std::string, std::string>> &output)
{
	std::vector<int> channels;
	Validator::validate(std::string(optarg), "channel", channels);
	int index = optind;
	while (index < argc && *argv[index] != '-') {
		std::string argument(argv[index]);
		if (argument == "range") {
			for (int &channel : channels) {
				addOutputMessage(output, ("range_channel_" + std::to_string(channel)).c_str(),
						 std::string(range[analogIn->getRange(
							 static_cast<libm2k::analog::ANALOG_IN_CHANNEL>(channel))]));
			}
		} else if (argument == "trigger_level") {
			for (int &channel : channels) {
				addOutputMessage(output, ("trigger_level_channel_" + std::to_string(channel)).c_str(),
						 std::to_string(analogIn->getTrigger()->getAnalogLevel(channel)));
			}
		} else if (argument == "trigger_condition") {
			for (int &channel : channels) {
				addOutputMessage(output, ("trigger_condition_channel_" +
							  std::to_string(channel)).c_str(), std::string(
					triggerCondition[analogIn->getTrigger()->getAnalogCondition(channel)]));
			}
		} else if (argument == "trigger_mode") {
			for (int &channel : channels) {
				addOutputMessage(output, ("trigger_mode_channel_" + std::to_string(channel)).c_str(),
						 std::string(
							 triggerMode[analogIn->getTrigger()->getAnalogMode(channel)]));
			}
		} else if (argument == "trigger_hysteresis") {
			for (int &channel : channels) {
				addOutputMessage(output,
						 ("trigger_hysteresis_channel_" + std::to_string(channel)).c_str(),
						 std::to_string(analogIn->getTrigger()->getAnalogHysteresis(channel)));
			}
		} else if (argument == "all") {
			for (int &channel : channels) {
				addOutputMessage(output, ("range_channel_" + std::to_string(channel)).c_str(),
						 std::string(range[analogIn->getRange(
							 static_cast<libm2k::analog::ANALOG_IN_CHANNEL>(channel))]));
				addOutputMessage(output, ("trigger_level_channel_" + std::to_string(channel)).c_str(),
						 std::to_string(analogIn->getTrigger()->getAnalogLevel(channel)));
				addOutputMessage(output, ("trigger_condition_channel_" +
							  std::to_string(channel)).c_str(), std::string(
					triggerCondition[analogIn->getTrigger()->getAnalogCondition(channel)]));
				addOutputMessage(output, ("trigger_mode_channel_" + std::to_string(channel)).c_str(),
						 std::string(
							 triggerMode[analogIn->getTrigger()->getAnalogMode(channel)]));
				addOutputMessage(output,
						 ("trigger_hysteresis_channel_" + std::to_string(channel)).c_str(),
						 std::to_string(analogIn->getTrigger()->getAnalogHysteresis(channel)));
			}
		} else {
			throw std::runtime_error("Invalid attribute: " + argument + '\n');
		}
		index++;
	}
}

void AnalogIn::handleSet()
{
	int index = optind - 1;
	while (index < argc && *argv[index] != '-') {
		std::string argument(argv[index]);
		if (argument.find('=') == std::string::npos) {
			throw std::invalid_argument("Expecting the following format: attr_name=val\n");
		}
		std::istringstream iss(argument);
		std::string value;
		getline(iss, argument, '=');
		getline(iss, value, '=');

		if (argument == "sampling_frequency") {
			analogIn->setSampleRate(std::stod(value));
		} else if (argument == "oversampling_ratio") {
			analogIn->setOversamplingRatio(std::stod(value));
		} else if (argument == "trigger_source") {
			int enumIndex = getIndexOfElement(value, triggerSource);
			if (enumIndex == -1) {
				throw std::runtime_error("Invalid trigger source: '" + std::string(value) + "'.\n");
			}
			analogIn->getTrigger()->setAnalogSource(
				static_cast<libm2k::M2K_TRIGGER_SOURCE_ANALOG>(enumIndex));
		} else if (argument == "trigger_delay") {
			analogIn->getTrigger()->setAnalogDelay(std::stoi(value));
		} else if (argument == "kernel_buffers") {
			analogIn->setKernelBuffersCount(std::stoi(value));
		} else {
			throw std::runtime_error("Invalid attribute: " + argument + '\n');
		}
		index++;
	}
}

void AnalogIn::handleSetChannel()
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

		if (argument == "range") {
			int enumIndex = getIndexOfElement(value, range);
			if (enumIndex == -1) {
				throw std::runtime_error("Invalid range: '" + std::string(value) + "'.\n");
			}
			for (int &channel : channels) {
				analogIn->setRange(static_cast<libm2k::analog::ANALOG_IN_CHANNEL>(channel),
						   static_cast<libm2k::analog::M2K_RANGE>(enumIndex));
			}
		} else if (argument == "trigger_level") {
			for (int &channel : channels) {
				analogIn->getTrigger()->setAnalogLevel(channel, std::stod(value));
			}
		} else if (argument == "trigger_condition") {
			int enumIndex = getIndexOfElement(value, triggerCondition);
			if (enumIndex == -1) {
				throw std::runtime_error("Invalid trigger condition: '" + std::string(value) + "'.\n");
			}
			for (int &channel : channels) {
				analogIn->getTrigger()->setAnalogCondition(channel,
									   static_cast<libm2k::M2K_TRIGGER_CONDITION_ANALOG>(enumIndex));
			}
		} else if (argument == "trigger_mode") {
			int enumIndex = getIndexOfElement(value, triggerMode);
			if (enumIndex == -1) {
				throw std::runtime_error("Invalid trigger mode: '" + std::string(value) + "'.\n");
			}
			for (int &channel : channels) {
				analogIn->getTrigger()->setAnalogMode(channel,
								      static_cast<libm2k::M2K_TRIGGER_MODE>(enumIndex));
			}
		} else if (argument == "trigger_hysteresis") {
			for (int &channel : channels) {
				analogIn->getTrigger()->setAnalogHysteresis(channel, std::stod(value));
			}
		} else {
			throw std::runtime_error("Invalid attribute: " + argument + '\n');
		}
		index++;
	}
}

const struct option AnalogIn::options[] = {
	{"help",        no_argument,       nullptr, 'h'},
	{"quiet",       no_argument,       nullptr, 'q'},
	{"calibration", no_argument,       nullptr, 'C'},
	{"voltage",     required_argument, nullptr, 'v'},
	{"capture",     required_argument, nullptr, 'c'},
	{"get",         required_argument, nullptr, 'g'},
	{"get-channel", required_argument, nullptr, 'G'},
	{"set",         required_argument, nullptr, 's'},
	{"set-channel", required_argument, nullptr, 'S'},
	{nullptr, 0,                       nullptr, 0},
};

const std::vector<const char *> AnalogIn::triggerSource{"channel_1", "channel_2", "channel_1_or_channel_2",
							"channel_1_and_channel_2", "channel_1_xor_channel_2"};
const std::vector<const char *> AnalogIn::range{"low", "high"};
const std::vector<const char *> AnalogIn::triggerCondition{"rising_edge", "falling_edge", "low_level", "high_level",
							   "any_edge", "no_trigger"};
const std::vector<const char *> AnalogIn::triggerMode{"always", "analog", "digital", "digital_or_analog",
						      "digital_and_analog", "digital_xor_analog",
						      "n_digital_or_analog", "n_digital_and_analog",
						      "n_digital_xor_analog"};

const char *const AnalogIn::helpMessage = "Usage:\n"
					  "m2kcli analog-in <uri>\n"
					  "                 [-h | --help]\n"
					  "                 [-q | --quiet]\n"
					  "                 [-C | --calibrate]\n"
					  "                 [-v | --voltage channel=<index>... raw=<value>]\n"
					  "                 [-c | --capture channel=<index>... buffer_size=<size> raw=<value> [nb_samples=<value>] [format=<type>]]\n"
					  "                 [-g | --get <attribute> ...]\n"
					  "                 [-G | --get-channel channel=<index> <attribute> ...]\n"
					  "                 [-s | --set <attribute>=<value> ...]\n"
					  "                 [-S | --set-channel channel=<index> <attribute>=<value> ...]\n"
					  "\n"
					  "Pinout:\n"
					  "\u25A4 \u25A5 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1\n"
					  "\u25A4 \u25A5 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1 \u25A1\n"
					  "\n"
					  "Positional arguments:\n"
					  "  uri                   describe the context location \n"
					  "                        auto | ip:192.168.2.1 | usb:XX.XX.X\n"
					  "Optional arguments:\n"
					  "  -h, --help            show this help message and exit\n"
					  "  -q, --quiet           return result only\n"
					  "  -C, --calibrate       calibrate the ADC\n"
					  "  -v, --voltage channel=<index>... raw=<value>\n"
					  "                        print the voltage of the given channel\n"
					  "                        channel - {0 | 1}\n"
					  "                        raw - 0 (processed values)\n"
					  "                            - 1 (raw values)\n"
					  "  -c, --capture channel=<index>... buffer_size=<size> raw=<value> [nb_samples=<value>] [format=<type>]\n"
					  "                        print a specific number of samples\n"
					  "                        nb_samples - number of samples to be captured, 0 = infinite; default\n"
					  "                        format - {csv | binary}; default csv\n"
					  "  -g, --get [<attribute>...]\n"
					  "                        return the value of the specified global attributes\n"
					  "                        attribute:\n"
					  "                            sampling_frequency\n"
					  "                            oversampling_ratio\n"
					  "                            trigger_source\n"
					  "                            trigger_delay\n"
					  "                            all\n"
					  "  -G, --get-channel channel=<index>... [<attribute> ...]\n"
					  "                        return the value of the attributes corresponding to the given channel\n"
					  "                        attributes:\n"
					  "                            range\n"
					  "                            trigger_level\n"
					  "                            trigger_condition\n"
					  "                            trigger_mode\n"
					  "                            trigger_hysteresis\n"
					  "                            all\n"
					  "  -s, --set [<attribute>=<value>...]\n"
					  "                        set the value of the specified global attributes\n"
					  "                        attribute:\n"
					  "                            sampling_frequency - {1000 | 10000 | 100000 | 1000000 | 10000000 | 100000000}\n"
					  "                            oversampling_ratio - int\n"
					  "                            trigger_source - {channel_1 | channel_2 | channel_1_or_channel_2 | channel_1_and_channel_2 | channel_1_xor_channel_2}\n"
					  "                            trigger_delay - int\n"
					  "                            kernel_buffers - int\n"
					  "  -S, --set-channel channel=<index>... [<attribute>=<value> ...]\n"
					  "                        set the value of the specified attributes corresponding to the given channel\n"
					  "                        attributes: \n"
					  "                            range - {high | low}\n"
					  "                            trigger_level - int\n"
					  "                            trigger_condition - {rising_edge | falling_edge | low_level | high_level}\n"
					  "                            trigger_mode - {always | analog | digital | digital_or_analog | digital_and_analog | digital_xor_analog | \n"
					  "                                            n_digital_or_analog | n_digital_and_analog | n_digital_xor_analog}\n"
					  "                            trigger_hysteresis - double (in Volts)\n";
