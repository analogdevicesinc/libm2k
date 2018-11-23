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

#include "libm2k/m2kdigital.hpp"
#include "libm2k/m2kexceptions.hpp"
#include "utils.hpp"
#include <iio.h>
#include <iostream>
#include <algorithm>

using namespace libm2k::utils;
using namespace libm2k::digital;
using namespace libm2k::analog;
using namespace std;

M2kDigital::M2kDigital(struct iio_context *ctx, std::string logic_dev) :
	GenericDigital(ctx, logic_dev)
{
	m_dev_name_write = logic_dev + "-tx";
	m_dev_name_read = logic_dev + "-rx";

	m_dev_read = iio_context_find_device(ctx, m_dev_name_read.c_str());
	m_dev_write = iio_context_find_device(ctx, m_dev_name_write.c_str());
	if (!m_dev_read || !m_dev_write) {
		m_dev_read = nullptr;
		m_dev_write = nullptr;
		throw no_device_exception("M2K logic device not found");
	}
	m_buffer_in = nullptr;
	m_buffer_out = nullptr;

	for (unsigned int i = 0; i < m_nb_channels; i++) {
		m_channel_read_list.push_back(iio_device_get_channel(m_dev_read, i));
		m_channel_write_list.push_back(iio_device_get_channel(m_dev_write, i));
	}
	iio_device_set_kernel_buffers_count(m_dev_read, 25);
}

M2kDigital::~M2kDigital()
{
	if (m_buffer_out) {
		iio_buffer_cancel(m_buffer_out);
		iio_buffer_destroy(m_buffer_out);
		m_buffer_out = nullptr;
	}

	m_data.clear();

}

void M2kDigital::setDirection(DIO_CHANNEL index, GenericDigital::DIO_DIRECTION dir)
{
	if (index < m_nb_channels) {
		std::string dir_str = "";
		if (dir == 0) {
			dir_str = "in";
		} else {
			dir_str = "out";
		}
		iio_channel_attr_write(m_channel_list.at(index).m_channel,
				       "direction", dir_str.c_str());
	} else {
		throw invalid_parameter_exception("No such digital channel.");
	}

}

GenericDigital::DIO_DIRECTION M2kDigital::getDirection(DIO_CHANNEL index)
{
	if (index < m_nb_channels) {
		char dir_str[10];
		iio_channel_attr_read(m_channel_list.at(index).m_channel, "direction",
				      dir_str, sizeof(dir_str));
		if (std::string(dir_str) == "in") {
			return GenericDigital::DIO_INPUT;
		} else {
			return GenericDigital::DIO_OUTPUT;
		}
	} else {
		throw invalid_parameter_exception("No such digital channel");
	}
}

void M2kDigital::setValue(DIO_CHANNEL index, GenericDigital::level level)
{
	if (index < m_nb_channels) {
		long long val = static_cast<long long>(level);
		iio_channel_attr_write_longlong(m_channel_list.at(index).m_channel,
						"raw", val);
	} else {
		throw invalid_parameter_exception("No such digital channel");
	}
}

GenericDigital::level M2kDigital::getValue(DIO_CHANNEL index)
{
	if (index < m_nb_channels) {
		long long val;
		int ret = iio_channel_attr_read_longlong(m_channel_list.at(index).m_channel,
					       "raw", &val);
		if (ret < 0) {
			throw invalid_parameter_exception("Cannot read value for channel");
		}
		return static_cast<level>(val);
	} else {
		throw invalid_parameter_exception("No such digital channel");
	}
}

