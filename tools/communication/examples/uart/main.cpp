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

#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/tools/uart.hpp>
#include <libm2k/tools/uart_extra.hpp>
#include <iostream>
#include <thread>

/*
 * Please check our wiki page: https://wiki.analog.com/university/courses/electronics/m2k-uart-debug
 */

int main()
{
	libm2k::contexts::M2k *context = libm2k::contexts::m2kOpen("ip:192.168.2.1");
	if (!context) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC.\n";
		return -1;
	}

	m2k_uart_init m2KUartInit;
	m2KUartInit.bits_number = 8;
	m2KUartInit.parity = NO_PARITY;
	m2KUartInit.stop_bits = ONE;
	m2KUartInit.context = context;

	uart_init_param uartInitParam;
	uartInitParam.device_id = 0;
	uartInitParam.baud_rate = 9600;
	uartInitParam.extra = (void*)&m2KUartInit;

	uart_desc *uartDescWrite = nullptr;
	uart_init(&uartDescWrite, &uartInitParam);

	uart_desc *uartDescRead = nullptr;
	uartInitParam.device_id = 1;
	uart_init(&uartDescRead, &uartInitParam);

	uint8_t dataRead[] = {0x0, 0x0, 0x0};
	std::thread threadRead(uart_read, uartDescRead, dataRead, sizeof(dataRead));
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	uint8_t dataWrite[] = {'A', 'D', 'I'};
	uart_write(uartDescWrite, dataWrite, sizeof(dataWrite));

	threadRead.join();

	for (auto &byte : dataRead) {
		std::cout << byte;
	}
	std::cout << std::endl;

	uart_remove(uartDescWrite);
	uart_remove(uartDescRead);
	return 0;
}
