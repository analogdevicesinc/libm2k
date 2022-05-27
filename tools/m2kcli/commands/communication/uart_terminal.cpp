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

#include "uart_terminal.h"
#include <thread>
#include <libm2k/tools/uart_extra.hpp>
#include "utils/linux_key_encoder.h"
#include "communication/src/utils/util.h"

#if defined(_WIN32) || defined(__CYGWIN__)
#include <conio.h>
#include <Windows.h>
#else
#include "utils/conio.h"
#endif

using namespace libm2k::cli;

UartTerminal::UartTerminal(int argc, char **argv) : Command(argc, argv)
{
	uartDescRx = nullptr;
	uartDescTx = nullptr;
	bufferSize= 8000;
	currentIndex = 0;
	data = new SafeQueue<uint8_t>();
}

UartTerminal::~UartTerminal()
{
	if (uartDescRx != nullptr) {
		uart_remove(uartDescRx);
	}
	if (uartDescTx != nullptr) {
		uart_remove(uartDescTx);
	}
}

bool UartTerminal::parseArguments(std::vector<std::pair<std::string, std::string>> &output)
{
	if (context == nullptr) {
		std::cout << helpMessage;
		return false;
	}
	int c, option_index = 0;
	while ((c = getopt_long(argc, argv, "hi:",
				options, &option_index)) != -1) {
		switch (c) {
			case 'h':
				std::cout << helpMessage;
				return false;
			case 'i':
				handleInit();
				break;
			default:
				break;
		}
	}
	return false;
}

void UartTerminal::handleInit()
{
	std::map<std::string, std::string> arguments = Validator::validate(getArguments());
	if (!(arguments.count("rx") && arguments.count("tx") && arguments.count("baud_rate") && arguments.count("parity") &&
	      arguments.count("bits_number") && arguments.count("stop_bits"))) {
		throw std::runtime_error(
			"Expecting: rx=<index> tx=<index> baud_rate=<value> parity=<value> bits_number=<value> stop_bits=<value>\n");
	}

	int rx;
	Validator::validate(arguments["rx"], "rx", rx);

	int tx;
	Validator::validate(arguments["tx"], "tx", tx);

	int baud_rate;
	Validator::validate(arguments["baud_rate"], "baud_rate", baud_rate);

	int bits_number;
	Validator::validate(arguments["bits_number"], "bits_number", bits_number);

	std::string stop_bits;
	Validator::validate(arguments["stop_bits"], "stop_bits", stop_bits);
	int enumIndexStop = getIndexOfElement(stop_bits, this->stop_bits);
	if (enumIndexStop == -1) {
		throw std::runtime_error("Invalid number of stop bits: '" + std::string(stop_bits) + "'.\n");
	}

	std::string parity;
	Validator::validate(arguments["parity"], "parity", parity);
	int enumIndexParity = getIndexOfElement(parity, this->parity);
	if (enumIndexParity == -1) {
		throw std::runtime_error("Invalid parity: '" + std::string(parity) + "'.\n");
	}
	m2k_uart_init m2KUartInit;
	m2KUartInit.bits_number = bits_number;
	m2KUartInit.parity = static_cast<uart_parity>(enumIndexParity);
	m2KUartInit.stop_bits = static_cast<uart_stop_bits>(enumIndexStop + 2);
	m2KUartInit.context = context;

	uart_init_param uartInitParam;
	uartInitParam.device_id = rx;
	uartInitParam.baud_rate = baud_rate;
	uartInitParam.extra = (void *) &m2KUartInit;

	if (uart_init(&uartDescRx, &uartInitParam) == -1) {
		throw std::runtime_error("Could not initiate UART RX\n");
	}

	uartInitParam.device_id = tx;
	if (uart_init(&uartDescTx, &uartInitParam) == -1) {
		throw std::runtime_error("Could not initiate UART TX\n");
	}

	auto *m2KUartDescRx = (m2k_uart_desc *) uartDescRx->extra;

	setInputChannel(rx, m2KUartDescRx->digital);

	m2KUartDescRx->digital->stopAcquisition();
	m2KUartDescRx->digital->setKernelBuffersCountIn(16);

	std::thread thread_decoder(&UartTerminal::processStream, this);
	std::thread thread_encoder(&UartTerminal::writeData, this);
	std::thread thread_read(&UartTerminal::readKeyboardInput, this);
	thread_decoder.join();
	thread_encoder.join();
	thread_read.join();
}

void UartTerminal::processStream()
{
	auto *m2KUartDesc = (m2k_uart_desc *) uartDescRx->extra;
	auto samplesPerCycle = (unsigned int) (m2KUartDesc->sample_rate / uartDescRx->baud_rate);
	samples = m2KUartDesc->digital->getSamples(bufferSize);

	uint8_t data = 0;

	while(true){
		waitForStart();

		//start
		readBit(samplesPerCycle);

		//data
		for (unsigned int j = 0; j < m2KUartDesc->bits_number; ++j) {
			if (readBit(samplesPerCycle)) {
				setBit(data, j);
			}
		}

		//parity
		if (m2KUartDesc->parity != NO_PARITY) {
			readBit(samplesPerCycle);
		}

		//stop
		readBit((samplesPerCycle / 2) * m2KUartDesc->stop_bits);

		std::cout << (unsigned char)data << std::flush;
		data = 0;
	}
}