void M2kDigital::push(std::vector<unsigned short> &data, bool cyclic)
{
	size_t size = data.size();
	if (m_buffer_out) {
		iio_buffer_destroy(m_buffer_out);
	}

	try {
		for (unsigned int i = 0; i < 16; i++) {
			enableChannelOut(static_cast<DIO_CHANNEL>(i), true);
			setDirection(static_cast<DIO_CHANNEL>(i), DIO_OUTPUT);
		}
		if (!anyChannelEnabled(DIO_OUTPUT)) {
			throw invalid_parameter_exception("No TX channel enabled.");
		}

		m_buffer_out = iio_device_create_buffer(m_dev_write, size, cyclic);

		short *p_dat;
		int i = 0;

		for (p_dat = (short *)iio_buffer_start(m_buffer_out); (p_dat < iio_buffer_end(m_buffer_out));
		     (unsigned short*)p_dat++, i++) {
			*p_dat = data[i];
		}

		iio_buffer_push(m_buffer_out);
	} catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

std::vector<unsigned short> M2kDigital::getSamples(int nb_samples)
{
	m_data.clear();
	for (int i = 0; i < nb_samples; i++) {
		m_data.push_back(0);
	}

	try {
		if (!anyChannelEnabled(DIO_INPUT)) {
			throw invalid_parameter_exception("No RX channel enabled.");
		}

		/* There is a restriction in the HDL that the buffer size must
		 * be a multiple of 8 bytes (4x 16-bit samples). Round up to the
		 * nearest multiple.*/
		nb_samples = ((nb_samples + 3) / 4) * 4;
		m_buffer_in = iio_device_create_buffer(m_dev_read, nb_samples, false);

		if (!m_buffer_in) {
			m_buffer_in = nullptr;
			throw invalid_parameter_exception("Could not create logic RX buffer");
		}

		ssize_t nbytes_rx = iio_buffer_refill(m_buffer_in);
		if (nbytes_rx < 0) {
			throw instrument_already_in_use_exception(
				"Cannot refill buffer for " + m_dev_name_read);
		}

		unsigned short* data = (unsigned short*)iio_buffer_start(m_buffer_in);
		for (int i = 0; i < nb_samples; i++) {
			m_data[i] = data[i];
		}

		iio_buffer_cancel(m_buffer_in);
		iio_buffer_destroy(m_buffer_in);
		return m_data;

	} catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

void M2kDigital::enableChannelIn(DIO_CHANNEL index, bool enable)
{
	if (index < m_nb_channels) {
		if (enable) {
			iio_channel_enable(m_channel_read_list.at(index));
		} else {
			iio_channel_disable(m_channel_read_list.at(index));
		}
	} else {
		throw invalid_parameter_exception("Cannot enable digital channel");
	}
}

void M2kDigital::enableChannelOut(DIO_CHANNEL index, bool enable)
{
	if (index < m_nb_channels) {
		if (enable) {
			iio_channel_enable(m_channel_write_list.at(index));
		} else {
			iio_channel_disable(m_channel_write_list.at(index));
		}
	} else {
		throw invalid_parameter_exception("Cannot enable digital channel");
	}
}

bool M2kDigital::anyChannelEnabled(DIO_DIRECTION dir)
{

	if (dir == DIO_INPUT) {
		for (struct iio_channel* chn : m_channel_read_list) {
			if (iio_channel_is_enabled(chn)) {
				return true;
			}
		}
	} else {
		for (struct iio_channel* chn : m_channel_write_list) {
			if (iio_channel_is_enabled(chn)) {
				return true;
			}
		}
	}
	return false;
}

void M2kDigital::setTrigger(M2kDigital::DIO_CHANNEL chn,
			    libm2k::analog::M2kHardwareTrigger::condition cond)
{
	std::string trigger_val = M2kHardwareTrigger::getAvailableDigitalConditions()[cond];
	iio_channel_attr_write(m_channel_read_list.at(chn), "trigger", trigger_val.c_str());
}

M2kHardwareTrigger::condition M2kDigital::getTrigger(M2kDigital::DIO_CHANNEL chn)
{
	char trigger_val[1024];
	iio_channel_attr_read(m_channel_read_list.at(chn), "trigger", trigger_val,
			      sizeof(trigger_val));
	std::vector<std::string> available_digital_conditions =
			M2kHardwareTrigger::getAvailableDigitalConditions();

	auto it = std::find(available_digital_conditions.begin(),
			    available_digital_conditions.end(), trigger_val);
	if (it == available_digital_conditions.end()) {
		throw invalid_argument("Cannot read channel attribute: trigger");
	}

	return static_cast<M2kHardwareTrigger::condition>
		(it - available_digital_conditions.begin());
}

