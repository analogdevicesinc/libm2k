/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * */

#ifndef M2KCLI_COMMAND_H
#define M2KCLI_COMMAND_H

#include <string>
#include <vector>
#include <getopt.h>
#include <libm2k/m2k.hpp>
#include "utils/validator.h"

namespace libm2k {
namespace cli {

class Command {
public:
	Command() = default;
	virtual ~Command() = default;

	Command(int argc, char **argv);

	virtual bool parseArguments(std::vector<std::pair<std::string, std::string>> &output) = 0;

	libm2k::context::M2k *getContext();

protected:
	libm2k::context::M2k *context;
	int argc;
	char **argv;

	std::vector<std::string> getArguments();

	static void
	addOutputMessage(std::vector<std::pair<std::string, std::string>> &output, const char *key, std::string value);

	static unsigned int getIndexOfElement(std::string element, const std::vector<const char *> &list);
};
}
}

#endif //M2KCLI_COMMAND_H
