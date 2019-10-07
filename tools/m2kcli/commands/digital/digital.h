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