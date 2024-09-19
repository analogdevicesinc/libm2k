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

#ifndef M2KCLI_COMMAND_IN_H
#define M2KCLI_COMMAND_IN_H

#include "command.h"

namespace libm2k {
namespace cli {

class CommandIn : virtual public Command {
public:
	virtual ~CommandIn() = default;

protected:
	static void printSamplesCsvFormat(std::vector<uint16_t> &samples, unsigned int nb_samples);

	static void printSamplesCsvFormat(std::vector<std::vector<double>> &samples, unsigned int nb_samples,
					  std::vector<unsigned int> &channels);

	static void printSamplesBinaryFormat(std::vector<std::vector<double>> &samples, unsigned int nb_samples,
					     std::vector<unsigned int> &channels);

	static void printRawSamplesBinaryFormat(std::vector<std::vector<double>> &samples, unsigned int nb_samples,
						std::vector<unsigned int> &channels);

	static void printSamplesBinaryFormat(std::vector<uint16_t> &samples, unsigned int nb_samples);
};
}
}

#endif //M2KCLI_COMMAND_IN_H
