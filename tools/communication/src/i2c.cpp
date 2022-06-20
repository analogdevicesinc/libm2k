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

#include <libm2k/tools/i2c.hpp>
#include <libm2k/tools/i2c_extra.hpp>
#include "utils/util.h"
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/m2khardwaretrigger.hpp>
#include <thread>
#include <atomic>

constexpr unsigned int samplesPerCycle = 4;
constexpr uint8_t condition10BitAddressing = 0x1E;

typedef struct i2c_data {
	uint8_t data;
	bool acknowledge;
} i2c_data;


static void writeStartCondition(struct i2c_desc *desc, std::vector<unsigned short> &buffer)
{
	auto *m2KI2CDesc = (m2k_i2c_desc *) desc->extra;
	auto samplesPerBit = (unsigned int) (m2KI2CDesc->sample_rate / desc->max_speed_hz);
	//scl low, sda high - avoid to be interpreted as a stop condition
	for (unsigned int i = 0; i < samplesPerBit; ++i) {
		unsigned short sample = 0;
		setBit(sample, m2KI2CDesc->sda);
		buffer.push_back(sample);
	}
	//both high
	for (unsigned int i = 0; i < samplesPerBit; ++i) {
		unsigned short sample = 0;
		setBit(sample, m2KI2CDesc->scl);
		setBit(sample, m2KI2CDesc->sda);
		buffer.push_back(sample);
	}
	//scl high, sda low
	for (unsigned int i = 0; i < samplesPerBit; ++i) {
		unsigned short sample = 0;
		setBit(sample, m2KI2CDesc->scl);
		buffer.push_back(sample);
	}
	//both low
	for (unsigned int i = 0; i < samplesPerBit; ++i) {
		unsigned short sample = 0;
		buffer.push_back(sample);
	}
}

static void writeStopCondition(struct i2c_desc *desc, std::vector<unsigned short> &buffer)
{
	auto *m2KI2CDesc = (m2k_i2c_desc *) desc->extra;
	auto samplesPerBit = (unsigned int) (m2KI2CDesc->sample_rate / desc->max_speed_hz);
	//scl high, sda low
	for (unsigned int i = 0; i < samplesPerBit; ++i) {
		unsigned short sample = 0;
		setBit(sample, m2KI2CDesc->scl);
		buffer.push_back(sample);
	}
	//both high
	for (unsigned int i = 0; i < samplesPerBit; ++i) {
		unsigned short sample = 0;
		setBit(sample, m2KI2CDesc->scl);
		setBit(sample, m2KI2CDesc->sda);
		buffer.push_back(sample);
	}
}

static void writeBit(struct i2c_desc *desc, std::vector<unsigned short> &buffer, bool bit)
{
	auto *m2KI2CDesc = (m2k_i2c_desc *) desc->extra;
	auto samplesPerHalfBit = (unsigned int) (m2KI2CDesc->sample_rate / desc->max_speed_hz) / 2;
	//scl low
	for (unsigned int i = 0; i < samplesPerHalfBit; ++i) {
		unsigned short sample = 0;
		if (bit) {
			setBit(sample, m2KI2CDesc->sda);
		}
		buffer.push_back(sample);
	}
	//scl high
	for (unsigned int i = 0; i < samplesPerHalfBit; ++i) {
		unsigned short sample = 0;
		setBit(sample, m2KI2CDesc->scl);
		if (bit) {
			setBit(sample, m2KI2CDesc->sda);
		}
		buffer.push_back(sample);
	}
}

