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

#ifndef M2KCLI_DIGITAL_H
#define M2KCLI_DIGITAL_H

#include "../command_out.h"
#include "../command_in.h"
#include <libm2k/digital/m2kdigital.hpp>

namespace libm2k {
namespace cli {

class Digital : public CommandOut, public CommandIn {
public:
	Digital(int argc, char **argv);

	bool parseArguments(std::vector<std::pair<std::string, std::string>> &output) override;

private:
	libm2k::digital::M2kDigital *digital;

	static const struct option options[];

	void handleCapture();

	void handleGenerate();

	void handleGet(std::vector<std::pair<std::string, std::string>> &output);

	void handleGetChannel(std::vector<std::pair<std::string, std::string>> &output);

	void handleSet();

	void handleSetChannel();

	static const std::vector<const char *> triggerMode;
	static const std::vector<const char *> triggerCondition;
	static const std::vector<const char *> outputMode;

	static const char *const helpMessage;

};
}
}
#endif //M2KCLI_DIGITAL_H
