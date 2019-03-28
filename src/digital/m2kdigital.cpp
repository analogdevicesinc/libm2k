/*
 * Copyright 2018 Analog Devices, Inc.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file LICENSE.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <libm2k/digital/m2kdigital.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>
#include <iio.h>
#include <iostream>
#include <algorithm>

using namespace libm2k::utils;
using namespace libm2k::digital;
using namespace libm2k::analog;
using namespace std;

std::vector<std::string> M2kDigital::m_output_mode = {
	"open-drain",
	"push-pull",
};

std::vector<std::string> M2kDigital::m_trigger_logic_mode = {
	"or",
	"and",
};

M2kDigital::M2kDigital(struct iio_context *ctx, std::string logic_dev) :
	Device(ctx, logic_dev)
{
	m_dev_name_write = logic_dev + "-tx";
	m_dev_name_read = logic_dev + "-rx";

	if (m_dev_name_write != "") {
		try {
			m_dev_write = make_shared<Device>(ctx, m_dev_name_write);
		} catch (std::runtime_error &e) {
			m_dev_write = nullptr;
			throw invalid_parameter_exception("M2K Digital: No device was found for writing");
		}
	}

	if (m_dev_name_read != "") {
		try {
			m_dev_read = make_shared<Device>(ctx, m_dev_name_read);
		} catch (std::runtime_error &e) {
			m_dev_read = nullptr;
			throw invalid_parameter_exception("M2K Digital: No device was found for reading");
		}
	}

	if (!m_dev_read || !m_dev_write) {
		m_dev_read = nullptr;
		m_dev_write = nullptr;
		throw no_device_exception("M2K Digital: logic device not found");
	}

	m_dev_read->setKernelBuffersCount(25);
}

M2kDigital::~M2kDigital()
{
}

void M2kDigital::setDirection(unsigned short mask)
{
	try {
		DIO_DIRECTION direction;
		bool dir = false;
		unsigned int index = 0;
		while (mask != 0 || index < m_dev_write->getNbChannels()) {
			dir = mask & 1;
			mask >>= 1;
			direction = static_cast<DIO_DIRECTION>(dir);
			setDirection(index, direction);
			index++;
		}
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kDigital::setDirection(DIO_CHANNEL index, bool dir)
{
	try {
		DIO_DIRECTION direction = static_cast<DIO_DIRECTION>(dir);
		setDirection(index, direction);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kDigital::setDirection(unsigned int index, bool dir)
{
	try {
		DIO_CHANNEL chn = static_cast<DIO_CHANNEL>(index);
		DIO_DIRECTION direction = static_cast<DIO_DIRECTION>(dir);
		setDirection(chn, direction);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kDigital::setDirection(unsigned int index, DIO_DIRECTION dir)
{
	try {
		DIO_CHANNEL chn = static_cast<DIO_CHANNEL>(index);
		setDirection(chn, dir);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kDigital::setDirection(DIO_CHANNEL index, DIO_DIRECTION dir)
{
	if (index < getNbChannels()) {
		std::string dir_str = "";
		if (dir == 0) {
			dir_str = "in";
		} else {
			dir_str = "out";
		}
		setStringValue(index, "direction", dir_str);
	} else {
		throw invalid_parameter_exception("No such digital channel.");
	}

}

DIO_DIRECTION M2kDigital::getDirection(DIO_CHANNEL index)
{
	if (index < getNbChannels()) {
		std::string dir_str = getStringValue(index, "direction");
		if (dir_str == "in") {
			return DIO_INPUT;
		} else {
			return DIO_OUTPUT;
		}
	} else {
		throw invalid_parameter_exception("No such digital channel");
	}
}

void M2kDigital::setValueRaw(DIO_CHANNEL index, DIO_LEVEL level)
{
	if (index < getNbChannels()) {
		long long val = static_cast<long long>(level);
		setDoubleValue(index, val, "raw");
	} else {
		throw invalid_parameter_exception("No such digital channel");
	}
}

DIO_LEVEL M2kDigital::getValueRaw(DIO_CHANNEL index)
{
	if (index < getNbChannels()) {
		try {
			long long val;
			val = getDoubleValue(index, "raw");
			return static_cast<DIO_LEVEL>(val);
		} catch (std::runtime_error &e) {
			throw invalid_parameter_exception("Cannot read value for channel");
		}
	} else {
		throw invalid_parameter_exception("No such digital channel");
	}
}

void M2kDigital::push(std::vector<short> &data)
{
	try {
		if (!anyChannelEnabled(DIO_OUTPUT)) {
			throw invalid_parameter_exception("No TX channel enabled.");
		}

		m_dev_write->push(data, 0, getCyclic(), true);

	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kDigital::stop()
{
	m_dev_write->stop();
}

std::vector<unsigned short> M2kDigital::getSamples(int nb_samples)
{

	try {
		if (!anyChannelEnabled(DIO_INPUT)) {
			throw invalid_parameter_exception("No RX channel enabled.");
		}

		/* There is a restriction in the HDL that the buffer size must
		 * be a multiple of 8 bytes (4x 16-bit samples). Round up to the
		 * nearest multiple.*/
		nb_samples = ((nb_samples + 3) / 4) * 4;
		return m_dev_read->getSamples(nb_samples);

	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception("M2K Digital: " + string(e.what()));
	}
}

