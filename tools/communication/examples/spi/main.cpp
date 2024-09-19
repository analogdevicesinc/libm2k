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

#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/digital/m2kdigital.hpp>
#include <libm2k/tools/spi.hpp>
#include <libm2k/tools/spi_extra.hpp>
#include <numeric>
#include <iostream>

/*
 * Please check Electronics Lab 14 on our wiki page: https://wiki.analog.com/university/courses/electronics/electronics-lab-14
 * Configure the connections for 'Unipolar output operation' mode
 */

#define VOLTAGE 2.5

void getRegisterData(double voltage, uint8_t *data)
{
	//only positive values for voltage
	if (voltage < 0) {
		data[0] = 0;
		data[1] = 0;
		return;
	}
	//output voltage up to 4.095V
	if (voltage > 4.095) {
		data[0] = 0x0F;
		data[1] = 0xFF;
		return;
	}
	auto registerValue = (unsigned short) (voltage * 1000);
	data[0] = (uint8_t) ((registerValue & 0xFF00u) >> 8u);
	data[1] = (uint8_t) (registerValue & 0xFFu);
}

int main()
{
	libm2k::context::M2k *context = libm2k::context::m2kOpen("ip:192.168.2.1");
	if (!context) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC.\n";
		return -1;
	}
	context->calibrateADC();

	libm2k::analog::M2kAnalogIn *analogIn = context->getAnalogIn();
	libm2k::analog::M2kPowerSupply *powerSupply = context->getPowerSupply();
	libm2k::digital::M2kDigital *digital = context->getDigital();

	//setup analog in
	analogIn->setOversamplingRatio(1);
	analogIn->setSampleRate(1000000);
	analogIn->enableChannel(0, true);
	analogIn->enableChannel(1, false);
	analogIn->setRange(libm2k::analog::ANALOG_IN_CHANNEL_1, libm2k::analog::PLUS_MINUS_25V);

	//enable LDAC
	digital->setDirection(3, libm2k::digital::DIO_OUTPUT);
	digital->enableChannel(3, true);

	//enable CLR
	digital->setDirection(4, libm2k::digital::DIO_OUTPUT);
	digital->enableChannel(4, true);

	//setup SPI
	m2k_spi_init m2KSpiInit;
	m2KSpiInit.clock = 1;
	m2KSpiInit.mosi = 2;
	m2KSpiInit.miso = 7; //dummy value - miso is not used in this example
	m2KSpiInit.bit_numbering = MSB;
	m2KSpiInit.cs_polarity = ACTIVE_LOW;
	m2KSpiInit.context = context;

	spi_init_param spiInitParam;
	spiInitParam.max_speed_hz = 1000000;
	spiInitParam.mode = SPI_MODE_3;
	spiInitParam.chip_select = 0;
	spiInitParam.extra = (void*)&m2KSpiInit;

	spi_desc *desc = nullptr;
	if (spi_init(&desc, &spiInitParam) == -1) {
		std::cout << "SPI Error: Could not configure SPI\n";
		return -1;
	}

	powerSupply->enableChannel(0, true);
	powerSupply->pushChannel(0, 5);

	//CLR and LDAC high as long as bits are transmitted
	digital->setValueRaw(3, libm2k::digital::HIGH);
	digital->setValueRaw(4, libm2k::digital::HIGH);

	uint8_t data[2];
	getRegisterData(VOLTAGE, data);
	//transmitting data
	if (spi_write_and_read(desc, data, 2) == -1) {
		std::cout << "SPI Error: Could not transmit the data\n";
		return -1;
	}

	//update with current shift register contents
	digital->setValueRaw(3, libm2k::digital::LOW);

	std::vector<std::vector<double>> samples = analogIn->getSamples(100);

	double average = std::accumulate(samples[0].begin(), samples[0].end(), 0.0) / samples[0].size();
	std::cout << "Average value: " << average << " V" << std::endl;

	spi_remove(desc);
	libm2k::context::contextClose(context, true);
	return 0;
}