static void writeByte(struct i2c_desc *desc, std::vector<unsigned short> &buffer, uint8_t byte)
{
	auto *m2KI2CDesc = (m2k_i2c_desc *) desc->extra;
	auto samplesPerHalfBit = (unsigned int) (m2KI2CDesc->sample_rate / desc->max_speed_hz) / 2;
	bool clockPolarity = false;

	for (int i = 0; i < 16; i++) {
		//encode data
		for (unsigned int j = 0; j < samplesPerHalfBit; ++j) {
			unsigned short sample = 0;
			if (clockPolarity) {
				setBit(sample, m2KI2CDesc->scl);
			}
			if (getBit(byte, 7 - (i / 2))) {
				setBit(sample, m2KI2CDesc->sda);
			}
			buffer.push_back(sample);
		}
		clockPolarity = !clockPolarity;
	}
}

static void writeAddress(struct i2c_desc *desc, std::vector<unsigned short> &buffer, uint8_t option, bool operation)
{
	uint8_t address;
	if (option & i2c_general_call) {
		address = (desc->slave_address << 1u);
		if (operation) {
			//read operation
			setBit(address, 0);
		}
		//write address
		writeByte(desc, buffer, address);
		//write acknowledge
		writeBit(desc, buffer, true);
	} else {
		address = condition10BitAddressing << 3u;
		address = address | ((desc->slave_address >> 7u) & 0x06);
		if (operation) {
			//read operation
			setBit(address, 0);
		}
		//write first address byte
		writeByte(desc, buffer, address);
		//write acknowledge
		writeBit(desc, buffer, true);

		address = (uint8_t) desc->slave_address;
		//write second address byte
		writeByte(desc, buffer, address);
		//write acknowledge
		writeBit(desc, buffer, true);
	}

}

static std::vector<unsigned short> createBuffer(struct i2c_desc *desc,
						uint8_t *data,
						uint8_t bytesNumber,
						uint8_t option,
						bool operation)
{
	std::vector<unsigned short> bufferOut;
	writeStartCondition(desc, bufferOut);
	writeAddress(desc, bufferOut, option, operation);

	for (int i = 0; i < bytesNumber; i++) {
		if (operation) {
			writeByte(desc, bufferOut, 0xFF);

		} else {
			//write data
			writeByte(desc, bufferOut, data[i]);
		}
		bool acknowledge = (i == bytesNumber - 1) ? true : !operation;
		//write acknowledge
		writeBit(desc, bufferOut, acknowledge);
	}
	if (!(option & i2c_repeated_start)) {
		writeStopCondition(desc, bufferOut);
	}
	return bufferOut;
}

static void processSamples(struct i2c_desc *desc,
			   std::atomic<bool> &acquisition_started,
			   std::vector<i2c_data> *data,
			   uint8_t bytesNumber)
{
	auto *m2KI2CDesc = (m2k_i2c_desc *) desc->extra;
	auto samplesPerBit = (unsigned int) (m2KI2CDesc->sample_rate / desc->max_speed_hz);
	libm2k::M2kHardwareTrigger *trigger = m2KI2CDesc->digital->getTrigger();
	trigger->setDigitalCondition(m2KI2CDesc->sda, libm2k::FALLING_EDGE_DIGITAL);
	trigger->setDigitalDelay(-samplesPerCycle);

	m2KI2CDesc->digital->startAcquisition((bytesNumber + 1) * 8 * samplesPerBit + bytesNumber *samplesPerBit);

	acquisition_started = true;
	std::vector<unsigned short> samples = m2KI2CDesc->digital->getSamples(
		(bytesNumber + 1) * 8 * samplesPerBit + bytesNumber *samplesPerBit);

	int cnt = 7;
	int dataIndex = 0;
	bool started = false;

	if (data->size() != bytesNumber) {
		*data = std::vector<i2c_data>(bytesNumber);
	}

	unsigned short previousSample = samples.at(0);
	for (auto it = samples.begin() + 1; it != samples.end(); ++it) {
		if (!started) {
			//sda falling edge, scl high
			if ((getBit(previousSample, m2KI2CDesc->sda) > getBit(*it, m2KI2CDesc->sda)) &&
			    (getBit(*it, m2KI2CDesc->scl) == 1)) {
				started = true;
				previousSample = (*it);
			}
			continue;
		}
		if (dataIndex == bytesNumber) {
			break;
		}

		//rising edge
		if (getBit(previousSample, m2KI2CDesc->scl) < getBit((*it), m2KI2CDesc->scl)) {
			if (cnt == -1) {
				//acknowledge
				(*data)[dataIndex].acknowledge = getBit((*(it + 1)), m2KI2CDesc->sda);
				cnt = 8;
				dataIndex++;
			} else {
				//data
				if (getBit(*(it), m2KI2CDesc->sda) == 0) {
					clearBit((*data)[dataIndex].data, cnt);
				} else {
					setBit((*data)[dataIndex].data, cnt);
				}
			}
			cnt--;
		}
		previousSample = (*it);
	}
}