void M2kDigital::enableChannelIn(DIO_CHANNEL index, bool enable)
{
	if (index < getNbChannels()) {
		m_dev_read->enableChannel(index, enable);
	} else {
		throw invalid_parameter_exception("Cannot enable digital channel");
	}
}

void M2kDigital::enableAllIn(bool enable)
{
	try {
		for (unsigned int i = 0; i < m_dev_read->getNbChannels(); i++) {
			DIO_CHANNEL idx = static_cast<DIO_CHANNEL>(i);
			enableChannelIn(idx, enable);
		}
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kDigital::enableChannelOut(DIO_CHANNEL index, bool enable)
{
	if (index < getNbChannels()) {
		m_dev_write->enableChannel(index, enable);
	} else {
		throw invalid_parameter_exception("Cannot enable digital channel");
	}
}

void M2kDigital::enableAllOut(bool enable)
{
	try {
		for (unsigned int i = 0; i < m_dev_write->getNbChannels(); i++) {
			DIO_CHANNEL idx = static_cast<DIO_CHANNEL>(i);
			enableChannelOut(idx, enable);
		}
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

bool M2kDigital::anyChannelEnabled(DIO_DIRECTION dir)
{

	if (dir == DIO_INPUT) {
		for (unsigned int i = 0; i < m_dev_read->getNbChannels(); i++) {
			if (m_dev_read->isChannelEnabled(i)) {
				return true;
			}
		}
	} else {
		for (unsigned int i = 0; i < m_dev_write->getNbChannels(); i++) {
			if (m_dev_write->isChannelEnabled(i)) {
				return true;
			}
		}
	}
	return false;
}

void M2kDigital::setTrigger(DIO_CHANNEL chn, M2K_TRIGGER_CONDITION cond)
{
	std::string trigger_val = M2kHardwareTrigger::getAvailableDigitalConditions()[cond];
	m_dev_read->setStringValue(chn, "trigger", trigger_val, false);
}

M2K_TRIGGER_CONDITION M2kDigital::getTrigger(DIO_CHANNEL chn)
{
	std::string trigger_val = m_dev_read->getStringValue(chn, "trigger", false);
	std::vector<std::string> available_digital_conditions =
			M2kHardwareTrigger::getAvailableDigitalConditions();

	auto it = std::find(available_digital_conditions.begin(),
			    available_digital_conditions.end(), trigger_val.c_str());
	if (it == available_digital_conditions.end()) {
		throw invalid_argument("Cannot read channel attribute: trigger");
	}

	return static_cast<M2K_TRIGGER_CONDITION>
			(it - available_digital_conditions.begin());
}

void M2kDigital::setTriggerDelay(int delay)
{
	try {
		m_dev_read->setDoubleValue(DIO_CHANNEL_0, delay, "trigger_delay", false);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

int M2kDigital::getTriggerDelay()
{
	try {
		return (int)m_dev_read->getDoubleValue(DIO_CHANNEL_0, "trigger_delay", false);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kDigital::setTriggerMode(DIO_TRIGGER_MODE trig_mode)
{
	try {
		std::string trigger_mode = m_trigger_logic_mode[trig_mode];
		m_dev_read->setStringValue(DIO_CHANNEL_0, "trigger_logic_mode", trigger_mode, false);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

DIO_TRIGGER_MODE M2kDigital::getTriggerMode()
{
	std::string trigger_mode = "";
	try {
		trigger_mode = m_dev_read->getStringValue(DIO_CHANNEL_0,
					      "trigger_logic_mode", false);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}

	auto it = std::find(m_trigger_logic_mode.begin(), m_trigger_logic_mode.end(),
			    trigger_mode.c_str());
	if (it == m_trigger_logic_mode.end()) {
		throw invalid_argument("Cannot read channel attribute: trigger logic mode");
	}
	return static_cast<DIO_TRIGGER_MODE>(it - m_trigger_logic_mode.begin());
}

void M2kDigital::setOutputMode(DIO_CHANNEL chn, DIO_MODE mode)
{
	try {
		std::string output_mode = m_output_mode[mode];
		setStringValue(chn, "outputmode", output_mode);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

DIO_MODE M2kDigital::getOutputMode(DIO_CHANNEL chn)
{
	std::string output_mode = "";
	try {
		output_mode = getStringValue(chn, "outputmode");
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}

	auto it = std::find(m_output_mode.begin(), m_output_mode.end(),
			    output_mode.c_str());
	if (it == m_output_mode.end()) {
		throw invalid_argument("Cannot read channel attribute: trigger");
	}

	return static_cast<DIO_MODE>(it - m_output_mode.begin());
}

bool M2kDigital::getCyclic()
{
	return m_cyclic;
}

void M2kDigital::setCyclic(bool cyclic)
{
	m_cyclic = cyclic;
}

