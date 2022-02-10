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
#include <atomic>

constexpr unsigned int samplesPerCycle = 4;


std::vector<unsigned short> spi_create_buffer(struct spi_desc *desc,
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

			//set cs polarity
			if (m2KSpiDesc->cs_polarity == ACTIVE_HIGH) {
				setBit(sample, desc->chip_select);
			}

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

		//set cs polarity
		if (m2KSpiDesc->cs_polarity == ACTIVE_HIGH) {
			setBit(sample, desc->chip_select);
		}

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

		if (m2KSpiDesc->cs_polarity == ACTIVE_LOW) {
			setBit(sample, desc->chip_select);
		}

		if (clockPolarity) {
			setBit(sample, m2KSpiDesc->clock);
		}
		bufferOut.push_back(sample);
	}
	return bufferOut;
}

void processSamples(struct spi_desc *desc,
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
		 std::atomic<bool> &acquisition_started,
		 uint8_t *data,
		 uint8_t bytes_number)
{
	auto *m2KSpiDesc = (m2k_spi_desc *) desc->extra;
	auto samplesPerBit = (unsigned int) (m2KSpiDesc->sample_rate / desc->max_speed_hz);

	//set the trigger on CS
	libm2k::M2kHardwareTrigger *trigger = m2KSpiDesc->digital->getTrigger();

	if (m2KSpiDesc->cs_polarity == ACTIVE_HIGH) {
		trigger->setDigitalCondition(desc->chip_select, libm2k::RISING_EDGE_DIGITAL);
	} else {
		trigger->setDigitalCondition(desc->chip_select, libm2k::FALLING_EDGE_DIGITAL);
	}

	m2KSpiDesc->digital->startAcquisition((bytes_number + 1) * samplesPerBit * 8);

	//capture samples
	acquisition_started = true;
	std::vector<unsigned short> samples = m2KSpiDesc->digital->getSamples((bytes_number + 1) * samplesPerBit * 8);

	//process samples
	processSamples(desc, data, bytes_number, samples);
}

int32_t spi_init(struct spi_desc **desc,
		 const struct spi_init_param *param)
{
	uint32_t retVal;
	try {
		retVal = spi_init_write_only(desc, param);
		if (retVal != 0) {
			return retVal;
		}
		auto m2KSpiDesc = (m2k_spi_desc *) (*desc)->extra;
		auto m2KSpiInit = (m2k_spi_init *) param->extra;

		m2KSpiDesc->miso = m2KSpiInit->miso;
		m2KSpiDesc->digital->stopAcquisition();
		m2KSpiDesc->digital->setKernelBuffersCountIn(1);
		m2KSpiDesc->digital->setSampleRateIn(m2KSpiDesc->sample_rate);
		setInputChannel(m2KSpiDesc->miso, m2KSpiDesc->digital);
		m2KSpiDesc->digital->setOutputMode(m2KSpiDesc->miso, libm2k::digital::DIO_PUSHPULL);

		libm2k::M2kHardwareTrigger *trigger = m2KSpiDesc->digital->getTrigger();
		trigger->setDigitalCondition(m2KSpiDesc->clock, libm2k::NO_TRIGGER_DIGITAL);
		trigger->setDigitalCondition(m2KSpiDesc->mosi, libm2k::NO_TRIGGER_DIGITAL);
		trigger->setDigitalCondition(m2KSpiDesc->miso, libm2k::NO_TRIGGER_DIGITAL);
		trigger->setDigitalCondition((*desc)->chip_select, libm2k::NO_TRIGGER_DIGITAL);
		trigger->setDigitalMode(libm2k::digital::DIO_OR);
		trigger->setDigitalDelay(0);

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
		std::atomic<bool> acquisition_started(false);
		std::thread thread_read(read, desc, std::ref(acquisition_started), data, bytes_number);

		//make sure the reading thread is waiting
		while (!acquisition_started.load()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}

		std::vector<unsigned short> buffer = spi_create_buffer(desc, data, bytes_number);
		m2KSpiDesc->digital->push(buffer);
		thread_read.join();
	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}

int32_t spi_init_write_only(struct spi_desc **desc,
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
		m2KSpiDesc->bit_numbering = m2KSpiInit->bit_numbering;
		m2KSpiDesc->cs_polarity = m2KSpiInit->cs_polarity;
		m2KSpiDesc->context = m2KSpiInit->context;
		m2KSpiDesc->digital = m2KSpiDesc->context->getDigital();
		m2KSpiDesc->sample_rate = sampleRate;

		//set sampling frequencies
		m2KSpiDesc->digital->setSampleRateOut(m2KSpiDesc->sample_rate);

		//enable the channels
		setOutputChannel(spiDesc->chip_select, m2KSpiDesc->digital);
		setOutputChannel(m2KSpiDesc->clock, m2KSpiDesc->digital);
		setOutputChannel(m2KSpiDesc->mosi, m2KSpiDesc->digital);

		m2KSpiDesc->digital->setOutputMode(spiDesc->chip_select, libm2k::digital::DIO_PUSHPULL);
		m2KSpiDesc->digital->setOutputMode(m2KSpiDesc->clock, libm2k::digital::DIO_PUSHPULL);
		m2KSpiDesc->digital->setOutputMode(m2KSpiDesc->mosi, libm2k::digital::DIO_PUSHPULL);

		m2KSpiDesc->digital->setCyclic(false);

		//set the state of CS and CLK to idle
		if (m2KSpiDesc->cs_polarity == ACTIVE_HIGH) {
			m2KSpiDesc->digital->setValueRaw(spiDesc->chip_select, libm2k::digital::LOW);
		} else {
			m2KSpiDesc->digital->setValueRaw(spiDesc->chip_select, libm2k::digital::HIGH);
		}

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

int32_t spi_write_only(struct spi_desc *desc,
		       uint8_t *data,
		       uint8_t bytes_number)
{
	try {
		auto *m2KSpiDesc = (m2k_spi_desc *) desc->extra;
		std::vector<unsigned short> buffer = spi_create_buffer(desc, data, bytes_number);
		m2KSpiDesc->digital->push(buffer);
	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}

int32_t spi_write_and_read_samples(struct spi_desc *desc, std::vector<unsigned short> samples,
	uint8_t *data, uint8_t bytes_number)
{
	try {
		auto *m2KSpiDesc = (m2k_spi_desc *) desc->extra;
		std::atomic<bool> acquisition_started(false);
		std::thread thread_read(read, desc, std::ref(acquisition_started), data, bytes_number);

		//make sure the reading thread is waiting
		while (!acquisition_started.load()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}

		m2KSpiDesc->digital->push(samples);
		thread_read.join();
	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}
