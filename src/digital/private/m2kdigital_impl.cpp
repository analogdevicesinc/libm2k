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

#include "../../utils/private/device_impl.cpp"
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

class M2kDigital::M2kDigitalImpl : public DeviceImpl {
public:

	std::vector<std::string> m_output_mode = {
		"open-drain",
		"push-pull",
	};

	std::vector<std::string> m_trigger_logic_mode = {
		"or",
		"and",
	};

	M2kDigitalImpl(struct iio_context *ctx, std::string logic_dev) :
		DeviceImpl(ctx, logic_dev)
	{
		m_dev_name_write = logic_dev + "-tx";
		m_dev_name_read = logic_dev + "-rx";

		if (m_dev_name_write != "") {
			__try {
				m_dev_write = make_shared<Device>(ctx, m_dev_name_write);
			} __catch (exception_type &e) {
				m_dev_write = nullptr;
				throw_exception(EXC_INVALID_PARAMETER, "M2K Digital: No device was found for writing");
			}
		}

		if (m_dev_name_read != "") {
			__try {
				m_dev_read = make_shared<Device>(ctx, m_dev_name_read);
			} __catch (exception_type &e) {
				m_dev_read = nullptr;
				throw_exception(EXC_INVALID_PARAMETER, "M2K Digital: No device was found for reading");
			}
		}

		if (!m_dev_read || !m_dev_write) {
			m_dev_read = nullptr;
			m_dev_write = nullptr;
			throw_exception(EXC_INVALID_PARAMETER, "M2K Digital: logic device not found");
		}

		m_dev_read->setKernelBuffersCount(25);
	}

	~M2kDigitalImpl()
	{
	}

	void setDirection(unsigned short mask)
	{
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
	}

	void setDirection(DIO_CHANNEL index, bool dir)
	{
		DIO_DIRECTION direction = static_cast<DIO_DIRECTION>(dir);
		setDirection(index, direction);
	}

	void setDirection(unsigned int index, bool dir)
	{
		DIO_CHANNEL chn = static_cast<DIO_CHANNEL>(index);
		DIO_DIRECTION direction = static_cast<DIO_DIRECTION>(dir);
		setDirection(chn, direction);
	}

	void setDirection(unsigned int index, DIO_DIRECTION dir)
	{
		DIO_CHANNEL chn = static_cast<DIO_CHANNEL>(index);
		setDirection(chn, dir);
	}

