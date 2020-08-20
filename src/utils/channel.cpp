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

#include "channel.hpp"
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>
#include <sstream>
#include <iterator>

using namespace libm2k;
using namespace libm2k::utils;

Channel::Channel(iio_device *device, unsigned int channel) {
	m_device = device;
	if (m_device) {
		m_channel = iio_device_get_channel(m_device, channel);
	}

	if (!m_channel) {
		m_channel = nullptr;
	}
}

Channel::Channel(iio_device *device, std::string channel_name, bool output)
{
	m_device = device;
	if (m_device) {
		m_channel = iio_device_find_channel(device, channel_name.c_str(), output);
	}

	if (!m_channel) {
		m_channel = nullptr;

	}

}

Channel::~Channel() {
}

std::string Channel::getName()
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	std::string name = "";
	auto n = iio_channel_get_name(m_channel);
	if (n) {
		name = std::string(n);
	}
	return name;
}

std::string Channel::getId()
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	return iio_channel_get_id(m_channel);
}

unsigned int Channel::getIndex()
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	long ret;
	ret = iio_channel_get_index(m_channel);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot get the index of channel", libm2k::EXC_INVALID_PARAMETER, ret);
	}
	return ret;
}

bool Channel::isOutput()
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	return iio_channel_is_output(m_channel);
}

bool Channel::isEnabled()
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	return iio_channel_is_enabled(m_channel);
}

bool Channel::hasAttribute(std::string attr)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	if (iio_channel_find_attr(m_channel, attr.c_str()) != NULL) {
		return true;
	}
	return false;
}

void Channel::enableChannel(bool enable)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}

	if (enable) {
		iio_channel_enable(m_channel);
	} else {
		iio_channel_disable(m_channel);
	}
}

uintptr_t Channel::getFirst(iio_buffer *buffer)
{
	return reinterpret_cast<uintptr_t>(getFirstVoid(buffer));
}


void* Channel::getFirstVoid(iio_buffer *buffer)
{
	return iio_buffer_first(buffer, m_channel);
}

void Channel::write(struct iio_buffer* buffer, std::vector<short> const &data)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}

	size_t size = data.size();
	size_t ret = iio_channel_write(m_channel, buffer, data.data(),
				       size * sizeof(short));

	if (ret == 0) {
		THROW_M2K_EXCEPTION("Channel: could not write; result is 0 bytes", libm2k::EXC_INVALID_PARAMETER);
	}
}

void Channel::write(struct iio_buffer* buffer, std::vector<unsigned short> const &data)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}

	size_t size = data.size();
	size_t ret = iio_channel_write(m_channel, buffer, data.data(),
				       size * sizeof(unsigned short));

	if (ret == 0) {
		THROW_M2K_EXCEPTION("Channel: could not write; result is 0 bytes", libm2k::EXC_INVALID_PARAMETER);
	}
}

void Channel::write(struct iio_buffer* buffer, std::vector<double> const &data)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}

	size_t size = data.size();
	size_t ret = iio_channel_write(m_channel, buffer, data.data(),
				       size * sizeof(double));

	if (ret == 0) {
		THROW_M2K_EXCEPTION("Channel: could not write; result is 0 bytes", libm2k::EXC_INVALID_PARAMETER);

	}
}

void Channel::write(struct iio_buffer* buffer, double *data, unsigned int nb_samples)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}

	size_t ret = iio_channel_write(m_channel, buffer, data,
				       nb_samples * sizeof(double));

	if (ret == 0) {
		THROW_M2K_EXCEPTION("Channel: could not write; result is 0 bytes", libm2k::EXC_INVALID_PARAMETER);
	}
}

void Channel::write(struct iio_buffer* buffer, short *data, unsigned int nb_samples)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}

	size_t ret = iio_channel_write(m_channel, buffer, data,
				       nb_samples * sizeof(short));

	if (ret == 0) {
		THROW_M2K_EXCEPTION("Channel: could not write; result is 0 bytes", libm2k::EXC_INVALID_PARAMETER);
	}
}

void Channel::write(struct iio_buffer* buffer, unsigned short *data, unsigned int nb_samples)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}

	size_t ret = iio_channel_write(m_channel, buffer, data,
				       nb_samples * sizeof(unsigned short));

	if (ret == 0) {
		THROW_M2K_EXCEPTION("Channel: could not write; result is 0 bytes", libm2k::EXC_INVALID_PARAMETER);
	}
}

void Channel::convert(int16_t *avg, int16_t *src)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	iio_channel_convert(m_channel, (void *)avg, (const void *)src);
}

void Channel::convert(double *avg, int16_t *src)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	iio_channel_convert(m_channel, (void *)avg, (const void *)src);
}

double Channel::getDoubleValue(std::string attr)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	double value = 0.0;
	int ret = iio_channel_attr_read_double(m_channel, attr.c_str(), &value);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot read " + attr, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	return value;
}

void Channel::setDoubleValue(std::string attr, double val)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	int ret = iio_channel_attr_write_double(m_channel, attr.c_str(), val);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot write " + attr, libm2k::EXC_INVALID_PARAMETER, ret);
	}
}

void Channel::setLongValue(std::string attr, long long val)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	int ret = iio_channel_attr_write_longlong(m_channel, attr.c_str(), val);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot write " + attr, libm2k::EXC_INVALID_PARAMETER, ret);
	}
}

long long Channel::getLongValue(std::string attr)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	long long value = 0;
	int ret = iio_channel_attr_read_longlong(m_channel, attr.c_str(), &value);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot write " + attr, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	return value;
}

void Channel::setStringValue(std::string attr, std::string val)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	int ret = iio_channel_attr_write(m_channel, attr.c_str(), val.c_str());
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot write " + attr, libm2k::EXC_INVALID_PARAMETER, ret);
	}
}

std::string Channel::getStringValue(std::string attr)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	char value[1024];
	int ret = iio_channel_attr_read(m_channel, attr.c_str(), value, sizeof(value));
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot write " + attr, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	return std::string(value);
}

void Channel::setBoolValue(std::string attr, bool val)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	int ret = iio_channel_attr_write_bool(m_channel, attr.c_str(), val);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot write " + attr, libm2k::EXC_INVALID_PARAMETER, ret);
	}
}

bool Channel::getBoolValue(std::string attr)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	bool value;
	int ret = iio_channel_attr_read_bool(m_channel, attr.c_str(), &value);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot write " + attr, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	return value;
}

std::vector<std::string> Channel::getAvailableAttributeValues(const std::string &attr)
{
	std::vector<std::string> values;
	std::string valuesAsString;

	__try {
		valuesAsString = getStringValue(std::string(attr + "_available"));
		std::istringstream iss(valuesAsString);
		values = std::vector<std::string>(std::istream_iterator<std::string>{iss},
						  std::istream_iterator<std::string>());
	} __catch (exception_type &e) {
		values.push_back(getStringValue(attr));
	}
	return values;
}

bool Channel::isValid()
{
	return !!m_channel;
}

struct iio_channel* Channel::getChannel()
{
	return m_channel;
}