int32_t i2c_init(struct i2c_desc **desc,
		 const struct i2c_init_param *param)
{
	uint32_t retVal;
	try {
		retVal = i2c_init_write_only(desc, param);
		if (retVal != 0) {
			return retVal;
		}

		auto m2KI2CDesc = (m2k_i2c_desc *) (*desc)->extra;

		m2KI2CDesc->digital->stopAcquisition();
		m2KI2CDesc->digital->setKernelBuffersCountIn(1);

		m2KI2CDesc->digital->setSampleRateIn(m2KI2CDesc->sample_rate);

		libm2k::M2kHardwareTrigger *trigger = m2KI2CDesc->digital->getTrigger();
		trigger->setDigitalCondition(m2KI2CDesc->scl, libm2k::NO_TRIGGER_DIGITAL);
		trigger->setDigitalCondition(m2KI2CDesc->sda, libm2k::NO_TRIGGER_DIGITAL);
		trigger->setDigitalMode(libm2k::digital::DIO_OR);
		trigger->setDigitalDelay(0);
	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}

int32_t i2c_remove(struct i2c_desc *desc)
{
	try {
		auto *m2KI2CDesc = (m2k_i2c_desc *) desc->extra;
		delete m2KI2CDesc;
		m2KI2CDesc = nullptr;
		delete desc;
		desc = nullptr;
	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}

int32_t i2c_write(struct i2c_desc *desc,
		  uint8_t *data,
		  uint8_t bytes_number,
		  uint8_t option)
{
	try {
		auto *m2KI2CDesc = (m2k_i2c_desc *) desc->extra;
		unsigned int totalNumberOfBytes = 0;
		if (option & i2c_10_bit_transfer) {
			totalNumberOfBytes += bytes_number + 2;
		} else {
			totalNumberOfBytes += bytes_number + 1;
		}
		auto *i2c_data_vector = new std::vector<i2c_data>(totalNumberOfBytes);

		std::atomic<bool> acquisition_started(false);
		std::thread thread_read(processSamples, desc, std::ref(acquisition_started), i2c_data_vector, totalNumberOfBytes);

		//make sure the reading thread is waiting
		while (!acquisition_started.load()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}

		//create buffer
		auto bufferOut = createBuffer(desc, data, bytes_number, option, false);
		m2KI2CDesc->digital->push(bufferOut);
		//process samples
		thread_read.join();
		//validate data
		unsigned int numberAddressBytes = 0;
		if (option & i2c_10_bit_transfer) {
			numberAddressBytes = 2;
		} else {
			numberAddressBytes = 1;
		}
		for (unsigned int i = 0; i < numberAddressBytes; ++i) {
			if ((*i2c_data_vector)[i].acknowledge) {
				throw std::runtime_error("Unable to find slave device - invalid address\n");
			}
		}
		for (unsigned int i = numberAddressBytes; i < bytes_number; ++i) {
			if ((*i2c_data_vector)[i].acknowledge) {
				throw std::runtime_error("Slave device is unable to receive the data\n");
			}
		}
	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}

int32_t i2c_read(struct i2c_desc *desc,
		 uint8_t *data,
		 uint8_t bytes_number,
		 uint8_t option)
{
	try {
		auto *m2KI2CDesc = (m2k_i2c_desc *) desc->extra;
		unsigned int totalNumberOfBytes = 0;
		if (option & i2c_10_bit_transfer) {
			totalNumberOfBytes += bytes_number + 2;
		} else {
			totalNumberOfBytes += bytes_number + 1;
		}
		auto *i2c_data_vector = new std::vector<i2c_data>(totalNumberOfBytes);

		std::atomic<bool> acquisition_started(false);
		std::thread thread_read(processSamples, desc, std::ref(acquisition_started), i2c_data_vector, totalNumberOfBytes);

		//make sure the reading thread is waiting
		while (!acquisition_started.load()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}

		//create buffer
		auto bufferOut = createBuffer(desc, data, bytes_number, option, true);
		m2KI2CDesc->digital->push(bufferOut);
		//process samples
		thread_read.join();
		//validate data
		unsigned int numberAddressBytes = 0;
		if (option & i2c_10_bit_transfer) {
			numberAddressBytes = 2;
		} else {
			numberAddressBytes = 1;
		}
		for (unsigned int i = 0; i < numberAddressBytes; ++i) {
			if ((*i2c_data_vector)[i].acknowledge) {
				throw std::runtime_error("Unable to find slave device - invalid address\n");
			}
		}
		for (unsigned int i = numberAddressBytes; i < totalNumberOfBytes; ++i) {
			data[i - numberAddressBytes] = (*i2c_data_vector)[i].data;
		}
	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}

int32_t i2c_init_write_only(struct i2c_desc **desc,
			    const struct i2c_init_param *param)
{
	try {
		auto *i2cDesc = new i2c_desc;
		auto *m2KI2CDesc = new m2k_i2c_desc;
		m2k_i2c_init *m2KI2CInit;

		// initialize the attributes
		unsigned int sampleRate = getValidSampleRate(param->max_speed_hz, samplesPerCycle);
		i2cDesc->max_speed_hz = sampleRate / samplesPerCycle;
		i2cDesc->slave_address = param->slave_address;

		m2KI2CInit = (m2k_i2c_init *) param->extra;

		m2KI2CDesc->scl = m2KI2CInit->scl;
		m2KI2CDesc->sda = m2KI2CInit->sda;
		m2KI2CDesc->context = m2KI2CInit->context;
		m2KI2CDesc->digital = m2KI2CDesc->context->getDigital();
		m2KI2CDesc->sample_rate = sampleRate;

		//set sampling frequencies
		m2KI2CDesc->digital->setSampleRateOut(m2KI2CDesc->sample_rate);

		//enable the channels
		setOutputChannel(m2KI2CDesc->scl, m2KI2CDesc->digital);
		setOutputChannel(m2KI2CDesc->sda, m2KI2CDesc->digital);
		m2KI2CDesc->digital->setOutputMode(m2KI2CDesc->sda, libm2k::digital::DIO_OPENDRAIN);
		m2KI2CDesc->digital->setOutputMode(m2KI2CDesc->scl, libm2k::digital::DIO_OPENDRAIN);

		m2KI2CDesc->digital->setCyclic(false);

		//set the state of SCL and SDA to idle
		m2KI2CDesc->digital->setValueRaw(m2KI2CDesc->scl, libm2k::digital::HIGH);
		m2KI2CDesc->digital->setValueRaw(m2KI2CDesc->sda, libm2k::digital::HIGH);

		i2cDesc->extra = (void *) m2KI2CDesc;
		*desc = i2cDesc;

	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}

int32_t i2c_write_only(struct i2c_desc *desc,
		       uint8_t *data,
		       uint8_t bytes_number,
		       uint8_t option)
{
	try {
		auto *m2KI2CDesc = (m2k_i2c_desc *) desc->extra;
		auto bufferOut = createBuffer(desc, data, bytes_number, option, false);
		m2KI2CDesc->digital->push(bufferOut);
	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}
