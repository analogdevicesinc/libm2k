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

#include <libm2k/tools/spi.hpp>
#include <libm2k/tools/spi_extra.hpp>
#include "utils/util.h"
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/m2khardwaretrigger.hpp>
#include <thread>

constexpr unsigned int samplesPerCycle = 4;


static std::vector<unsigned short> createBuffer(struct spi_desc *desc,
						 uint8_t *data,
						 uint8_t bytesNumber)
{
	auto *m2KSpiDesc = (m2k_spi_desc *) desc->extra;
	std::vector<unsigned short> bufferOut;
	auto samplesPerHalfBit = (unsigned int) (m2KSpiDesc->sample_rate / desc->max_speed_hz) / 2;

	bool clockPolarity = (desc->mode & (unsigned) SPI_CPOL) >> 1u;
	bool phase = (desc->mode & (unsigned) SPI_CPHA);

	int cnt = 0;
	int boundary = 8;
	if (m2KSpiDesc->bit_numbering == MSB) {
		cnt = 7;
		boundary = -1;
	}
	int dataIndex = 0;

	for (int i = 0; i < bytesNumber * 16; ++i) {
		//move to the next byte in data
		if (cnt == boundary) {
			if (m2KSpiDesc->bit_numbering == MSB) {
				cnt = 7;
			} else {
				cnt = 0;
			}
			dataIndex++;
		}

		//encode the data - write half bit for half bit
		for (unsigned int j = 0; j < samplesPerHalfBit; ++j) {
			unsigned short sample = 0;

			//set clock polarity
			if (clockPolarity) {
				setBit(sample, m2KSpiDesc->clock);
			}

			//set mosi bit
			if (getBit(data[dataIndex], cnt)) {
				setBit(sample, m2KSpiDesc->mosi);
			}

			bufferOut.push_back(sample);
		}
		//move to the next bit in the current byte
		if (phase) {
			if (i % 2 == 0 && i > 0) {
				cnt += m2KSpiDesc->bit_numbering;
			}
		} else {
			if (i % 2 != 0) {
				cnt += m2KSpiDesc->bit_numbering;
			}
		}
		//change polarity
		clockPolarity = !clockPolarity;
	}

	//set clock to idle state
	for (unsigned int j = 0; j < samplesPerHalfBit; ++j) {
		unsigned short sample = 0;
		if (clockPolarity) {
			setBit(sample, m2KSpiDesc->clock);
		}
		if (phase) {
			//set mosi bit
			if (getBit(data[dataIndex], cnt)) {
				setBit(sample, m2KSpiDesc->mosi);
			}
		}
		bufferOut.push_back(sample);
	}

	//set cs to idle state
	for (unsigned int j = 0; j < samplesPerHalfBit; ++j) {
		unsigned short sample = 0;
		setBit(sample, desc->chip_select);
		if (clockPolarity) {
			setBit(sample, m2KSpiDesc->clock);
		}
		bufferOut.push_back(sample);
	}
	return bufferOut;
}

static void processSamples(struct spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytesNumber,
			   std::vector<unsigned short> &samples)
{
	auto *m2KSpiDesc = (m2k_spi_desc *) desc->extra;
	int cnt = 0;
	int boundary = 8;
	if (m2KSpiDesc->bit_numbering == MSB) {
		cnt = 7;
		boundary = -1;
	}
	int dataIndex = 0;

	unsigned short previousSample = samples.at(0);
	//decode the data
	for (auto it = samples.begin() + 1; it != samples.end(); ++it) {
		if (dataIndex == bytesNumber) {
			break;
		}
		//move to the next byte in data
		if (cnt == boundary) {
			if (m2KSpiDesc->bit_numbering == MSB) {
				cnt = 7;
			} else {
				cnt = 0;
			}
			dataIndex++;
		}
		//rising edge
		if (desc->mode == 0 || desc->mode == 3) {
			if (getBit(previousSample, m2KSpiDesc->clock) < getBit((*it), m2KSpiDesc->clock)) {
				if (getBit(*it, m2KSpiDesc->miso) == 0) {
					clearBit(data[dataIndex], cnt);
				} else {
					setBit(data[dataIndex], cnt);
				}
				cnt += m2KSpiDesc->bit_numbering;
			}
			//falling edge
		} else {
			if (getBit(previousSample, m2KSpiDesc->clock) > getBit((*it), m2KSpiDesc->clock)) {
				if (getBit(*it, m2KSpiDesc->miso) == 0) {
					clearBit(data[dataIndex], cnt);
				} else {
					setBit(data[dataIndex], cnt);
				}
				cnt += m2KSpiDesc->bit_numbering;
			}
		}
		previousSample = (*it);
	}
}

