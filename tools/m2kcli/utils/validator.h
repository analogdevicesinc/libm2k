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

#ifndef M2KCLI_VALIDATOR_H
#define M2KCLI_VALIDATOR_H


#include <string>
#include <vector>
#include <map>

class Validator {
public:
	static void validate(const std::string &argument, const char *argumentName, int &value);

	static void validate(const std::string &argument, const char *argumentName, bool &value);

	static void validate(const std::string &argument, const char *argumentName, std::string &value);

	static void validate(const std::string &argument, const char *argumentName, std::vector<int> &value);

	static std::map<std::string, std::string> validate(std::vector<std::string> arguments);

	static void validate(std::string strNumber, double &number);

	static void validate(std::string strNumber, uint16_t &number);

	static bool validateUri(const std::string &uri);

private:
	static std::string validateName(const std::string &argument, const char *argumentName);

	static bool is_number(const std::string &s);
};


#endif //M2KCLI_VALIDATOR_H