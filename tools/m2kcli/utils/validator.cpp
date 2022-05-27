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
#include <stdexcept>
#include <cctype>
#include "validator.h"

void Validator::validate(const std::string &argument, const char *argumentName, int &value)
{
	std::string strValue = validateName(argument, argumentName);
	if (is_number(strValue)) {
		value = std::stoi(strValue);
	} else if(is_hexa_number(strValue)) {
		value = std::stoul(strValue, nullptr, 16);
	} else {
		throw std::invalid_argument(std::string(argumentName) + " must be an integer\n");
	}
}

void Validator::validate(const std::string &argument, const char *argumentName, double &value)
{
	std::string strValue = validateName(argument, argumentName);
	validate(strValue, value);
}

void Validator::validate(const std::string &argument, const char *argumentName, bool &value)
{
	std::string strValue = validateName(argument, argumentName);
	if (strValue != "0" && strValue != "1") {
		throw std::invalid_argument("Possible values for raw : {0 | 1}\n");
	}
	value = static_cast<bool>(std::stoi(strValue));
}

void Validator::validate(const std::string &argument, const char *argumentName, std::string &value)
{
	value = validateName(argument, argumentName);
}

void Validator::validate(const std::string &argument, const char *argumentName, std::vector<unsigned int> &value)
{
	std::string strValue = validateName(argument, argumentName);
	std::istringstream iss(strValue);
	while (getline(iss, strValue, ',')) {
		if (!(is_number(strValue) && std::stoi(strValue) >= 0 && std::stoi(strValue) <= 15)) {
			throw std::runtime_error(
				"Possible values for digital channel : {0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 \n"
				"8 | 9 | 10 | 11 | 12 | 13 | 14 | 15}\n");
		}
		value.push_back(std::stoi(strValue));
	}
}

void Validator::validate(const std::string &argument, const char *argumentName, std::vector<uint8_t > &value)
{
	std::string strValue = validateName(argument, argumentName);
	std::istringstream iss(strValue);
	while (getline(iss, strValue, ',')) {
		if (is_number(strValue)) {
			value.push_back(std::stoi(strValue));
		} else if (is_hexa_number(strValue)) {
			value.push_back(std::stoul(strValue, nullptr, 16));
		} else {
			for (auto c : strValue) {
				value.push_back((uint8_t )c);
			}
		}

	}
}

std::map<std::string, std::string> Validator::validate(std::vector<std::string> arguments)
{
	std::map<std::string, std::string> values;
	for (auto it = arguments.begin(); it != arguments.end(); it++) {
		std::pair<std::string, std::string> value;
		if ((*it).find('=') == std::string::npos) {
			throw std::runtime_error("Expecting the following format: <argument>=<value>\n");
		}
		std::istringstream iss(*it);
		getline(iss, value.first, '=');
		value.second = *it;
		values.insert(value);
	}
	return values;
}

void Validator::validate(std::string strNumber, double &number)
{
	try {
		number = std::stod(strNumber);
	} catch (std::exception const& e) {
		throw std::runtime_error("'" + strNumber + "' is not a number.\n");
	}
}

void Validator::validate(std::string strNumber, uint16_t &number)
{
	try {
		number = std::stoi(strNumber);
	} catch (std::exception const& e) {
		throw std::runtime_error("'" + strNumber + "' is not a number.\n");
	}
}

bool Validator::validateUri(const std::string &uri)
{
	std::vector<std::string> uries{"usb:", "ip:", "local:", "xml:", "serial:"};
	for (auto &validUri : uries) {
		if (uri.find(validUri) != std::string::npos) {
			return true;
		}
	}
	return false;
}

std::string Validator::validateName(const std::string &argument, const char *argumentName)
{
	if (argument.find('=') == std::string::npos) {
		throw std::runtime_error("Expecting the following format: " + std::string(argumentName) + "=<value>\n");
	}
	std::istringstream iss(argument);
	std::string token;
	getline(iss, token, '=');
	if (token != argumentName) {
		throw std::runtime_error("Expecting the following format: " + std::string(argumentName) + "=<value>\n");
	}
	getline(iss, token, '=');
	return token;
}

bool Validator::is_number(const std::string &s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) {
		++it;
	}
	return !s.empty() && it == s.end();
}

bool Validator::is_hexa_number(const std::string &s)
{
	return s.compare(0, 2, "0x") == 0
	       && s.size() > 2
	       && s.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
}