static void read(struct spi_desc *desc,
		 uint8_t *data,
		 uint8_t bytes_number)
{
	auto *m2KSpiDesc = (m2k_spi_desc *) desc->extra;
	auto samplesPerBit = (unsigned int) (m2KSpiDesc->sample_rate / desc->max_speed_hz);

	//set the trigger on CS
	libm2k::M2kHardwareTrigger *trigger = m2KSpiDesc->digital->getTrigger();
	trigger->setDigitalCondition(desc->chip_select, libm2k::FALLING_EDGE_DIGITAL);

	//capture samples
	std::vector<unsigned short> samples = m2KSpiDesc->digital->getSamples((bytes_number + 1) * samplesPerBit * 8);

	//process samples
	processSamples(desc, data, bytes_number, samples);
}

int32_t spi_init(struct spi_desc **desc,
		 const struct spi_init_param *param)
{
	try {
		auto spiDesc = new spi_desc;
		auto m2KSpiDesc = new m2k_spi_desc;
		m2k_spi_init *m2KSpiInit;

		// initialize the attributes
		unsigned int sampleRate = getValidSampleRate(param->max_speed_hz, samplesPerCycle);
		spiDesc->max_speed_hz = sampleRate / samplesPerCycle;
		spiDesc->mode = param->mode;
		spiDesc->chip_select = param->chip_select;

		m2KSpiInit = (m2k_spi_init *) param->extra;

		m2KSpiDesc->clock = m2KSpiInit->clock;
		m2KSpiDesc->mosi = m2KSpiInit->mosi;
		m2KSpiDesc->miso = m2KSpiInit->miso;
		m2KSpiDesc->bit_numbering = m2KSpiInit->bit_numbering;
		m2KSpiDesc->context = m2KSpiInit->context;
		m2KSpiDesc->digital = m2KSpiDesc->context->getDigital();
		m2KSpiDesc->sample_rate = sampleRate;

		m2KSpiDesc->digital->flushBufferIn();
		m2KSpiDesc->digital->setKernelBuffersCountIn(1);

		//set sampling frequencies
		m2KSpiDesc->digital->setSampleRateOut(m2KSpiDesc->sample_rate);
		m2KSpiDesc->digital->setSampleRateIn(m2KSpiDesc->sample_rate);

		//enable the channels
		setOutputChannel(spiDesc->chip_select, m2KSpiDesc->digital);
		setOutputChannel(m2KSpiDesc->clock, m2KSpiDesc->digital);
		setOutputChannel(m2KSpiDesc->mosi, m2KSpiDesc->digital);
		setInputChannel(m2KSpiDesc->miso, m2KSpiDesc->digital);

		m2KSpiDesc->digital->setOutputMode(spiDesc->chip_select, libm2k::digital::DIO_PUSHPULL);
		m2KSpiDesc->digital->setOutputMode(m2KSpiDesc->clock, libm2k::digital::DIO_PUSHPULL);
		m2KSpiDesc->digital->setOutputMode(m2KSpiDesc->mosi, libm2k::digital::DIO_PUSHPULL);
		m2KSpiDesc->digital->setOutputMode(m2KSpiDesc->miso, libm2k::digital::DIO_PUSHPULL);

		m2KSpiDesc->digital->setCyclic(false);

		libm2k::M2kHardwareTrigger *trigger = m2KSpiDesc->digital->getTrigger();
		trigger->setDigitalCondition(m2KSpiDesc->clock, libm2k::NO_TRIGGER_DIGITAL);
		trigger->setDigitalCondition(m2KSpiDesc->mosi, libm2k::NO_TRIGGER_DIGITAL);
		trigger->setDigitalCondition(m2KSpiDesc->miso, libm2k::NO_TRIGGER_DIGITAL);
		trigger->setDigitalCondition(spiDesc->chip_select, libm2k::NO_TRIGGER_DIGITAL);
		trigger->setDigitalMode(libm2k::digital::DIO_OR);
		trigger->setDigitalDelay(0);

		//set the state of CS and CLK to idle
		m2KSpiDesc->digital->setValueRaw(spiDesc->chip_select, libm2k::digital::HIGH);
		if (spiDesc->mode & SPI_CPOL) {
			m2KSpiDesc->digital->setValueRaw(m2KSpiDesc->clock, libm2k::digital::HIGH);
		} else {
			m2KSpiDesc->digital->setValueRaw(m2KSpiDesc->clock, libm2k::digital::LOW);
		}

		spiDesc->extra = (void *) m2KSpiDesc;
		*desc = spiDesc;

	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}

int32_t spi_remove(struct spi_desc *desc)
{
	try {
		auto m2KSpiDesc = (m2k_spi_desc *) desc->extra;
		delete m2KSpiDesc;
		m2KSpiDesc = nullptr;
		delete desc;
		desc = nullptr;
	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}

int32_t spi_write_and_read(struct spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number)
{
	try {
		auto *m2KSpiDesc = (m2k_spi_desc *) desc->extra;
		//start reading - wait until buffer is pushed
		std::thread thread_read(read, desc, data, bytes_number);

		//make sure the reading thread is waiting
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		std::vector<unsigned short> buffer = createBuffer(desc, data, bytes_number);
		m2KSpiDesc->digital->push(buffer);
		thread_read.join();
	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}
