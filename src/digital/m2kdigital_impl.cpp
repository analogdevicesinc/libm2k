/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software: you can redistribute it and/or modify
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

#include "m2kdigital_impl.hpp"
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/logger.hpp>
#include <libm2k/utils/utils.hpp>
#include <iio.h>
#include <iostream>
#include <algorithm>

using namespace libm2k;
using namespace libm2k::utils;
using namespace libm2k::digital;
using namespace libm2k::analog;
using namespace std;


std::vector<std::string> M2kDigitalImpl::m_output_mode = {
	"open-drain",
	"push-pull",
};

M2kDigitalImpl::M2kDigitalImpl(struct iio_context *ctx, std::string logic_dev, bool sync, M2kHardwareTrigger *trigger)
{
	LIBM2K_LOG(INFO, "[BEGIN] Initialize M2kDigital");
	__try {
		m_dev_generic = make_shared<DeviceGeneric>(ctx, logic_dev);
	} __catch (exception_type &e) {
		m_dev_generic = nullptr;
		THROW_M2K_EXCEPTION("M2K Digital: No generic digital device was found.", libm2k::EXC_INVALID_PARAMETER);
	}

	m_trigger = trigger;
	m_dev_name_write = logic_dev + "-tx";
	m_dev_name_read = logic_dev + "-rx";

	if (m_dev_name_write != "") {
		__try {
			m_dev_write = make_shared<DeviceOut>(ctx, m_dev_name_write);
		} __catch (exception_type &e) {
			m_dev_write = nullptr;
			THROW_M2K_EXCEPTION("M2K Digital: No device was found for writing", libm2k::EXC_INVALID_PARAMETER);
		}
	}

	if (m_dev_name_read != "") {
		__try {
			m_dev_read = make_shared<DeviceIn>(ctx, m_dev_name_read);
		} __catch (exception_type &e) {
			m_dev_read = nullptr;
			THROW_M2K_EXCEPTION("M2K Digital: No device was found for reading", libm2k::EXC_INVALID_PARAMETER);
		}
	}

	if (!m_dev_read || !m_dev_write) {
		m_dev_read = nullptr;
		m_dev_write = nullptr;
		THROW_M2K_EXCEPTION("M2K Digital: logic device not found", libm2k::EXC_INVALID_PARAMETER);
	}

	if (sync) {
		syncDevice();
	}
	LIBM2K_LOG(INFO, "[END] Initialize M2kDigital");
}

M2kDigitalImpl::~M2kDigitalImpl()
{
}

void M2kDigitalImpl::syncDevice()
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kDigital sync");
	for (unsigned int i = 0; i < m_dev_generic->getNbChannels(false); i++) {
		/* Disable all the TX channels */
		bool en = m_dev_write->isChannelEnabled(i, true);
		m_tx_channels_enabled.push_back(en);

		/* Enable all the RX channels */
		m_rx_channels_enabled.push_back(true);
		m_dev_read->enableChannel(i, true, false);
	}
	LIBM2K_LOG(INFO, "[END] M2kDigital sync");
}

void M2kDigitalImpl::reset()
{
	cancelAcquisition();
	cancelBufferOut();
	for (unsigned int i = 0; i < m_dev_generic->getNbChannels(false); i++) {
		/* Disable all the TX channels */

		m_tx_channels_enabled.push_back(false);
		enableChannel(i, false);

		/* Enable all the RX channels */
		m_rx_channels_enabled.push_back(true);
		m_dev_read->enableChannel(i, true, false);
	}

	setKernelBuffersCountIn(4);
	setKernelBuffersCountOut(4);
}

void M2kDigitalImpl::setKernelBuffersCountIn(unsigned int count)
{
	m_dev_read->setKernelBuffersCount(count);
}

void M2kDigitalImpl::setKernelBuffersCountOut(unsigned int count)
{
	m_dev_write->setKernelBuffersCount(count);
}

void M2kDigitalImpl::setDirection(unsigned short mask)
{
	DIO_DIRECTION direction;
	bool dir = false;
	unsigned int index = 0;
	while (mask != 0 || index < m_dev_write->getNbChannels(true)) {
		dir = mask & 1;
		mask >>= 1;
		direction = static_cast<DIO_DIRECTION>(dir);
		setDirection(index, direction);
		index++;
	}
}

