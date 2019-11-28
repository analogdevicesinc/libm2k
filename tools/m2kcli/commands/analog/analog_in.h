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

#ifndef M2KCLI_ANALOG_IN_H
#define M2KCLI_ANALOG_IN_H

#include <libm2k/m2k.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include "../command_in.h"

namespace libm2k {
namespace cli {


class AnalogIn : public CommandIn {
public:
	AnalogIn(int argc, char **argv);

	bool parseArguments(std::vector<std::pair<std::string, std::string>> &output) override;

private:
	libm2k::analog::M2kAnalogIn *analogIn;

	static const struct option options[];

	void handleCalibration();

	void handleVoltage(std::vector<std::pair<std::string, std::string>> &output);

	void handleCapture();

	void handleGet(std::vector<std::pair<std::string, std::string>> &output);

	void handleGetChannel(std::vector<std::pair<std::string, std::string>> &output);

	void handleSet();

	void handleSetChannel();

	static const std::vector<const char *> triggerSource;
	static const std::vector<const char *> range;
	static const std::vector<const char *> triggerCondition;
	static const std::vector<const char *> triggerMode;

	static const char *const helpMessage;
};
}
}


#endif //M2KCLI_ANALOG_IN_H
