/*
 * Copyright 2016 Analog Devices, Inc.
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

#include <libm2k/utils/device.hpp>
#include <libm2k/utils/buffer.hpp>
#include <libm2k/utils/channel.hpp>
#include <libm2k/utils/utils.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <algorithm>
#include <cstring>

using namespace std;
using namespace libm2k::utils;

/*
 * Represents an iio_device
 */
Device::Device(struct iio_context* context, std::string dev_name, bool input)
{
	m_context = context;
	m_dev = nullptr;
	m_channel_list = {};
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

		__try {
			unsigned int nb_channels = iio_device_get_channels_count(m_dev);
			for (unsigned int i = 0; i < nb_channels; i++) {
				Channel *chn = nullptr;
				if (input) {
					chn = new Channel(m_dev, i);
					if (chn->isOutput()) {
						continue;
					}
				} else {
					std::string name = "voltage" + std::to_string(i);
					__try {
						chn = new Channel(m_dev, name.c_str(), true);
					} __catch (exception_type &e) {
						chn = new Channel(m_dev, name.c_str(), false);
					}
				}
				m_channel_list.push_back(chn);
			}
		} catch (std::exception &e) {

		}
	}
}

Device::~Device()
{

}

Channel* Device::getChannel(unsigned int chnIdx)
{
	if (chnIdx < m_channel_list.size()) {
		return m_channel_list.at(chnIdx);
	} else {
		throw_exception(EXC_OUT_OF_RANGE, "Device: No such channel: " + to_string(chnIdx));
	}
}

Channel* Device::getChannel(std::string id)
{
	for (auto ch : m_channel_list) {
		if (id == ch->getName()) {
			return ch;
		}
	}
	return nullptr;
}

bool Device::isChannel(unsigned int chnIdx, bool output)
{
	std::string name = "voltage" + to_string(chnIdx);
	auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
	if (chn) {
		return true;
	} else {
		return false;
	}
}

void Device::enableChannel(unsigned int chnIdx, bool enable)
{
	getChannel(chnIdx)->enableChannel(enable);
}

bool Device::isChannelEnabled(unsigned int chnIdx)
{
	return getChannel(chnIdx)->isEnabled();
}


void Device::push(std::vector<short> &data, unsigned int channel,
		  bool cyclic, bool multiplex)
{
	if (!m_buffer) {
		throw_exception(EXC_RUNTIME_ERROR, "Device: Can not push; device not buffer capable");
	}
	m_buffer->setChannels(m_channel_list);
	m_buffer->push(data, channel, cyclic, multiplex);
}

void Device::push(std::vector<double> &data, unsigned int channel, bool cyclic)
{
	if (!m_buffer) {
		throw_exception(EXC_RUNTIME_ERROR, "Device: Can not push; device not buffer capable");
	}
	m_buffer->setChannels(m_channel_list);
	m_buffer->push(data, channel, cyclic);
}

void Device::stop()
{
	if (m_buffer) {
		m_buffer->stop();
	}
}

std::vector<unsigned short> Device::getSamples(int nb_samples)
{
	if (!m_buffer) {
		throw_exception(EXC_INVALID_PARAMETER, "Device: Can not refill; device not buffer capable");
	}
	m_buffer->setChannels(m_channel_list);
	return m_buffer->getSamples(nb_samples);

}

std::vector<std::vector<double> > Device::getSamples(int nb_samples,
				std::function<double(int16_t, unsigned int)> process)
{
	if (!m_buffer) {
		throw_exception(EXC_INVALID_PARAMETER, "Device: Can not refill; device not buffer capable");
	}
	m_buffer->setChannels(m_channel_list);
	return m_buffer->getSamples(nb_samples, process);
}

string Device::getName()
{
	if (!m_dev) {
		throw_exception(EXC_INVALID_PARAMETER, "Device: No available device");
	}
	return iio_device_get_name(m_dev);
}

double Device::getDoubleValue(std::string attr)
{
	double value = 0;
	std::string dev_name = getName();

	if (Utils::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_read_double(m_dev, attr.c_str(),
			&value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
				attr + " attribute");
	}
	return value;
}

double Device::getDoubleValue(unsigned int chn_idx, std::string attr, bool output)
{
	double value = 0;
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();

	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no such channel");
	}

	std::string name = "voltage" + std::to_string(chn_idx);
	auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
	if (Utils::iioChannelHasAttribute(chn, attr)) {
		iio_channel_attr_read_double(chn, attr.c_str(), &value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
				attr + " attribute for the selected channel");
	}
	return value;
}

double Device::setDoubleValue(double value, std::string attr)
{
	std::string dev_name = iio_device_get_name(m_dev);
	if (Utils::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_write_double(m_dev, attr.c_str(),
			value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
				attr + " attribute");
	}
	return getDoubleValue(attr);
}

double Device::setDoubleValue(unsigned int chn_idx, double value, std::string attr, bool output)
{
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();
	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no such channel");
	}

	std::string name = "voltage" + std::to_string(chn_idx);
	auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
	if (Utils::iioChannelHasAttribute(chn, attr)) {
		iio_channel_attr_write_double(chn, attr.c_str(),
			value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute for the selected channel");
	}
	return getDoubleValue(chn_idx, attr, output);
}

