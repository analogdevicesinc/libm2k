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

#include "context.h"
#include <sstream>
#include <algorithm>
#include <libm2k/analog/dmm.hpp>
#include <string>

using namespace libm2k::cli;

Context::Context(int argc, char **argv) : Command(argc, argv)
{
	if (context != nullptr) {
		dmm = context->getDMM("ad9963");
	}
}

bool Context::parseArguments(std::vector<std::pair<std::string, std::string>> &output)
{
	if (context == nullptr) {
		std::cout << helpMessage;
		return false;
	}

        int c, option_index = 0;
        bool quiet = false;
        while ((c = getopt_long(argc, argv, "hqafiLmstlupv",
                                options, &option_index)) != -1) {
                switch (c) {
                case 'h':
                    std::cout << helpMessage;
                    return quiet;
                case 'q':
                    quiet = true;
                    break;
                case 'a':
                    handleContextInfo(output);
                    break;
                case 'f':
                    handleFirmware(output);
                    break;
                case 'i':
                    handleIIO(output);
                    break;
                case 'L':
                    handleLinux(output);
                    break;
                case 'm':
                    handleModel(output);
                    break;
                case 's':
                    handleSerial(output);
                    break;
                case 't':
                    handleTemperature(output);
                    break;
                case 'l':
                    handleLibusb(output);
                    break;
                case 'u':
                    handleUri(output);
                    break;
                case 'p':
                    handleProduct(output);
                    break;
                case 'v':
                    handleVendor(output);
                    break;
                default:
                    break;
                }
        }
        return quiet;
}

void Context::handleContextInfo(std::vector<std::pair<std::string, std::string>> &output)
{
        addOutputMessage(output, ("Firmware version"),
                         context->getFirmwareVersion());
        auto iio = context->getIioContextVersion();
        addOutputMessage(output, ("IIO version"),
                         iio.git_tag);
        addOutputMessage(output, ("Linux"),
                         context->getContextDescription());
        addOutputMessage(output, ("Serial"),
                         context->getSerialNumber());
        auto temp = dmm->readChannel("temp0");
        addOutputMessage(output, ("Temperature"),
                         std::to_string(temp.value));
        addOutputMessage(output, ("uri"),
                         context->getUri());
	try {
                addOutputMessage(output, ("Model"),
                                context->getContextAttributeValue("hw_model"));
        }
        catch (...) {
                addOutputMessage(output, ("Model"), "Attribute unavailable");
        }
        try {
                addOutputMessage(output, ("usb,Product Name"),
                                context->getContextAttributeValue("usb,product"));
        }
        catch (...) {

                addOutputMessage(output, ("usb,Product Name"), "Attribute unavailable");
        }
 	try {
                addOutputMessage(output, ("usb,Vendor"),
                                context->getContextAttributeValue("usb,vendor"));
        }
        catch (...) {

                addOutputMessage(output, ("usb,Vendor"), "Attribute unavailable");
        }
	try {
                addOutputMessage(output, ("usb,libusub"),
                                context->getContextAttributeValue("usb,libusb"));
        }
        catch (...) {

                addOutputMessage(output, ("usb,libusb"), "Attribute unavailable");
        }
}

void Context::handleFirmware(std::vector<std::pair<std::string, std::string>> &output)
{
	addOutputMessage(output, ("Firmware version"),
			context->getFirmwareVersion());
}

void Context::handleIIO(std::vector<std::pair<std::string, std::string>> &output)
{
        auto iio = context->getIioContextVersion();
        addOutputMessage(output, ("IIO version"),
                         iio.git_tag);
}

void Context::handleLinux(std::vector<std::pair<std::string, std::string>> &output)
{
        addOutputMessage(output, ("Linux"),
                         context->getContextDescription());
}

void Context::handleModel(std::vector<std::pair<std::string, std::string>> &output)
{
	try {
        	addOutputMessage(output, ("Model"),
                         	context->getContextAttributeValue("hw_model"));
	}
	catch (...) {
		throw std::runtime_error("Attribute unavailable\n");
	}
}