bool UartTerminal::readBit(unsigned int samplesPerCycle)
{
	unsigned int diff = (samples.size() > currentIndex) ? samples.size() - currentIndex : currentIndex - samples.size();
	if (diff <= 2*samplesPerCycle) {
		reallocateBuffer();

		currentIndex = 0;
	}
	return getAverageValue(samples, currentIndex, samplesPerCycle, uartDescRx->device_id);
}

void UartTerminal::reallocateBuffer()
{
	auto *m2KUartDesc = (m2k_uart_desc *) uartDescRx->extra;

	std::vector<unsigned short> tempSamples;
	std::copy(samples.begin() + currentIndex, samples.end(), std::back_inserter(tempSamples));
	samples.clear();

	samples.reserve(bufferSize + tempSamples.size());
	samples.insert(samples.end(), tempSamples.begin(), tempSamples.end());

	tempSamples = m2KUartDesc->digital->getSamples(bufferSize);
	samples.insert(samples.end(), tempSamples.begin(), tempSamples.end());
}

void UartTerminal::waitForStart()
{
	auto *m2KUartDesc = (m2k_uart_desc *) uartDescRx->extra;

	while (true) {
		if (currentIndex == 0) {
			currentIndex++;
		}

		if (currentIndex >= samples.size()) {
			samples = m2KUartDesc->digital->getSamples(bufferSize);
			currentIndex = 1;
		}

		if (getBit(samples[currentIndex-1], uartDescRx->device_id) &&
		    !getBit(samples[currentIndex], uartDescRx->device_id)) {
			currentIndex++;
			break;
		}
		currentIndex++;
	}
}

void UartTerminal::readKeyboardInput() {
	uint8_t c;
	while (true) {
		c = _getch();
		data->push(c);
	}
}

void UartTerminal::writeData() {
	uint8_t c;
	while(true) {
		c = data->pop();
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
		if (c == 0xE0) {
			c = data->pop();
			try {
				auto escapeSequence = LinuxKeyEncoder::getLinuxCode(c);
				uart_write(uartDescTx, escapeSequence.data(), escapeSequence.size());
			} catch (std::runtime_error const&) {
				continue;
			}
		} else if (LinuxKeyEncoder::isExitCode(c)) {
			std::cout << std::endl;
			ExitProcess(0);
#else
		if (c == '\033') {
			std::vector<uint8_t> escapeSequence;
			escapeSequence.push_back(c);
			for(int i =0; i < 2; i++) {
				escapeSequence.push_back(data->pop());
			}

			try{
				auto linuxEscapeSequence = LinuxKeyEncoder::getLinuxCode(escapeSequence[2]);
				while (escapeSequence.size() < linuxEscapeSequence.size()) {
					escapeSequence.push_back(data->pop());
				}
				uart_write(uartDescTx, escapeSequence.data(), escapeSequence.size());

			} catch (std::runtime_error const&) {
				continue;
			}
#endif

		} else {
			uart_write(uartDescTx, &c, 1);
		}
	}
}

const struct option UartTerminal::options[] = {
	{"help",  no_argument,       nullptr, 'h'},
	{"init",  required_argument, nullptr, 'i'},
	{nullptr, 0,                 nullptr, 0},
};

const std::vector<const char *> UartTerminal::parity{"none", "odd", "even", "mark", "space"};

const std::vector<const char *> UartTerminal::stop_bits{"1", "1.5", "2"};

const char *const UartTerminal::helpMessage = "Usage:\n"
				      "m2kcli uart-terminal <uri>\n"
				      "           [-h | --help]\n"
				      "           [-i | --init rx=<index> tx=<index> baud_rate=<value> parity=<value> bits_number=<value> stop_bits=<value>]\n"
				      "\n"
				      "Positional arguments:\n"
				      "  uri                   describe the context location \n"
				      "Optional arguments:\n"
				      "  -h, --help            show this help message and exit\n"
				      "  -i, --init [rx=<index> tx=<index> baud_rate=<value> parity=<value> bits_number=<value> stop_bits=<value>]\n"
				      "                        initiate uart\n"
				      "                        rx - index of any digital pin (read)\n"
				      "                        tx - index of any digital pin (write)\n"
				      "                        baud_rate - int\n"
				      "                        parity = {none | odd | even | mark | space}\n"
				      "                        bits_number - {5 | 6 | 7 | 8}\n"
				      "                        stop_bits - {1 | 1.5 | 2}\n";
