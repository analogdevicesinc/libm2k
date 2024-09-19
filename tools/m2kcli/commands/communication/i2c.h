/*
 * Copyright 2019 Analog Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 */

#ifndef LIBM2K_I2C_H
#define LIBM2K_I2C_H

#include <commands/command.h>
#include <libm2k/tools/i2c.hpp>

namespace libm2k {
namespace cli {

class I2c : public Command{
public:
	I2c(int argc, char **argv);

	~I2c();

	bool parseArguments(std::vector<std::pair<std::string, std::string>> &output) override;

private:
	i2c_desc *i2cDesc;
	bool writeOnly;

	static const struct option options[];

	void handleInit();

	void handleWrite();

	void handleRead();

	static const char *const helpMessage;
};
}
}


#endif //LIBM2K_I2C_H