void Context::handleSerial(std::vector<std::pair<std::string, std::string>> &output)
{
        addOutputMessage(output, ("Serial"),
                         context->getSerialNumber());
}

void  Context::handleTemperature(std::vector<std::pair<std::string, std::string>> &output)
{
        auto temp = dmm->readChannel("temp0");
        addOutputMessage(output, ("Temperature"),
                         std::to_string(temp.value));
}

void Context::handleLibusb(std::vector<std::pair<std::string, std::string>> &output)
{
        try {
		addOutputMessage(output, ("usb,libusb"),
                         context->getContextAttributeValue("usb,libusb"));
	}
	catch (...) {
		throw std::runtime_error("Attribute unavailable\n");
	}
}

void Context::handleUri(std::vector<std::pair<std::string, std::string>> &output)
{
        addOutputMessage(output, ("uri"),
                         context->getUri());
}

void Context::handleProduct(std::vector<std::pair<std::string, std::string>> &output)
{
        try {
		addOutputMessage(output, ("usb,Product Name"),
                         context->getContextAttributeValue("usb,product"));
	}
	catch (...) {
		throw std::runtime_error("Attribute unavailable\n");
	}
}

void Context::handleVendor(std::vector<std::pair<std::string, std::string>> &output)
{
        try {
		addOutputMessage(output, ("usb,Vendor"),
                         context->getContextAttributeValue("usb,vendor"));
	}
	catch (...) {
		throw std::runtime_error("Attribute unavailable\n");
	}
}

const struct option Context::options[] = {
        {"help",        no_argument,       nullptr, 'h'},
        {"quiet",       no_argument,       nullptr, 'q'},
        {"all",         no_argument,       nullptr, 'a'},
        {"firmware",    no_argument,       nullptr, 'f'},
        {"iio",         no_argument,       nullptr, 'i'},
        {"linux",       no_argument,       nullptr, 'L'},
        {"serial",      no_argument,       nullptr, 's'},
        {"temperature", no_argument,       nullptr, 't'},
        {"libusb",      no_argument,       nullptr, 'l'},
        {"uri",         no_argument,       nullptr, 'u'},
        {"product",     no_argument,       nullptr, 'p'},
        {"vendor",      no_argument,       nullptr, 'v'},
        {nullptr, 0,                       nullptr, 0},
};

const char *const Context::helpMessage = "Usage:\n"
                                         "m2kcli context <uri>\n"
                                         "                 [-h | --help]\n"
                                         "                 [-q | --quiet]\n"
                                         "                 [-a | --all]\n"
                                         "                 [-f | --firmware]\n"
                                         "                 [-i | --iio]\n"
                                         "                 [-L | --linux]\n"
                                         "                 [-s | --serial]\n"
                                         "                 [-t | --temperature]\n"
                                         "                 [-l | --libusb]\n"
                                         "                 [-u | --uri]\n"
                                         "                 [-p | --product]\n"
                                         "                 [-v | --vendor]\n"
                                         "\n"
                                         "Positional arguments:\n"
                                         "  uri                   describe the context location \n"
                                         "                        auto | ip:192.168.2.1 | usb:XX.XX.X\n"
                                         "Optional arguments:\n"
                                         "  -h, --help            show this help message and exit\n"
                                         "  -q, --quiet           return result only\n"
                                         "  -a, --all             show all context attributes\n"
                                         "  -f, --firmware        show firmware version\n"
                                         "  -i, --iio             show libiio version\n"
                                         "  -L, --linux           show linux version\n"
                                         "  -s, --serial          show serial number\n"
                                         "  -t, --temperature     show device temperature\n"
                                         "  -l, --libusb          show libusb version\n"
                                         "  -u, --uri             show uri\n"
                                         "  -p, --product         show product name\n"
                                         "  -v, --vendor          show vendor\n";