	void setDirection(DIO_CHANNEL index, DIO_DIRECTION dir)
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
			throw_exception(EXC_OUT_OF_RANGE, "M2kDigital: No such digital channel.");
		}

	}

	DIO_DIRECTION getDirection(DIO_CHANNEL index)
	{
		if (index >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "M2kDigital: No such digital channel");
		}

		std::string dir_str = getStringValue(index, "direction");
		if (dir_str == "in") {
			return DIO_INPUT;
		}
		return DIO_OUTPUT;
	}

	void setValueRaw(DIO_CHANNEL index, DIO_LEVEL level)
	{
		if (index >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "M2kDigital: No such digital channel");
		}
		long long val = static_cast<long long>(level);
		setDoubleValue(index, val, "raw");
	}

	DIO_LEVEL getValueRaw(DIO_CHANNEL index)
	{
		if (index >= getNbChannels()) {
			throw_exception(EXC_OUT_OF_RANGE, "M2kDigital: No such digital channel");
		}
		long long val;
		val = getDoubleValue(index, "raw");
		return static_cast<DIO_LEVEL>(val);
	}

	void push(std::vector<short> &data)
	{
		if (!anyChannelEnabled(DIO_OUTPUT)) {
			throw_exception(EXC_INVALID_PARAMETER, "M2kDigital: No TX channel enabled.");
		}

		m_dev_write->push(data, 0, getCyclic(), true);
	}

	void stop()
	{
		m_dev_write->stop();
	}

	std::vector<unsigned short> getSamples(int nb_samples)
	{
		__try {
			if (!anyChannelEnabled(DIO_INPUT)) {
				throw_exception(EXC_INVALID_PARAMETER, "M2kDigital: No RX channel enabled.");

			}

			/* There is a restriction in the HDL that the buffer size must
			 * be a multiple of 8 bytes (4x 16-bit samples). Round up to the
			 * nearest multiple.*/
			nb_samples = ((nb_samples + 3) / 4) * 4;
			return m_dev_read->getSamples(nb_samples);

		} __catch (exception_type &e) {
			throw_exception(EXC_INVALID_PARAMETER, "M2K Digital: " + string(e.what()));
		}
	}

	void enableChannelIn(DIO_CHANNEL index, bool enable)
	{
		if (index < getNbChannels()) {
			m_dev_read->enableChannel(index, enable);
		} else {
			throw_exception(EXC_OUT_OF_RANGE, "M2kDigital: Cannot enable digital channel");
		}
	}

	void enableAllIn(bool enable)
	{
		for (unsigned int i = 0; i < m_dev_read->getNbChannels(); i++) {
			DIO_CHANNEL idx = static_cast<DIO_CHANNEL>(i);
			enableChannelIn(idx, enable);
		}
	}

	void enableChannelOut(DIO_CHANNEL index, bool enable)
	{
		if (index < getNbChannels()) {
			m_dev_write->enableChannel(index, enable);
		} else {
			throw_exception(EXC_OUT_OF_RANGE, "M2kDigital: Cannot enable digital channel");
		}
	}

	void enableAllOut(bool enable)
	{
		for (unsigned int i = 0; i < m_dev_write->getNbChannels(); i++) {
			DIO_CHANNEL idx = static_cast<DIO_CHANNEL>(i);
			enableChannelOut(idx, enable);
		}
	}

	bool anyChannelEnabled(DIO_DIRECTION dir)
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

	void setTrigger(DIO_CHANNEL chn, M2K_TRIGGER_CONDITION cond)
	{
		std::string trigger_val = M2kHardwareTrigger::getAvailableDigitalConditions()[cond];
		m_dev_read->setStringValue(chn, "trigger", trigger_val, false);
	}

	M2K_TRIGGER_CONDITION getTrigger(DIO_CHANNEL chn)
	{
		std::string trigger_val = m_dev_read->getStringValue(chn, "trigger", false);
		std::vector<std::string> available_digital_conditions =
				M2kHardwareTrigger::getAvailableDigitalConditions();

		auto it = std::find(available_digital_conditions.begin(),
				    available_digital_conditions.end(), trigger_val.c_str());
		if (it == available_digital_conditions.end()) {
			throw_exception(EXC_INVALID_PARAMETER, "M2kDigital: Cannot read channel attribute: trigger");
		}

		return static_cast<M2K_TRIGGER_CONDITION>
				(it - available_digital_conditions.begin());
	}

	void setTriggerDelay(int delay)
	{
		__try {
			m_dev_read->setDoubleValue(DIO_CHANNEL_0, delay, "trigger_delay", false);
		} __catch (exception_type &e) {
			throw_exception(EXC_INVALID_PARAMETER, e.what());
		}
	}

	int getTriggerDelay()
	{
		return (int)m_dev_read->getDoubleValue(DIO_CHANNEL_0, "trigger_delay", false);
	}

	void setTriggerMode(DIO_TRIGGER_MODE trig_mode)
	{
		std::string trigger_mode = m_trigger_logic_mode[trig_mode];
		m_dev_read->setStringValue(DIO_CHANNEL_0, "trigger_logic_mode", trigger_mode, false);
	}

	DIO_TRIGGER_MODE getTriggerMode()
	{
		std::string trigger_mode = "";
		trigger_mode = m_dev_read->getStringValue(DIO_CHANNEL_0,
						"trigger_logic_mode", false);

		auto it = std::find(m_trigger_logic_mode.begin(), m_trigger_logic_mode.end(),
				    trigger_mode.c_str());
		if (it == m_trigger_logic_mode.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "Cannot read channel attribute: trigger logic mode");
		}
		return static_cast<DIO_TRIGGER_MODE>(it - m_trigger_logic_mode.begin());
	}

	void setOutputMode(DIO_CHANNEL chn, DIO_MODE mode)
	{
		std::string output_mode = m_output_mode[mode];
		setStringValue(chn, "outputmode", output_mode);
	}

	DIO_MODE getOutputMode(DIO_CHANNEL chn)
	{
		std::string output_mode = "";
			output_mode = getStringValue(chn, "outputmode");

		auto it = std::find(m_output_mode.begin(), m_output_mode.end(),
				    output_mode.c_str());
		if (it == m_output_mode.end()) {
			throw_exception(EXC_OUT_OF_RANGE, "M2kDigital: Cannot read channel attribute: trigger");
		}

		return static_cast<DIO_MODE>(it - m_output_mode.begin());
	}

	bool getCyclic()
	{
		return m_cyclic;
	}

	void setCyclic(bool cyclic)
	{
		m_cyclic = cyclic;
	}


private:
	bool m_cyclic;
	std::shared_ptr<Device> m_dev_read;
	std::shared_ptr<Device> m_dev_write;
	std::string m_dev_name_read;
	std::string m_dev_name_write;
};
