/***************************************************************************//**
*   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/


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
	auto samplesPerQuarterBit = (unsigned int) (m2KI2CDesc->sample_rate / desc->max_speed_hz) / 4;

	//scl high, sda high
	for (unsigned int i = 0; i < samplesPerQuarterBit*2; ++i) {
		unsigned short sample = 0;
		setBit(sample, m2KI2CDesc->scl);
		setBit(sample, m2KI2CDesc->sda);
		buffer.push_back(sample);
	}
	//scl high, sda low
	for (unsigned int i = 0; i < samplesPerQuarterBit*2; ++i) {
		unsigned short sample = 0;
		setBit(sample, m2KI2CDesc->scl);
		buffer.push_back(sample);
	}
	//scL low, sda low
	for (unsigned int i = 0; i < samplesPerQuarterBit*4; ++i) {
		unsigned short sample = 0;
		buffer.push_back(sample);
	}
}

static void writeStopCondition(struct i2c_desc *desc, std::vector<unsigned short> &buffer)
{
	auto *m2KI2CDesc = (m2k_i2c_desc *) desc->extra;
	auto samplesPerQuarterBit = (unsigned int) (m2KI2CDesc->sample_rate / desc->max_speed_hz) / 4;

	//scl low, sda low
	for (unsigned int i = 0; i < samplesPerQuarterBit*2; ++i) {
		unsigned short sample = 0;
		buffer.push_back(sample);
	}
	//scl high, sda low
	for (unsigned int i = 0; i < samplesPerQuarterBit * 2; ++i) {
		unsigned short sample = 0;
		setBit(sample, m2KI2CDesc->scl);
		buffer.push_back(sample);
	}
	//scl high, sda high
	for (unsigned int i = 0; i < samplesPerQuarterBit * 4; ++i) {
		unsigned short sample = 0;
		setBit(sample, m2KI2CDesc->scl);
		setBit(sample, m2KI2CDesc->sda);
		buffer.push_back(sample);
	}
}

static void writeBit(struct i2c_desc *desc, std::vector<unsigned short> &buffer, bool bit)
{
	//during data transmission, SDA transitions must occur only when SCL is low
	auto *m2KI2CDesc = (m2k_i2c_desc *) desc->extra;
	auto samplesPerQuarterBit = (unsigned int) (m2KI2CDesc->sample_rate / desc->max_speed_hz) / 4;

	//sda transition; scl low 
	for (unsigned int i = 0; i < samplesPerQuarterBit; ++i) {
		unsigned short sample = 0;
		if (bit) {
			setBit(sample, m2KI2CDesc->sda);
		}
		buffer.push_back(sample);
	}
	//sda stable; scl high
	for (unsigned int i = 0; i < samplesPerQuarterBit*2; ++i) {
		unsigned short sample = 0;
		if (bit) {
			setBit(sample, m2KI2CDesc->sda);
		}
		setBit(sample, m2KI2CDesc->scl);
		buffer.push_back(sample);
	}
	//sda stable; scl low for next bit
	for (unsigned int i = 0; i < samplesPerQuarterBit; ++i) {
		unsigned short sample = 0;
		if (bit) {
			setBit(sample, m2KI2CDesc->sda);
		}
		buffer.push_back(sample);
	}
}

static void writeByte(struct i2c_desc *desc, std::vector<unsigned short> &buffer, uint8_t byte)
{
	//transmit the most significant bits of the byte first
	const size_t BITS_IN_BYTE = 8;
	for (size_t i = 0; i < BITS_IN_BYTE; i++) {
		auto bit = getBit(byte, 7 - i);
		writeBit(desc, buffer, bit);
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
