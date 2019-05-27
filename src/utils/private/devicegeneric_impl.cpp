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
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file LICENSE.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <libm2k/utils/devicegeneric.hpp>
#include <libm2k/utils/buffer.hpp>
#include <libm2k/utils/channel.hpp>
#include <libm2k/utils/utils.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/context.hpp>
#include <algorithm>
#include <cstring>

extern "C" {
	struct iio_context;
	struct iio_device;
}

using namespace std;
using namespace libm2k::utils;
using namespace libm2k::devices;

class DeviceGeneric::DeviceGenericImpl {
public:

	/*
	 * Represents an iio_device
	 */
	DeviceGenericImpl(struct iio_context* context, std::string dev_name = "", bool input = false)
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
							delete chn;
							chn = nullptr;
							continue;
						}
					} else {
						std::string name = "voltage" + std::to_string(i);

						chn = new Channel(m_dev, name.c_str(), true);
						if (!chn->isValid()) {
							delete chn;
							chn = nullptr;
							chn = new Channel(m_dev, name.c_str(), false);
						}
					}
					if (!chn->isValid()) {
						delete chn;
						chn = nullptr;
						continue;
					}
					m_channel_list.push_back(chn);
				}
			} __catch (std::exception &e) {

			}
		}
	}

	virtual ~DeviceGenericImpl()
	{
		if (m_buffer) {
			delete m_buffer;
			m_buffer = nullptr;
		}
		for (auto ch : m_channel_list) {
			delete ch;
		}
		m_channel_list.clear();
	}

	Channel* getChannel(unsigned int chnIdx)
	{
		if (chnIdx >= m_channel_list.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Device: No such channel: " + to_string(chnIdx));
		}
		return m_channel_list.at(chnIdx);
	}

	Channel* getChannel(std::string id)
	{
		for (auto ch : m_channel_list) {
			if (id == ch->getName()) {
				return ch;
			} else if (id == ch->getId()) {
				return ch;
			}
		}
		return nullptr;
	}

	bool isChannel(unsigned int chnIdx, bool output)
	{
		std::string name = "voltage" + to_string(chnIdx);
		auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
		if (chn) {
			return true;
		} else {
			return false;
		}
	}

	virtual void enableChannel(unsigned int chnIdx, bool enable)
	{
		getChannel(chnIdx)->enableChannel(enable);
	}

	bool isChannelEnabled(unsigned int chnIdx)
	{
		return getChannel(chnIdx)->isEnabled();
	}


	void push(std::vector<short> const &data, unsigned int channel,
			  bool cyclic = true, bool multiplex = false)
	{
		if (!m_buffer) {
			throw_exception(EXC_RUNTIME_ERROR, "Device: Can not push; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		m_buffer->push(data, channel, cyclic, multiplex);
	}

	void push(std::vector<double> const &data, unsigned int channel, bool cyclic = true)
	{
		if (!m_buffer) {
			throw_exception(EXC_RUNTIME_ERROR, "Device: Can not push; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		m_buffer->push(data, channel, cyclic);
	}

	void stop()
	{
		if (m_buffer) {
			m_buffer->stop();
		}
	}

	std::vector<unsigned short> getSamples(unsigned int nb_samples)
	{
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: Can not refill; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		return m_buffer->getSamples(nb_samples);

	}

	std::vector<std::vector<double> > getSamples(unsigned int nb_samples,
					std::function<double(int16_t, unsigned int)> process)
	{
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: Can not refill; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		return m_buffer->getSamples(nb_samples, process);
	}

	string getName()
	{
		if (!m_dev) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: No available device");
		}
		return iio_device_get_name(m_dev);
	}

	double getDoubleValue(std::string attr)
	{
		double value = 0;
		std::string dev_name = getName();

		if (Context::iioDevHasAttribute(m_dev, attr)) {
			iio_device_attr_read_double(m_dev, attr.c_str(),
				&value);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
					attr + " attribute");
		}
		return value;
	}

	double getDoubleValue(unsigned int chn_idx, std::string attr, bool output = false)
	{
		double value = 0;
		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		std::string dev_name = getName();

		if (chn_idx >= nb_channels) {
			throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no such channel");
		}

		std::string name = "voltage" + std::to_string(chn_idx);
		auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
		if (Context::iioChannelHasAttribute(chn, attr)) {
			iio_channel_attr_read_double(chn, attr.c_str(), &value);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
					attr + " attribute for the selected channel");
		}
		return value;
	}

	double setDoubleValue(double value, std::string attr)
	{
		std::string dev_name = iio_device_get_name(m_dev);
		if (Context::iioDevHasAttribute(m_dev, attr)) {
			iio_device_attr_write_double(m_dev, attr.c_str(),
				value);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, dev_name + " has no " +
					attr + " attribute");
		}
		return getDoubleValue(attr);
	}

	double setDoubleValue(unsigned int chn_idx, double value, std::string attr, bool output = false)
	{
		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		std::string dev_name = getName();
		if (chn_idx >= nb_channels) {
			throw_exception(EXC_INVALID_PARAMETER, dev_name +
					" has no such channel");
		}

		std::string name = "voltage" + std::to_string(chn_idx);
		auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
		if (Context::iioChannelHasAttribute(chn, attr)) {
			iio_channel_attr_write_double(chn, attr.c_str(),
				value);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, dev_name +
					" has no " + attr +
					" attribute for the selected channel");
		}
		return getDoubleValue(chn_idx, attr, output);
	}

	bool getBoolValue(string attr)
	{
		bool value = 0;
		std::string dev_name = getName();

		if (Context::iioDevHasAttribute(m_dev, attr)) {
			iio_device_attr_read_bool(m_dev, attr.c_str(),
				&value);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, dev_name +
					" has no " + attr + " attribute");
		}
		return value;
	}

	bool getBoolValue(unsigned int chn_idx, string attr, bool output = false)
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
		if (Context::iioChannelHasAttribute(chn, attr)) {
			iio_channel_attr_read_bool(chn, attr.c_str(),
				&value);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, dev_name +
					" has no " + attr +
					" attribute for the selected channel");
		}
		return value;
	}

	bool setBoolValue(bool value, string attr)
	{
		std::string dev_name = iio_device_get_name(m_dev);
		if (Context::iioDevHasAttribute(m_dev, attr)) {
			iio_device_attr_write_bool(m_dev, attr.c_str(), value);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, dev_name +
					" has no " + attr +
					" attribute");
		}
		return getBoolValue(attr);
	}

	bool setBoolValue(unsigned int chn_idx, bool value, string attr, bool output = false)
	{
		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		std::string dev_name = getName();
		if (chn_idx >= nb_channels) {
			throw_exception(EXC_INVALID_PARAMETER, dev_name +
					" has no such channel");
		}

		std::string name = "voltage" + std::to_string(chn_idx);
		auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
		if (Context::iioChannelHasAttribute(chn, attr)) {
			iio_channel_attr_write_bool(chn, attr.c_str(),
				value);
		} else {
			throw_exception(EXC_INVALID_PARAMETER, dev_name +
					" has no " + attr +
					" attribute for the selected channel");
		}
		return getBoolValue(chn_idx, attr, output);
	}

	string setStringValue(string attr, string value)
	{
		std::string dev_name = iio_device_get_name(m_dev);
		if (Context::iioDevHasAttribute(m_dev, attr)) {
			iio_device_attr_write(m_dev, attr.c_str(), value.c_str());
		} else {
			throw_exception(EXC_INVALID_PARAMETER, dev_name +
					" has no " + attr + " attribute");
		}
		return getStringValue(attr);
	}

	string setStringValue(unsigned int chn_idx, string attr, string value, bool output = false)
	{
		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		std::string dev_name = getName();
		if (chn_idx >= nb_channels) {
			throw_exception(EXC_INVALID_PARAMETER, dev_name +
					" has no such channel");
		}

		std::string name = "voltage" + std::to_string(chn_idx);
		auto chn = iio_device_find_channel(m_dev, name.c_str(), output);
		if (Context::iioChannelHasAttribute(chn, attr)) {
			iio_channel_attr_write(chn, attr.c_str(), value.c_str());
		} else {
			throw_exception(EXC_INVALID_PARAMETER, dev_name +
					" has no " + attr +
					" attribute for the selected channel");
		}
		return getStringValue(chn_idx, attr, output);
	}

	string getStringValue(string attr)
	{
		char value[100];
		std::string dev_name = getName();

		if (Context::iioDevHasAttribute(m_dev, attr)) {
			iio_device_attr_read(m_dev, attr.c_str(),
				value, sizeof(value));
		} else {
			throw_exception(EXC_INVALID_PARAMETER, dev_name +
					" has no " + attr + " attribute");
		}
		return std::string(value);
	}

	string getStringValue(unsigned int chn_idx, string attr, bool output = false)
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
		if (Context::iioChannelHasAttribute(chn, attr)) {
			iio_channel_attr_read(chn, attr.c_str(),
					value, sizeof(value));
		} else {
			throw_exception(EXC_INVALID_PARAMETER, dev_name +
					" has no " + attr +
					" attribute for the selected channel");
		}
		return value;
	}

	std::vector<double> getAvailableSampleRates()
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

	void writeRegister(uint32_t address, uint32_t value)
	{
		int ret = iio_device_reg_write(m_dev, address, value);
		if (ret) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: can't write register" +
							  std::string(std::strerror(-ret)));
		}
	}

	std::string getHardwareRevision()
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

	unsigned int getNbChannels()
	{
		return m_channel_list.size();
	}

	void convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src)
	{
		if (chn_idx < m_channel_list.size()) {
			m_channel_list.at(chn_idx)->convert(avg, src);
		} else {
			throw_exception(EXC_OUT_OF_RANGE, "Device: No such channel");
		}
	}

	void convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src)
	{
		if (chn_idx < m_channel_list.size()) {
			m_channel_list.at(chn_idx)->convert(avg, src);
		} else {
			throw_exception(EXC_OUT_OF_RANGE, "Device: No such channel");
		}
	}

	void setKernelBuffersCount(unsigned int count)
	{
		if (!m_dev) {
			throw_exception(EXC_RUNTIME_ERROR, "Device: no such device");
		}
		iio_device_set_kernel_buffers_count(m_dev, count);
	}

	bool isValidDmmChannel(unsigned int chnIdx)
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

	std::pair<string, string> getContextAttr(unsigned int attrIdx)
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

private:
	struct iio_context *m_context;
	struct iio_device *m_dev;
	std::vector<Channel*> m_channel_list;
	Buffer* m_buffer;
};