bool Device::getBoolValue(string attr)
{
	bool value = 0;
	std::string dev_name = getName();

	if (Utils::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_read_bool(m_dev, attr.c_str(),
			&value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr + " attribute");
	}
	return value;
}

bool Device::getBoolValue(unsigned int chn_idx, string attr, bool output)
{
	bool value = 0;
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();

	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no such channel");
	}

	std::string name = "voltage" + std::to_string(chn_idx);
	auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
	if (Utils::iioChannelHasAttribute(chn, attr)) {
		iio_channel_attr_read_bool(chn, attr.c_str(),
			&value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute for the selected channel");
	}
	return value;
}

bool Device::setBoolValue(bool value, string attr)
{
	std::string dev_name = iio_device_get_name(m_dev);
	if (Utils::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_write_bool(m_dev, attr.c_str(), value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute");
	}
	return getBoolValue(attr);
}

bool Device::setBoolValue(unsigned int chn_idx, bool value, string attr, bool output)
{
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();
	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no such channel");
	}

	std::string name = "voltage" + std::to_string(chn_idx);
	auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
	if (Utils::iioChannelHasAttribute(chn, attr)) {
		iio_channel_attr_write_bool(chn, attr.c_str(),
			value);
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute for the selected channel");
	}
	return getBoolValue(chn_idx, attr, output);
}

string Device::setStringValue(string attr, string value)
{
	std::string dev_name = iio_device_get_name(m_dev);
	if (Utils::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_write(m_dev, attr.c_str(), value.c_str());
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr + " attribute");
	}
	return getStringValue(attr);
}

string Device::setStringValue(unsigned int chn_idx, string attr, string value, bool output)
{
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();
	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no such channel");
	}

	std::string name = "voltage" + std::to_string(chn_idx);
	auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
	if (Utils::iioChannelHasAttribute(chn, attr)) {
		iio_channel_attr_write(chn, attr.c_str(), value.c_str());
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute for the selected channel");
	}
	return getStringValue(chn_idx, attr, output);
}

string Device::getStringValue(string attr)
{
	char value[100];
	std::string dev_name = getName();

	if (Utils::iioDevHasAttribute(m_dev, attr)) {
		iio_device_attr_read(m_dev, attr.c_str(),
			value, sizeof(value));
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr + " attribute");
	}
	return std::string(value);
}

string Device::getStringValue(unsigned int chn_idx, string attr, bool output)
{
	char value[100];
	unsigned int nb_channels = iio_device_get_channels_count(m_dev);
	std::string dev_name = getName();

	if (chn_idx >= nb_channels) {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no such channel");
	}

	std::string name = "voltage" + std::to_string(chn_idx);
	auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
	if (Utils::iioChannelHasAttribute(chn, attr)) {
		iio_channel_attr_read(chn, attr.c_str(),
				value, sizeof(value));
	} else {
		throw_exception(EXC_INVALID_PARAMETER, dev_name +
				" has no " + attr +
				" attribute for the selected channel");
	}
	return value;
}

std::vector<double> Device::getAvailableSamplerates()
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
//				std::cout << "Not a valid samplerate " << e.what();
			}
		}
	}

	std::sort(values.begin(), values.end());
	return values;
}

void Device::writeRegister(uint32_t address, uint32_t value)
{
	int ret = iio_device_reg_write(m_dev, address, value);
	if (ret) {
		throw_exception(EXC_INVALID_PARAMETER, "Device: can't write register" +
						  std::string(std::strerror(-ret)));
	}
}

std::string Device::getHardwareRevision()
{
	const char *hw_rev_attr_val = iio_context_get_attr_value(m_context, "hw_model");
	std::string rev;

	if (hw_rev_attr_val) {
		std::string const s = hw_rev_attr_val;
		std::string const key = "Rev.";
		int n = s.find(key);

		n += key.length();
		rev =  s.substr(n, 1);
	} else {
		rev = "A";
	}

	return rev;
}

unsigned int Device::getNbChannels()
{
	return m_channel_list.size();
}

void Device::convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src)
{
	if (chn_idx < m_channel_list.size()) {
		m_channel_list.at(chn_idx)->convert(avg, src);
	} else {
		throw_exception(EXC_OUT_OF_RANGE, "Device: No such channel");
	}
}

void Device::convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src)
{
	if (chn_idx < m_channel_list.size()) {
		m_channel_list.at(chn_idx)->convert(avg, src);
	} else {
		throw_exception(EXC_OUT_OF_RANGE, "Device: No such channel");
	}
}

void Device::setKernelBuffersCount(unsigned int count)
{
	if (!m_dev) {
		throw_exception(EXC_RUNTIME_ERROR, "Device: no such device");
	}
	iio_device_set_kernel_buffers_count(m_dev, count);
}

bool Device::isValidDmmChannel(unsigned int chnIdx)
{
	if (chnIdx >= m_channel_list.size()) {
		throw_exception(EXC_OUT_OF_RANGE, "Device: no such DMM channel");
	}

	auto chn = m_channel_list.at(chnIdx);
	if (chn->isOutput()) {
		return false;
	}

	if (chn->hasAttribute("raw") || chn->hasAttribute("input") ||
			chn->hasAttribute("processed")) {
		return true;
	}
	return false;
}


