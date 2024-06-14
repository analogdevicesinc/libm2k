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

#include "channel_v1.hpp"
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/logger.hpp>
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
	m_dev_name = iio_device_get_name(m_device);
	m_channel_id = iio_channel_get_id(m_channel);

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

bool Channel::isEnabled(struct iio_channels_mask *mask)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	return iio_channel_is_enabled(m_channel, mask);
}

unsigned int Channel::getNbAttributes()
{
	return iio_channel_get_attrs_count(m_channel);
}

std::string Channel::getAttributeName(unsigned int idx)
{
	const struct iio_attr *attr = iio_channel_get_attr(m_channel, idx);
	if (attr != NULL) {
		return iio_attr_get_name(attr);
	}
	return "";
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

void Channel::enableChannel(bool enable, struct iio_channels_mask *mask)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}

	if (enable) {
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name, m_channel_id}, "Enable channel"));
		iio_channel_enable(m_channel, mask);
	} else {
		LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name, m_channel_id}, "Disable channel"));
		iio_channel_disable(m_channel, mask);
	}
}

uintptr_t Channel::getFirst(const struct iio_block *block)
{
	return reinterpret_cast<uintptr_t>(getFirstVoid(block));
}


void* Channel::getFirstVoid(const struct iio_block *block)
{
	return iio_block_first(block, m_channel);
}

void Channel::write(const struct iio_block* block, void *data, ssize_t data_type_size, size_t nb_samples,
		    bool raw)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}

	size_t ret = iio_channel_write(m_channel, (struct iio_block*)block, data,
				       nb_samples * data_type_size, raw);

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

double Channel::getDoubleValue(std::string attr_name)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	const struct iio_attr *attr = iio_channel_find_attr(m_channel, attr_name.c_str());
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Channel: Cannot find attribute: " + attr_name, libm2k::EXC_INVALID_PARAMETER);
	}

	double value = 0.0;
	int ret = iio_attr_read_double(attr, &value);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot read " + attr_name, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name, m_channel_id, attr_name.c_str(), LIBM2K_ATTRIBUTE_READ},
                                         std::to_string(value).c_str()));
	return value;
}

void Channel::setDoubleValue(std::string attr_name, double val)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	const struct iio_attr *attr = iio_channel_find_attr(m_channel, attr_name.c_str());
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Channel: Cannot find attribute: " + attr_name, libm2k::EXC_INVALID_PARAMETER);
	}

	int ret = iio_attr_write_double(attr, val);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot write " + attr_name, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	LIBM2K_LOG(INFO,
	       libm2k::buildLoggingMessage({m_dev_name, m_channel_id, attr_name.c_str(), LIBM2K_ATTRIBUTE_WRITE},
                                   std::to_string(val).c_str()));
}

void Channel::setLongValue(std::string attr_name, long long val)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	const struct iio_attr *attr = iio_channel_find_attr(m_channel, attr_name.c_str());
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Channel: Cannot find attribute: " + attr_name, libm2k::EXC_INVALID_PARAMETER);
	}

	int ret = iio_attr_write_longlong(attr, val);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot write " + attr_name, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	LIBM2K_LOG(INFO,
	       libm2k::buildLoggingMessage({m_dev_name, m_channel_id, attr_name.c_str(), LIBM2K_ATTRIBUTE_WRITE},
                                   std::to_string(val).c_str()));
}

long long Channel::getLongValue(std::string attr_name)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	const struct iio_attr *attr = iio_channel_find_attr(m_channel, attr_name.c_str());
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Channel: Cannot find attribute: " + attr_name, libm2k::EXC_INVALID_PARAMETER);
	}

	long long value = 0;
	int ret = iio_attr_read_longlong(attr, &value);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot write " + attr_name, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name, m_channel_id, attr_name.c_str(), LIBM2K_ATTRIBUTE_READ},
                                         std::to_string(value).c_str()));
	return value;
}

void Channel::setStringValue(std::string attr_name, std::string val)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	const struct iio_attr *attr = iio_channel_find_attr(m_channel, attr_name.c_str());
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Channel: Cannot find attribute: " + attr_name, libm2k::EXC_INVALID_PARAMETER);
	}

	int ret = iio_attr_write_raw(attr, val.c_str(), val.size());
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot write " + attr_name, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name, m_channel_id, attr_name.c_str(), LIBM2K_ATTRIBUTE_WRITE}, val.c_str()));
}

std::string Channel::getStringValue(std::string attr_name)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	char value[1024];
	const struct iio_attr *attr = iio_channel_find_attr(m_channel, attr_name.c_str());
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Channel: Cannot find attribute: " + attr_name, libm2k::EXC_INVALID_PARAMETER);
	}

	int ret = iio_attr_read_raw(attr, value, sizeof(value));
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot read " + attr_name, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name, m_channel_id, attr_name.c_str(), LIBM2K_ATTRIBUTE_READ}, value));
	return std::string(value);
}

void Channel::setBoolValue(std::string attr_name, bool val)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	const struct iio_attr *attr = iio_channel_find_attr(m_channel, attr_name.c_str());
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Channel: Cannot find attribute: " + attr_name, libm2k::EXC_INVALID_PARAMETER);
	}

	int ret = iio_attr_write_bool(attr, val);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot write " + attr_name, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	LIBM2K_LOG(INFO,
	       libm2k::buildLoggingMessage({m_dev_name, m_channel_id, attr_name.c_str(), LIBM2K_ATTRIBUTE_WRITE},
                                   std::to_string(val).c_str()));
}

bool Channel::getBoolValue(std::string attr_name)
{
	if (!m_channel) {
		THROW_M2K_EXCEPTION("Channel: Cannot find associated channel", libm2k::EXC_INVALID_PARAMETER);
	}
	bool value;
	const struct iio_attr *attr = iio_channel_find_attr(m_channel, attr_name.c_str());
	if (attr == NULL) {
		THROW_M2K_EXCEPTION("Channel: Cannot find attribute: " + attr_name, libm2k::EXC_INVALID_PARAMETER);
	}

	int ret = iio_attr_read_bool(attr, &value);
	if (ret < 0) {
		THROW_M2K_EXCEPTION("Channel: Cannot read " + attr_name, libm2k::EXC_INVALID_PARAMETER, ret);
	}
	LIBM2K_LOG(INFO, libm2k::buildLoggingMessage({m_dev_name, m_channel_id, attr_name.c_str(), LIBM2K_ATTRIBUTE_READ},
                                         std::to_string(value).c_str()));
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
	} __catch (exception_type&) {
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

iio_device *Channel::getDevice()
{
	return  m_device;
}