void M2kDigitalImpl::setDirection(DIO_CHANNEL index, bool dir)
{
	DIO_DIRECTION direction = static_cast<DIO_DIRECTION>(dir);
	setDirection(index, direction);
}

void M2kDigitalImpl::setDirection(unsigned int index, bool dir)
{
	DIO_CHANNEL chn = static_cast<DIO_CHANNEL>(index);
	DIO_DIRECTION direction = static_cast<DIO_DIRECTION>(dir);
	setDirection(chn, direction);
}

void M2kDigitalImpl::setDirection(unsigned int index, DIO_DIRECTION dir)
{
	DIO_CHANNEL chn = static_cast<DIO_CHANNEL>(index);
	setDirection(chn, dir);
}

void M2kDigitalImpl::setDirection(DIO_CHANNEL index, DIO_DIRECTION dir)
{
	if (index < m_dev_generic->getNbChannels(false)) {
		std::string dir_str = "";
		if (dir == 0) {
			dir_str = "in";
		} else {
			dir_str = "out";
		}
		m_dev_generic->setStringValue(index, "direction", dir_str);
	} else {
		THROW_M2K_EXCEPTION("M2kDigital: No such digital channel.", libm2k::EXC_OUT_OF_RANGE);
	}

}

DIO_DIRECTION M2kDigitalImpl::getDirection(DIO_CHANNEL index)
{
	if (index >= m_dev_generic->getNbChannels(false)) {
		THROW_M2K_EXCEPTION("M2kDigital: No such digital channel", libm2k::EXC_OUT_OF_RANGE);
	}

	std::string dir_str = m_dev_generic->getStringValue(index, "direction");
	if (dir_str == "in") {
		return DIO_INPUT;
	}
	return DIO_OUTPUT;
}

void M2kDigitalImpl::setValueRaw(DIO_CHANNEL index, DIO_LEVEL level)
{
	if (index >= m_dev_generic->getNbChannels(false)) {
		THROW_M2K_EXCEPTION("M2kDigital: No such digital channel", libm2k::EXC_OUT_OF_RANGE);
	}
	long long val = static_cast<long long>(level);
	m_dev_generic->setDoubleValue(index, val, "raw");
}

void M2kDigitalImpl::setValueRaw(unsigned int index, DIO_LEVEL level)
{
	DIO_CHANNEL idx = static_cast<DIO_CHANNEL>(index);
	setValueRaw(idx, level);
}

void M2kDigitalImpl::setValueRaw(DIO_CHANNEL index, bool level)
{
	DIO_LEVEL lvl = static_cast<DIO_LEVEL>(level);
	setValueRaw(index, lvl);
}

DIO_LEVEL M2kDigitalImpl::getValueRaw(DIO_CHANNEL index)
{
	if (index >= m_dev_generic->getNbChannels(false)) {
		THROW_M2K_EXCEPTION("M2kDigital: No such digital channel", libm2k::EXC_OUT_OF_RANGE);
	}
	long long val;
	val = m_dev_generic->getDoubleValue(index, "raw");
	return static_cast<DIO_LEVEL>(val);
}

DIO_LEVEL M2kDigitalImpl::getValueRaw(unsigned int index)
{
	DIO_CHANNEL idx = static_cast<DIO_CHANNEL>(index);
	return getValueRaw(idx);
}

void M2kDigitalImpl::push(std::vector<unsigned short> const &data)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kDigital push");
	if (!anyChannelEnabled(DIO_OUTPUT)) {
		THROW_M2K_EXCEPTION("M2kDigital: No TX channel enabled.", libm2k::EXC_INVALID_PARAMETER);
	}
	m_dev_write->push(data, 0, getCyclic(), true);
	LIBM2K_LOG(INFO, "[END] M2kDigital push");
}

void M2kDigitalImpl::push(unsigned short *data, unsigned int nb_samples)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kDigital push");
	if (!anyChannelEnabled(DIO_OUTPUT)) {
		THROW_M2K_EXCEPTION("M2kDigital: No TX channel enabled.", libm2k::EXC_INVALID_PARAMETER);
	}
	m_dev_write->push(data, 0, nb_samples, getCyclic(), true);
	LIBM2K_LOG(INFO, "[END] M2kDigital push");
}

