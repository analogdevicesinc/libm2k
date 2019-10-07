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

#ifndef M2KCLI_ANALOG_OUT_H
#define M2KCLI_ANALOG_OUT_H

#include "../command_out.h"
#include <libm2k/m2k.hpp>
#include <libm2k/analog/m2kanalogout.hpp>

namespace libm2k {
namespace cli {

class AnalogOut : public CommandOut {
public:
	AnalogOut(int argc, char **argv);

	bool parseArguments(std::vector<std::pair<std::string, std::string>> &output) override;

private:
	libm2k::analog::M2kAnalogOut *analogOut;

	static const struct option options[];

	void handleCalibration();

	void handleGenerate();

	void handleGetChannel(std::vector<std::pair<std::string, std::string>> &output);

	void handleSetChannel();

	static const char *const helpMessage;
};
}
}

#endif //M2KCLI_ANALOG_OUT_H