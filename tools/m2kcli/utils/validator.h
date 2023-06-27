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

#ifndef M2KCLI_VALIDATOR_H
#define M2KCLI_VALIDATOR_H


#include <string>
#include <vector>
#include <map>
#include <cstdint>

class Validator {
public:
	static void validate(const std::string &argument, const char *argumentName, int &value);

	static void validate(const std::string &argument, const char *argumentName, double &value);

	static void validate(const std::string &argument, const char *argumentName, bool &value);

	static void validate(const std::string &argument, const char *argumentName, std::string &value);

	static void validate(const std::string &argument, const char *argumentName, std::vector<unsigned int> &value);

	static void validate(const std::string &argument, const char *argumentName, std::vector<uint8_t > &value);

	static std::map<std::string, std::string> validate(std::vector<std::string> arguments);

	static void validate(std::string strNumber, double &number);

	static void validate(std::string strNumber, uint16_t &number);

	static bool validateUri(const std::string &uri);

private:
	static std::string validateName(const std::string &argument, const char *argumentName);

	static bool is_number(const std::string &s);

	static bool is_hexa_number(const std::string &s);
};


#endif //M2KCLI_VALIDATOR_H
