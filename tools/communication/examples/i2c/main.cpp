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
#include <libm2k/tools/i2c.hpp>
#include <libm2k/tools/i2c_extra.hpp>
#include <iostream>


/*
 * This example uses EVAL-ADT7420-PMDZ as a slave
 * Hardware configuration:
 * 	(ADALM2000) DIO_0 <---> Pin 1 (ADT7420) <--->  10 kilohms resistor <--- V+ (ADALM2000)
 * 	(ADALM2000) DIO_1 <---> Pin 3 (ADT7420) <---> 10 kilohms resistor <--- V+ (ADALM2000)
 * 	(ADALM2000) GND <---> Pin 5 (ADT7420)
 * 	(ADALM2000) V+ ---> Pin 7 (ADT7420)
*/


float convertTemperature(uint8_t *data)
{
	uint8_t msb_temp = data[0];
	uint8_t lsb_temp = data[1];
	uint16_t temp = 0;
	float temperature = 0;

	temp = ((uint16_t)msb_temp << 8u) + lsb_temp;
	if(temp & 0x8000) {
		/*! Negative temperature */
		temperature = (float) ((int32_t) temp - 65536) / 128;
	}
	else {
		/*! Positive temperature */
		temperature = (float) temp / 128;
	}
	return temperature;
}

int main()
{
	libm2k::context::M2k *context = libm2k::context::m2kOpen("ip:192.168.2.1");
	if (!context) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC.\n";
		return -1;
	}
	std::cout << "Calibrating ADC . . .\n";
	context->calibrateADC();

	libm2k::analog::M2kPowerSupply *powerSupply = context->getPowerSupply();

	powerSupply->enableChannel(0, true);
	powerSupply->pushChannel(0, 3.3);

	//m2k initial struct (included from i2c_extra header)
	m2k_i2c_init m2KI2CInit;
	m2KI2CInit.scl = 0;
	m2KI2CInit.sda = 1;
	m2KI2CInit.context = context;

	i2c_init_param i2CInitParam;
	i2CInitParam.max_speed_hz = 100000;
	i2CInitParam.slave_address = 0x48;
	i2CInitParam.extra = (void*)&m2KI2CInit;

	i2c_desc *desc = nullptr;
	i2c_init(&desc, &i2CInitParam);

	std::cout << "Initiating I2C transfer . . .\n";
	uint8_t  data_write_config[] = {0x0B};
	uint8_t  data_read_config[] = {0};
	//7-bit addressing and repeated start
	i2c_write(desc, data_write_config, sizeof(data_write_config), i2c_general_call | i2c_repeated_start);
	//only 7-bit addressing
	i2c_read(desc, data_read_config, sizeof(data_read_config), i2c_general_call);

	std::cout << "Reading the temperature . . .\n";
	uint8_t  data_write_temperature[] = {0};
	uint8_t  data_read_temperature[] = {0, 0};
	//7-bit addressing and repeated start
	i2c_write(desc, data_write_temperature, sizeof(data_write_temperature), i2c_general_call | i2c_repeated_start);
	//only 7-bit addressing
	i2c_read(desc, data_read_temperature, sizeof(data_read_temperature), i2c_general_call);

	float temperature = convertTemperature(data_read_temperature);
	std::cout << "Temperature: " << temperature << "\u2103\n";

	i2c_remove(desc);
	libm2k::context::contextClose(context, true);
	return 0;
}
