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

#include "devicegeneric.hpp"
#include "utils/buffer.hpp"
#include "utils/channel.hpp"
#include <libm2k/utils/utils.hpp>
#include <libm2k/m2kexceptions.hpp>
#include "context_impl.hpp"
#include <algorithm>
#include <cstring>

using namespace std;
using namespace libm2k::utils;
using namespace libm2k::context;


/** Represents an iio_device **/
DeviceGeneric::DeviceGeneric(struct iio_context* context, std::string dev_name)
{
	m_context = context;
	m_dev = nullptr;
	m_buffer = nullptr;

	if (dev_name != "") {
		m_dev = iio_context_find_device(context, dev_name.c_str());
		if (!m_dev) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: No such device");
		}

		__try {
			m_buffer = new Buffer(m_dev);
		} __catch (exception_type &e) {
			delete m_buffer;
			m_buffer = nullptr;
		}

		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		for (unsigned int i = 0; i < nb_channels; i++) {
			Channel *chn = nullptr;
			chn = new Channel(m_dev, i);
			if (!chn->isValid()) {
				delete chn;
				chn = nullptr;
				continue;
			}

			if (chn->isOutput()) {
				m_channel_list_out.push_back(chn);
			} else {
				m_channel_list_in.push_back(chn);
			}
		}
		std::sort(m_channel_list_out.begin(), m_channel_list_out.end(), [](Channel* lchn, Channel* rchn)
		{
			return Utils::compareNatural(lchn->getId(), rchn->getId());
		});
		std::sort(m_channel_list_in.begin(), m_channel_list_in.end(), [](Channel* lchn, Channel* rchn)
		{
			return Utils::compareNatural(lchn->getId(), rchn->getId());
		});
	}
}

DeviceGeneric::~DeviceGeneric()
{
	if (m_buffer) {
		delete m_buffer;
		m_buffer = nullptr;
	}
	for (auto ch : m_channel_list_in) {
		delete ch;
	}
	for (auto ch : m_channel_list_out) {
		delete ch;
	}
	m_channel_list_out.clear();
	m_channel_list_in.clear();
}

Channel* DeviceGeneric::getChannel(unsigned int chnIdx, bool output)
{
	if (output) {
		if (chnIdx >= m_channel_list_out.size()) {
			goto error;
		}
		return m_channel_list_out.at(chnIdx);
	} else {
		if (chnIdx >= m_channel_list_in.size()) {
			goto error;
		}
		return m_channel_list_in.at(chnIdx);
	}
error:
	throw_exception(EXC_OUT_OF_RANGE, "Device: No such channel: " + to_string(chnIdx));
	return nullptr;
}

Channel* DeviceGeneric::getChannel(std::string id, bool output)
{
	std::vector<Channel*> tmp = output ? m_channel_list_out : m_channel_list_in;
	for (auto ch : tmp) {
		if (id == ch->getName()) {
			return ch;
		} else if (id == ch->getId()) {
			return ch;
		}
	}
	return nullptr;
}

bool DeviceGeneric::isChannel(unsigned int chnIdx, bool output)
{
	std::string name = "voltage" + to_string(chnIdx);
	auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
	if (chn) {
		return true;
	} else {
		return false;
	}
}

void DeviceGeneric::enableChannel(unsigned int chnIdx, bool enable, bool output)
{
	getChannel(chnIdx, output)->enableChannel(enable);
}

bool DeviceGeneric::isChannelEnabled(unsigned int chnIdx, bool output)
{
	return getChannel(chnIdx, output)->isEnabled();
}

string DeviceGeneric::getName()
{
	if (!m_dev) {
		throw_exception(EXC_INVALID_PARAMETER, "Device: No available device");
	}
	return iio_device_get_name(m_dev);
}

double DeviceGeneric::getDoubleValue(std::string attr)
{
	double value = 0;
	std::string dev_name = getName();

	if (ContextImpl::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_read_double(m_dev, attr.c_str(),
					    &value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
				attr + " attribute");
	}
	return value;
}

double DeviceGeneric::getDoubleValue(unsigned int chn_idx, std::string attr, bool output)
{
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();

	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no such channel");
	}

	auto chn = getChannel(chn_idx, output);
	if (!chn->hasAttribute(attr)) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
				attr + " attribute for the selected channel");
	}

	return chn->getDoubleValue(attr);
}

double DeviceGeneric::setDoubleValue(double value, std::string attr)
{
	std::string dev_name = iio_device_get_name(m_dev);
	if (ContextImpl::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_write_double(m_dev, attr.c_str(),
					     value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
				attr + " attribute");
	}
	return getDoubleValue(attr);
}