void M2kDigitalImpl::stopBufferOut()
{
	m_dev_write->stop();
}

void M2kDigitalImpl::startAcquisition(unsigned int nb_samples)
{
	m_dev_read->initializeBuffer(nb_samples);
}

void M2kDigitalImpl::stopAcquisition()
{
	m_dev_read->flushBuffer();
}

std::vector<unsigned short> M2kDigitalImpl::getSamples(unsigned int nb_samples)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kDigital getSamples");
	__try {
		if (!anyChannelEnabled(DIO_INPUT)) {
			THROW_M2K_EXCEPTION("M2kDigital: No RX channel enabled.", libm2k::EXC_INVALID_PARAMETER);
		}

		/* There is a restriction in the HDL that the buffer size must
			 * be a multiple of 8 bytes (4x 16-bit samples). Round up to the
			 * nearest multiple.*/
		nb_samples = ((nb_samples + 3) / 4) * 4;
		LIBM2K_LOG(INFO, "[END] M2kDigital getSamples");
		return m_dev_read->getSamplesShort(nb_samples);

	} __catch (exception_type &e) {
		THROW_M2K_EXCEPTION("M2K Digital: " + string(e.what()), libm2k::EXC_INVALID_PARAMETER);
		return std::vector<unsigned short>();
	}
}

const unsigned short* M2kDigitalImpl::getSamplesP(unsigned int nb_samples)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kDigital getSamplesP");
	__try {
		if (!anyChannelEnabled(DIO_INPUT)) {
			THROW_M2K_EXCEPTION("M2kDigital: No RX channel enabled.", libm2k::EXC_INVALID_PARAMETER);
			return nullptr;
		}

		/* There is a restriction in the HDL that the buffer size must
			 * be a multiple of 8 bytes (4x 16-bit samples). Round up to the
			 * nearest multiple.*/
		nb_samples = ((nb_samples + 3) / 4) * 4;
		LIBM2K_LOG(INFO, "[END] M2kDigital getSamplesP");
		return m_dev_read->getSamplesP(nb_samples);

	} __catch (exception_type &e) {
		THROW_M2K_EXCEPTION("M2K Digital: " + string(e.what()), libm2k::EXC_INVALID_PARAMETER);
		return nullptr;
	}
}

void M2kDigitalImpl::enableChannel(unsigned int index, bool enable)
{
	if (index < m_dev_write->getNbChannels(true)) {
		m_dev_write->enableChannel(index, enable, true);
		m_tx_channels_enabled.at(index) = enable;
	} else {
		THROW_M2K_EXCEPTION("M2kDigital: Cannot enable digital TX channel", libm2k::EXC_INVALID_PARAMETER);
	}
}

void M2kDigitalImpl::enableChannel(DIO_CHANNEL index, bool enable)
{
	if (index < m_dev_write->getNbChannels(true)) {
		m_dev_write->enableChannel(index, enable, true);
		m_tx_channels_enabled.at(index) = enable;
	} else {
		THROW_M2K_EXCEPTION("M2kDigital: Cannot enable digital TX channel", libm2k::EXC_INVALID_PARAMETER);
	}
}

void M2kDigitalImpl::enableAllOut(bool enable)
{
	for (unsigned int i = 0; i < m_dev_write->getNbChannels(true) - 2; i++) {
		DIO_CHANNEL idx = static_cast<DIO_CHANNEL>(i);
		enableChannel(idx, enable);
	}
}

bool M2kDigitalImpl::anyChannelEnabled(DIO_DIRECTION dir)
{
	if (dir == DIO_INPUT) {
		for (auto b : m_rx_channels_enabled) {
			if (b) {
				return true;
			}
		}
	} else {
		for (auto b : m_tx_channels_enabled) {
			if (b) {
				return true;
			}
		}
	}
	return false;
}

M2kHardwareTrigger* M2kDigitalImpl::getTrigger()
{
	return m_trigger;
}

void M2kDigitalImpl::setOutputMode(DIO_CHANNEL chn, DIO_MODE mode)
{
	std::string output_mode = m_output_mode[mode];
	m_dev_generic->setStringValue(chn, "outputmode", output_mode);
}

