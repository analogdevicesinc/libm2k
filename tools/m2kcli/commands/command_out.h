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

#ifndef M2KCLI_COMMAND_OUT_H
#define M2KCLI_COMMAND_OUT_H

#include "command.h"
#include <limits>

namespace libm2k {
namespace cli {

class CommandOut : virtual public Command {
protected:

	static void getSamplesCsvFormat(std::string &file, std::vector<uint16_t> &samples);

	static void getSamplesCsvFormat(std::string &file, std::vector<std::vector<short>> &samples);

	static void getSamplesCsvFormat(std::string &file, std::vector<std::vector<double>> &samples);

	static void getSamplesBinaryFormat(std::string &file, std::vector<uint16_t> &samples);

	static void getSamplesBinaryFormat(std::string &file, std::vector<double> &samples);

};
}
}

#endif //M2KCLI_COMMAND_OUT_H
