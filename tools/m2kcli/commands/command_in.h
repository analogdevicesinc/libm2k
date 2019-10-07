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

#ifndef M2KCLI_COMMAND_IN_H
#define M2KCLI_COMMAND_IN_H

#include "command.h"

namespace libm2k {
namespace cli {

class CommandIn : virtual public Command {
protected:
	static void printSamplesCsvFormat(std::vector<uint16_t> &samples);

	static void printSamplesCsvFormat(std::vector<std::vector<double>> &samples);

	static void printSamplesBinaryFormat(std::vector<double> &samples);

	static void printSamplesBinaryFormat(std::vector<uint16_t> &samples);
};
}
}

#endif //M2KCLI_COMMAND_IN_H