void M2kDigitalImpl::setOutputMode(unsigned int chn, DIO_MODE mode)
{
	DIO_CHANNEL idx = static_cast<DIO_CHANNEL>(chn);
	setOutputMode(idx, mode);
}

DIO_MODE M2kDigitalImpl::getOutputMode(DIO_CHANNEL chn)
{
	std::string output_mode = "";
	output_mode = m_dev_generic->getStringValue(chn, "outputmode");

	auto it = std::find(m_output_mode.begin(), m_output_mode.end(),
			    output_mode.c_str());
	if (it == m_output_mode.end()) {
		THROW_M2K_EXCEPTION("M2kDigital: Cannot read channel attribute: output mode", libm2k::EXC_OUT_OF_RANGE);
	}

	return static_cast<DIO_MODE>(it - m_output_mode.begin());
}

DIO_MODE M2kDigitalImpl::getOutputMode(unsigned int chn)
{
	DIO_CHANNEL idx = static_cast<DIO_CHANNEL>(chn);
	return getOutputMode(idx);
}

double M2kDigitalImpl::setSampleRateIn(double samplerate)
{
	return m_dev_read->setDoubleValue(samplerate, "sampling_frequency");
}

double M2kDigitalImpl::setSampleRateOut(double samplerate)
{
	return m_dev_write->setDoubleValue(samplerate, "sampling_frequency");
}

double M2kDigitalImpl::getSampleRateIn()
{
	return m_dev_read->getDoubleValue("sampling_frequency");
}

double M2kDigitalImpl::getSampleRateOut()
{
	return m_dev_write->getDoubleValue("sampling_frequency");
}

bool M2kDigitalImpl::getCyclic()
{
	return m_cyclic;
}

void M2kDigitalImpl::setCyclic(bool cyclic)
{
	m_cyclic = cyclic;
	m_dev_write->setCyclic(cyclic);
}

struct IIO_OBJECTS M2kDigitalImpl::getIioObjects()
{
	auto tx_iio = m_dev_write->getIioObjects();
	auto rx_iio = m_dev_read->getIioObjects();
	rx_iio.buffers_tx.insert(rx_iio.buffers_tx.end(), tx_iio.buffers_tx.begin(), tx_iio.buffers_tx.end());
	rx_iio.channels_out.insert(rx_iio.channels_out.end(), tx_iio.channels_out.begin(), tx_iio.channels_out.end());
	rx_iio.devices.insert(rx_iio.devices.end(), tx_iio.devices.begin(), tx_iio.devices.end());
	return rx_iio;
}


void M2kDigitalImpl::cancelAcquisition()
{
	m_dev_read->cancelBuffer();
}

void M2kDigitalImpl::cancelBufferOut()
{
	m_dev_write->cancelBuffer();
}

unsigned int M2kDigitalImpl::getNbChannelsIn()
{
	return m_dev_read->getNbChannels(false);
}

unsigned int M2kDigitalImpl::getNbChannelsOut()
{
	return m_dev_write->getNbChannels(true);
}

void M2kDigitalImpl::getSamples(std::vector<unsigned short> &data, unsigned int nb_samples)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kDigital getSamples");
	if (!anyChannelEnabled(DIO_INPUT)) {
		THROW_M2K_EXCEPTION("M2kDigital: No RX channel enabled.", libm2k::EXC_INVALID_PARAMETER);
	}

	/* There is a restriction in the HDL that the buffer size must
	 * be a multiple of 8 bytes (4x 16-bit samples). Round up to the
	 * nearest multiple.*/
	nb_samples = ((nb_samples + 3) / 4) * 4;
	m_dev_read->getSamples(data, nb_samples);
	LIBM2K_LOG(INFO, "[END] M2kDigital getSamples");
}

bool M2kDigitalImpl::hasRateMux()
{
	return m_dev_read->hasGlobalAttribute("rate_mux");
}

void M2kDigitalImpl::setRateMux()
{
	m_dev_read->setStringValue("rate_mux", "oscilloscope");
}

void M2kDigitalImpl::resetRateMux()
{
	m_dev_read->setStringValue("rate_mux", "logic_analyzer");
}