double DeviceGeneric::setDoubleValue(unsigned int chn_idx, double value, std::string attr, bool output)
{
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();
	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no such channel");
	}

	auto chn = getChannel(chn_idx, output);
	if (!chn->hasAttribute(attr)) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute for the selected channel");
	}

	chn->setDoubleValue(attr, value);
	return chn->getDoubleValue(attr);
}

int DeviceGeneric::getLongValue(std::string attr)
{
	long long value = 0;
	std::string dev_name = getName();

	if (ContextImpl::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_read_longlong(m_dev, attr.c_str(),
					      &value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
				attr + " attribute");
	}
	return value;
}

int DeviceGeneric::getLongValue(unsigned int chn_idx, std::string attr, bool output)
{
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();

	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no such channel");
	}

	auto chn = getChannel(chn_idx, output);
	if (!chn->hasAttribute(attr)) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
				attr + " attribute for the selected channel");
	}

	return chn->getLongValue(attr);
}

int DeviceGeneric::getBufferLongValue(std::string attr)
{
	long long value = 0;
	std::string dev_name = getName();

	if (ContextImpl::iioDevBufferHasAttribute(m_dev, attr)) {
		iio_device_buffer_attr_read_longlong(m_dev, attr.c_str(),
						     &value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
				attr + " attribute");
	}
	return value;
}

int DeviceGeneric::setLongValue(int value, std::string attr)
{
	std::string dev_name = iio_device_get_name(m_dev);
	if (ContextImpl::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_write_longlong(m_dev, attr.c_str(),
					       value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
				attr + " attribute");
	}
	return getLongValue(attr);
}

int DeviceGeneric::setLongValue(unsigned int chn_idx, int value, std::string attr, bool output)
{
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();
	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no such channel");
	}

	auto chn = getChannel(chn_idx, output);
	if (!chn->hasAttribute(attr)) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute for the selected channel");
	}

	chn->setLongValue(attr, value);
	return chn->getLongValue(attr);
}

int DeviceGeneric::setBufferLongValue(int value, std::string attr)
{
	std::string dev_name = iio_device_get_name(m_dev);
	if (ContextImpl::iioDevBufferHasAttribute(m_dev, attr)) {
		iio_device_buffer_attr_write_longlong(m_dev, attr.c_str(),
						      value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
				attr + " attribute");
	}
	return getLongValue(attr);
}

bool DeviceGeneric::getBoolValue(string attr)
{
	bool value = 0;
	std::string dev_name = getName();

	if (ContextImpl::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_read_bool(m_dev, attr.c_str(),
					  &value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr + " attribute");
	}
	return value;
}

bool DeviceGeneric::getBoolValue(unsigned int chn_idx, string attr, bool output)
{
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();

	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no such channel");
	}

	auto chn = getChannel(chn_idx, output);
	if (!chn->hasAttribute(attr)) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute for the selected channel");
	}

	return chn->getBoolValue(attr);
}

bool DeviceGeneric::setBoolValue(bool value, string attr)
{
	std::string dev_name = iio_device_get_name(m_dev);
	if (ContextImpl::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_write_bool(m_dev, attr.c_str(), value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute");
	}
	return getBoolValue(attr);
}

bool DeviceGeneric::setBoolValue(unsigned int chn_idx, bool value, string attr, bool output)
{
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();
	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no such channel");
	}

	auto chn = getChannel(chn_idx, output);
	if (!chn->hasAttribute(attr)) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute for the selected channel");
	}

	chn->setBoolValue(attr, value);
	return chn->getBoolValue(attr);
}

string DeviceGeneric::setStringValue(string attr, string value)
{
	std::string dev_name = iio_device_get_name(m_dev);
	if (ContextImpl::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_write(m_dev, attr.c_str(), value.c_str());
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr + " attribute");
	}
	return getStringValue(attr);
}

string DeviceGeneric::setStringValue(unsigned int chn_idx, string attr, string value, bool output)
{
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();
	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no such channel");
	}

	auto chn = getChannel(chn_idx, output);
	if (!chn->hasAttribute(attr)) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute for the selected channel");
	}

	chn->setStringValue(attr, value);
	return chn->getStringValue(attr);
}

string DeviceGeneric::getStringValue(string attr)
{
	char value[100];
	std::string dev_name = getName();

	if (ContextImpl::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_read(m_dev, attr.c_str(),
				     value, sizeof(value));
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr + " attribute");
	}
	return std::string(value);
}

