/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software; you can redistribute it and/or modify
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


#ifndef LIBM2K_UART_TERMINAL_H
#define LIBM2K_UART_TERMINAL_H

#include <libm2k/tools/uart.hpp>
#include "commands/command.h"
#include "utils/safe_queue.h"

namespace libm2k {
namespace cli {

class UartTerminal : public Command {
public:
	UartTerminal(int argc, char **argv);

	~UartTerminal();

	bool parseArguments(std::vector<std::pair<std::string, std::string>> &output) override;

private:
	uart_desc *uartDescRx;
	uart_desc *uartDescTx;

	std::vector<unsigned short> samples;
	unsigned int currentIndex;
	unsigned int bufferSize;
	SafeQueue<uint8_t> *data;

	static const struct option options[];

	void handleInit();

	void readKeyboardInput();

	void processStream();

	bool readBit(unsigned int samplesOneCycle);

	void reallocateBuffer();

	void waitForStart();

	void writeData();

	static const std::vector<const char *> parity;

	static const std::vector<const char *> stop_bits;

	static const char *const helpMessage;
};
}
}

#endif //LIBM2K_UART_TERMINAL_H
