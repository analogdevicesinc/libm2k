/***************************************************************************//**
*   @author Cristian Pop (cristian.pop@analog.com)
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


#include <libm2k/tools/uart.hpp>
#include <libm2k/tools/uart_extra.hpp>
#include "utils/util.h"
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/m2khardwaretrigger.hpp>
#include <bitset>


constexpr unsigned int samplesPerCycle = 8;

static bool getParityBit(struct uart_desc *desc, uint8_t byte)
{
	auto *m2KUartDesc = (m2k_uart_desc *) desc->extra;
	switch (m2KUartDesc->parity) {
		case ODD:
			return std::bitset<8>(byte).count() % 2 == 0;
		case EVEN:
			return std::bitset<8>(byte).count() % 2 != 0;
		case MARK:
			return true;
		case SPACE:
			return false;
		default:
			return false;
	}

}

static void processSamples(struct uart_desc *desc,
			   uint8_t *data,
			   uint8_t bytesNumber,
			   std::vector<unsigned short> &samples)
{
	auto *m2KUartDesc = (m2k_uart_desc *) desc->extra;
	unsigned int currentIndex = 0;
	for (int i = 0; i < bytesNumber; ++i) {
		while (getBit(samples[currentIndex], desc->device_id)) {
			currentIndex++;
		}
		//start
		if (getAverageValue(samples, currentIndex, samplesPerCycle, desc->device_id)) {
			m2KUartDesc->total_error_count++;
		}
		//data
		for (unsigned int j = 0; j < m2KUartDesc->bits_number; ++j) {
			if(getAverageValue(samples, currentIndex, samplesPerCycle, desc->device_id)) {
				setBit(data[i], j);
			}
		}
		//parity
		if (m2KUartDesc->parity != NO_PARITY) {
			if (getParityBit(desc, data[i]) != getAverageValue(samples, currentIndex, samplesPerCycle, desc->device_id)) {
				m2KUartDesc->total_error_count++;
			}
		}
		//stop
		if (!getAverageValue(samples, currentIndex, (samplesPerCycle / 2) * m2KUartDesc->stop_bits, desc->device_id)) {
			m2KUartDesc->total_error_count++;
		}
	}
}

int32_t uart_init(struct uart_desc **desc, const struct uart_init_param *param)
{
	try {
		auto *uartDesc = new uart_desc;
		auto *m2KUartDesc = new m2k_uart_desc;
		m2k_uart_init *m2KUartInit;

		// initialize the attributes
		unsigned int sampleRate = getValidSampleRate(param->baud_rate, samplesPerCycle);
		uartDesc->baud_rate = param->baud_rate;
		uartDesc->device_id = param->device_id;

		m2KUartInit = (m2k_uart_init *) param->extra;

		if (m2KUartInit->bits_number > 8 || m2KUartInit->bits_number < 5) {
			throw std::runtime_error("Invalid number of bits\n");
		}
		m2KUartDesc->parity = m2KUartInit->parity;
		m2KUartDesc->bits_number = m2KUartInit->bits_number;
		m2KUartDesc->stop_bits = m2KUartInit->stop_bits;
		m2KUartDesc->context = m2KUartInit->context;
		m2KUartDesc->digital = m2KUartDesc->context->getDigital();
		m2KUartDesc->sample_rate = sampleRate;
		m2KUartDesc->total_error_count = 0;

		m2KUartDesc->digital->stopAcquisition();
		m2KUartDesc->digital->setKernelBuffersCountIn(1);

		//set sampling frequencies
		m2KUartDesc->digital->setSampleRateOut(m2KUartDesc->sample_rate);
		m2KUartDesc->digital->setSampleRateIn(m2KUartDesc->sample_rate);

		//enable the channels
		m2KUartDesc->digital->setOutputMode(uartDesc->device_id, libm2k::digital::DIO_PUSHPULL);

		m2KUartDesc->digital->setCyclic(false);

		libm2k::M2kHardwareTrigger *trigger = m2KUartDesc->digital->getTrigger();
		trigger->setDigitalCondition(uartDesc->device_id, libm2k::NO_TRIGGER_DIGITAL);
		trigger->setDigitalMode(libm2k::digital::DIO_OR);
		trigger->setDigitalDelay(0);

		//set the state of SCL and SDA to idle
		m2KUartDesc->digital->setValueRaw(uartDesc->device_id, libm2k::digital::HIGH);

		uartDesc->extra = (void *) m2KUartDesc;
		*desc = uartDesc;

	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}

int32_t uart_remove(struct uart_desc *desc)
{
	try {
		auto *m2KUartDesc = (m2k_uart_desc *) desc->extra;
		delete m2KUartDesc;
		m2KUartDesc = nullptr;
		delete desc;
		desc = nullptr;
	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}

int32_t uart_read(struct uart_desc *desc, uint8_t *data, uint32_t bytes_number)
{
	try {
		auto *m2KUartDesc = (m2k_uart_desc *) desc->extra;
		desc->baud_rate = m2KUartDesc->sample_rate / samplesPerCycle;
		auto samplesPerBit = (unsigned int) (m2KUartDesc->sample_rate / desc->baud_rate);

		//start
		unsigned int nb_samples = samplesPerBit;
		//data size
		nb_samples += samplesPerBit * (bytes_number * m2KUartDesc->bits_number);
		//stop
		nb_samples += samplesPerBit/2 * m2KUartDesc->stop_bits;
		//parity
		if (m2KUartDesc->parity != NO_PARITY) {
			nb_samples += samplesPerBit;
		}

		//capture samples
		std::vector<unsigned short> samples = m2KUartDesc->digital->getSamples(nb_samples);
		processSamples(desc, data, bytes_number, samples);

	} catch (std::exception &e){
		std::cout << e.what();
		return -1;
	}
	return 0;
}

int32_t uart_write(struct uart_desc *desc, const uint8_t *data,
			      uint32_t bytes_number)
{
	try {
		auto *m2KUartDesc = (m2k_uart_desc *) desc->extra;
		double sampleRate = 10000000;
		m2KUartDesc->sample_rate = sampleRate;
		m2KUartDesc->digital->setSampleRateOut(sampleRate);

		std::vector<unsigned short> bufferOut;
		auto samplesPerBit = (unsigned int) (m2KUartDesc->sample_rate / desc->baud_rate);
		setOutputChannel(desc->device_id, m2KUartDesc->digital);

		for (unsigned int i = 0; i < bytes_number; ++i) {

			//start
			for (unsigned int j = 0; j < samplesPerBit; ++j) {
				unsigned short sample = 0;
				setBit(sample, desc->device_id);
				bufferOut.push_back(sample);
			}
			for (unsigned int j = 0; j < samplesPerBit; ++j) {
				bufferOut.push_back(0);
			}

			//data
			for (unsigned int j = 0; j < m2KUartDesc->bits_number; ++j) {
				for (unsigned int k = 0; k < samplesPerBit; ++k) {
					unsigned short sample = 0;
					if (getBit(data[i], j)) {
						setBit(sample, desc->device_id);
					}
					bufferOut.push_back(sample);
				}
			}
			//parity
			if (m2KUartDesc->parity != NO_PARITY) {
				for (unsigned int j = 0; j < samplesPerBit; ++j) {
					unsigned short sample = 0;
					if (getParityBit(desc, data[i])) {
						setBit(sample, desc->device_id);
					}
					bufferOut.push_back(sample);
				}
			}
			//stop bits
			for (unsigned int j = 0; j < m2KUartDesc->stop_bits; ++j) {
				for (unsigned int k = 0; k < samplesPerBit / 2; ++k) {
					unsigned short sample = 0;
					setBit(sample, desc->device_id);
					bufferOut.push_back(sample);
				}
			}
		}
		m2KUartDesc->digital->push(bufferOut);
	} catch (std::exception &e) {
		std::cout << e.what();
		return -1;
	}
	return 0;
}

uint32_t uart_get_errors(struct uart_desc *desc)
{
	auto *m2KUartDesc = (m2k_uart_desc *) desc->extra;
	return m2KUartDesc->total_error_count;
}