string DeviceGeneric::getStringValue(unsigned int chn_idx, string attr, bool output)
{
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();

	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no such channel");
	}

	auto chn = getChannel(chn_idx, output);
	if (!chn->hasAttribute(attr)) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute for the selected channel");
	}

	return chn->getStringValue(attr);
}

std::vector<double> DeviceGeneric::getAvailableSampleRates()
{
	std::vector<double> values = {};
	std::vector<std::string> str_values;
	char buf[1024];
	int ret;

	ret = iio_device_attr_read(m_dev, "sampling_frequency_available",
				   buf, sizeof(buf));

	if (ret > 0) {
		str_values = Utils::split(buf, " ");

		for (auto it : str_values) {
			values.push_back(std::stod(it));
		}
	}

	if (values.empty()) {
		ret = iio_device_attr_read(m_dev, "sampling_frequency",
					   buf, sizeof(buf));

		if (!ret) {
			__try {
				values.push_back(std::stoul(buf));
			} __catch (exception_type &e) {
				throw_exception(EXC_INVALID_PARAMETER, "Device: Not a valid samplerate.");
			}
		}
	}

	std::sort(values.begin(), values.end());
	return values;
}

void DeviceGeneric::writeRegister(uint32_t address, uint32_t value)
{
	int ret = iio_device_reg_write(m_dev, address, value);
	if (ret) {
		throw_exception(EXC_INVALID_PARAMETER, "Device: can't write register" +
				std::string(std::strerror(-ret)));
	}
}

std::string DeviceGeneric::getHardwareRevision()
{
	const char *hw_rev_attr_val = iio_context_get_attr_value(m_context, "hw_model");
	std::string rev;

	if (hw_rev_attr_val) {
		std::string const s = hw_rev_attr_val;
		std::string const key = "Rev.";
		unsigned long n = s.find(key);

		n += key.length();
		rev =  s.substr(n, 1);
	} else {
		rev = "A";
	}

	return rev;
}

unsigned int DeviceGeneric::getNbChannels(bool output)
{
	if (output) {
		return m_channel_list_out.size();
	} else {
		return m_channel_list_in.size();
	}
}

void DeviceGeneric::convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src, bool output)
{
	std::vector<Channel*> tmp = output ? m_channel_list_out : m_channel_list_in;
	if (chn_idx < tmp.size()) {
		tmp.at(chn_idx)->convert(avg, src);
	} else {
		throw_exception(EXC_OUT_OF_RANGE, "Device: No such channel");
	}
}

void DeviceGeneric::convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src, bool output)
{
	std::vector<Channel*> tmp = output ? m_channel_list_out : m_channel_list_in;
	if (chn_idx < tmp.size()) {
		tmp.at(chn_idx)->convert(avg, src);
	} else {
		throw_exception(EXC_OUT_OF_RANGE, "Device: No such channel");
	}
}

void DeviceGeneric::setKernelBuffersCount(unsigned int count)
{
	if (!m_dev) {
		throw_exception(EXC_RUNTIME_ERROR, "Device: no such device");
	}
	iio_device_set_kernel_buffers_count(m_dev, count);
}

bool DeviceGeneric::isValidDmmChannel(unsigned int chnIdx)
{
	if (chnIdx >= m_channel_list_in.size()) {
		throw_exception(EXC_OUT_OF_RANGE, "Device: no such DMM channel");
	}

	auto chn = m_channel_list_in.at(chnIdx);
	if (chn->isOutput()) {
		return false;
	}

	if (chn->hasAttribute("raw") || chn->hasAttribute("input") ||
			chn->hasAttribute("processed")) {
		return true;
	}
	return false;
}

std::pair<string, string> DeviceGeneric::getContextAttr(unsigned int attrIdx)
{
	std::pair<std::string, std::string> pair;
	const char *name;
	const char *value;
	int ret = iio_context_get_attr(m_context, attrIdx, &name, &value);
	if (ret < 0) {
		throw_exception(EXC_RUNTIME_ERROR, "Device: Can't get context attribute " +
				std::to_string(attrIdx));
	}
	pair.first = std::string(name);
	pair.second = std::string(value);
	return pair;
}

void DeviceGeneric::setCyclic(bool enable)
{
	if (m_buffer) {
		m_buffer->setCyclic(enable);
	}
}

bool DeviceGeneric::hasGlobalAttribute(string attr)
{
	return ContextImpl::iioDevHasAttribute(m_dev, attr);
}

bool DeviceGeneric::hasBufferAttribute(string attr)
{
	return ContextImpl::iioDevBufferHasAttribute(m_dev, attr);
}
