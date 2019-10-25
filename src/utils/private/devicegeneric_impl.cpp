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
using namespace libm2k::contexts;

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
				throw_exception(EXC_INVALID_PARAMETER, "Device: No such device", __FILE__, __LINE__);
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
			throw_exception(EXC_OUT_OF_RANGE, "Device: No such channel: " + to_string(chnIdx),
					__FILE__, __LINE__);
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
			throw_exception(EXC_RUNTIME_ERROR, "Device: Cannot push; device not buffer capable",
					__FILE__, __LINE__);
		}
		m_buffer->setChannels(m_channel_list);
		m_buffer->push(data, channel, cyclic, multiplex);
	}

	void push(std::vector<unsigned short> const &data, unsigned int channel,
			  bool cyclic = true, bool multiplex = false)
	{
		if (!m_buffer) {
			throw_exception(EXC_RUNTIME_ERROR, "Device: Cannot push; device not buffer capable",
					__FILE__, __LINE__);
		}
		m_buffer->setChannels(m_channel_list);
		m_buffer->push(data, channel, cyclic, multiplex);
	}

	void push(std::vector<double> const &data, unsigned int channel, bool cyclic = true)
	{
		if (!m_buffer) {
			throw_exception(EXC_RUNTIME_ERROR, "Device: Cannot push; device not buffer capable",
					__FILE__, __LINE__);
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
			throw_exception(EXC_INVALID_PARAMETER, "Device: Cannot refill; device not buffer capable",
					__FILE__, __LINE__);
		}
		m_buffer->setChannels(m_channel_list);
		return m_buffer->getSamples(nb_samples);

	}

	std::vector<std::vector<double> > getSamples(unsigned int nb_samples,
					std::function<double(int16_t, unsigned int)> process)
	{
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: Cannot refill; device not buffer capable",
					__FILE__, __LINE__);
		}
		m_buffer->setChannels(m_channel_list);
		return m_buffer->getSamples(nb_samples, process);
	}

	string getName()
	{
		if (!m_dev) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: No available device", __FILE__, __LINE__);
		}
		return iio_device_get_name(m_dev);
	}

	double getDoubleValue(std::string attr)
	{
		std::string errstr = "";
		double value = 0;
		std::string dev_name = getName();

		if (Context::iioDevHasAttribute(m_dev, attr)) {
			int ret = iio_device_attr_read_double(m_dev, attr.c_str(), &value);
			if (ret < 0) {
				errstr += "Error reading attribute " +  dev_name + "\n ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr += dev_name + " has no "	+ attr + " attribute ";
			goto out_cleanup;
		}
	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return value;
	}

	double getDoubleValue(unsigned int chn_idx, std::string attr, bool output = false)
	{
		double value = 0;
		int ret = 0;
		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		std::string name, errstr = "";
		std::string dev_name = getName();
		struct iio_channel* chn;

		if (chn_idx >= nb_channels) {
			errstr = dev_name + " has no such channel";
			goto out_cleanup;
		}

		name = "voltage" + std::to_string(chn_idx);
		chn = iio_device_find_channel(m_dev, name.c_str(), output);

		if (Context::iioChannelHasAttribute(chn, attr)) {
			ret = iio_channel_attr_read_double(chn, attr.c_str(), &value);
			if (ret < 0) {
				errstr = "Can't read attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " +
				attr + " attribute for the selected channel";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return value;
	}

	double setDoubleValue(double value, std::string attr)
	{
		int ret = 0;
		std::string errstr = "";
		std::string dev_name = iio_device_get_name(m_dev);
		if (Context::iioDevHasAttribute(m_dev, attr)) {
			ret = iio_device_attr_write_double(m_dev, attr.c_str(), value);
			if (ret < 0) {
				errstr = "Can't write attribute " + attr + "\n";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr + " attribute";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return getDoubleValue(attr);
	}

	double setDoubleValue(unsigned int chn_idx, double value, std::string attr, bool output = false)
	{
		int ret = 0;
		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		std::string errstr = "";
		std::string name = "";
		std::string dev_name = getName();
		struct iio_channel *chn;

		if (chn_idx >= nb_channels) {
			errstr = dev_name + " has no such channel";
			goto out_cleanup;
		}

		name = "voltage" + std::to_string(chn_idx);
		chn = iio_device_find_channel(m_dev, name.c_str(), output);

		if (Context::iioChannelHasAttribute(chn, attr)) {
			ret = iio_channel_attr_write_double(chn, attr.c_str(), value);
			if (ret < 0) {
				errstr = "Can't write attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr +	" attribute for the selected channel";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return getDoubleValue(chn_idx, attr, output);
	}

	int getLongValue(std::string attr)
	{
		int ret = 0;
		long long value = 0;
		std::string errstr = "";
		std::string dev_name = getName();

		if (Context::iioDevHasAttribute(m_dev, attr)) {
			ret = iio_device_attr_read_longlong(m_dev, attr.c_str(), &value);
			if (ret < 0) {
				errstr = "Can't read attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr + " attribute";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return value;
	}

	int getLongValue(unsigned int chn_idx, std::string attr, bool output = false)
	{
		int ret = 0;
		long long value = 0;
		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		std::string errstr = "";
		std::string name = "";
		std::string dev_name = getName();
		struct iio_channel *chn;

		if (chn_idx >= nb_channels) {
			errstr = dev_name + " has no such channel";
			goto out_cleanup;
		}

		name = "voltage" + std::to_string(chn_idx);
		chn = iio_device_find_channel(m_dev, name.c_str(), output);
		if (Context::iioChannelHasAttribute(chn, attr)) {
			ret = iio_channel_attr_read_longlong(chn, attr.c_str(), &value);
			if (ret < 0) {
				errstr = "Can't read attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr + " attribute for the selected channel";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return value;
	}

	int setLongValue(int value, std::string attr)
	{
		int ret = 0;
		std::string errstr = "";
		std::string dev_name = iio_device_get_name(m_dev);
		if (Context::iioDevHasAttribute(m_dev, attr)) {
			ret = iio_device_attr_write_longlong(m_dev, attr.c_str(), value);
			if (ret < 0) {
				errstr = "Can't write attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr + " attribute";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return getLongValue(attr);
	}

	int setLongValue(unsigned int chn_idx, int value, std::string attr, bool output = false)
	{
		int ret = 0;
		std::string errstr = "";
		std::string name = "";
		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		std::string dev_name = getName();
		struct iio_channel* chn;

		if (chn_idx >= nb_channels) {
			errstr = dev_name + " has no such channel";
			goto out_cleanup;
		}

		name = "voltage" + std::to_string(chn_idx);
		chn = iio_device_find_channel(m_dev, name.c_str(), output);
		if (Context::iioChannelHasAttribute(chn, attr)) {
			ret = iio_channel_attr_write_longlong(chn, attr.c_str(), value);
			if (ret < 0) {
				errstr = "Can't write attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr +	" attribute for the selected channel";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return getLongValue(chn_idx, attr, output);
	}

	bool getBoolValue(string attr)
	{
		int ret = 0;
		bool value = 0;
		std::string errstr = "";
		std::string dev_name = getName();

		if (Context::iioDevHasAttribute(m_dev, attr)) {
			ret = iio_device_attr_read_bool(m_dev, attr.c_str(), &value);
			if (ret < 0) {
				errstr = "Can't read attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr =  dev_name + " has no " + attr + " attribute";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return value;
	}

	bool getBoolValue(unsigned int chn_idx, string attr, bool output = false)
	{
		int ret = 0;
		bool value = 0;
		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		std::string dev_name = getName();
		std::string errstr, name = "";
		struct iio_channel *chn;

		if (chn_idx >= nb_channels) {
			errstr = dev_name + " has no such channel";
			goto out_cleanup;
		}

		name = "voltage" + std::to_string(chn_idx);
		chn = iio_device_find_channel(m_dev, name.c_str(), output);
		if (Context::iioChannelHasAttribute(chn, attr)) {
			ret = iio_channel_attr_read_bool(chn, attr.c_str(), &value);
			if (ret < 0) {
				errstr = "Can't read attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr +	" attribute for the selected channel";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return value;
	}

	bool setBoolValue(bool value, string attr)
	{
		int ret = 0;
		std::string errstr = "";
		std::string dev_name = iio_device_get_name(m_dev);
		if (Context::iioDevHasAttribute(m_dev, attr)) {
			ret = iio_device_attr_write_bool(m_dev, attr.c_str(), value);
			if (ret < 0) {
				errstr = "Can't write attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr +	" attribute";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}
		return getBoolValue(attr);
	}

	bool setBoolValue(unsigned int chn_idx, bool value, string attr, bool output = false)
	{
		int ret = 0;
		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		std::string dev_name = getName();
		std::string errstr, name = "";
		struct iio_channel *chn;

		if (chn_idx >= nb_channels) {
			errstr = dev_name + " has no such channel";
			goto out_cleanup;
		}

		name = "voltage" + std::to_string(chn_idx);
		chn = iio_device_find_channel(m_dev, name.c_str(), output);
		if (Context::iioChannelHasAttribute(chn, attr)) {
			ret = iio_channel_attr_write_bool(chn, attr.c_str(), value);
			if (ret < 0) {
				errstr = "Can't write attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr + " attribute for the selected channel";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return getBoolValue(chn_idx, attr, output);
	}

	string setStringValue(string attr, string value)
	{
		ssize_t ret = 0;
		std::string errstr = "";
		std::string dev_name = iio_device_get_name(m_dev);
		if (Context::iioDevHasAttribute(m_dev, attr)) {
			ret = iio_device_attr_write(m_dev, attr.c_str(), value.c_str());
			if (ret < 0) {
				errstr = "Can't write attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr + " attribute";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return getStringValue(attr);
	}

	string setStringValue(unsigned int chn_idx, string attr, string value, bool output = false)
	{
		std::string errstr = "";
		ssize_t ret = 0;
		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		std::string dev_name = getName();
		std::string name = "";
		struct iio_channel *chn;

		if (chn_idx >= nb_channels) {
			errstr = dev_name + " has no such channel";
			goto out_cleanup;
		}

		name = "voltage" + std::to_string(chn_idx);
		chn = iio_device_find_channel(m_dev, name.c_str(), output);
		if (Context::iioChannelHasAttribute(chn, attr)) {
			ret = iio_channel_attr_write(chn, attr.c_str(), value.c_str());
			if (ret < 0) {
				errstr = "Can't write attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr + " attribute for the selected channel";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return getStringValue(chn_idx, attr, output);
	}

	string getStringValue(string attr)
	{
		ssize_t ret = 0;
		std::string errstr = "";
		char value[100];
		std::string dev_name = getName();

		if (Context::iioDevHasAttribute(m_dev, attr)) {
			ret = iio_device_attr_read(m_dev, attr.c_str(), value, sizeof(value));
			if (ret < 0) {
				errstr = "Can't read attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr + " attribute";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return std::string(value);
	}

	string getStringValue(unsigned int chn_idx, string attr, bool output = false)
	{
		ssize_t ret = 0;
		char value[100];
		unsigned int nb_channels = iio_device_get_channels_count(m_dev);
		std::string dev_name = getName();
		std::string name, errstr = "";
		struct iio_channel *chn;

		if (chn_idx >= nb_channels) {
			errstr = dev_name + " has no such channel";
			goto out_cleanup;
		}

		name = "voltage" + std::to_string(chn_idx);
		chn = iio_device_find_channel(m_dev, name.c_str(), output);
		if (Context::iioChannelHasAttribute(chn, attr)) {
			ret = iio_channel_attr_read(chn, attr.c_str(), value, sizeof(value));
			if (ret < 0) {
				errstr = "Can't read attribute " + attr + " ";
				errstr += std::strerror(-ret);
				goto out_cleanup;
			}
		} else {
			errstr = dev_name + " has no " + attr + " attribute for the selected channel";
			goto out_cleanup;
		}

	out_cleanup:
		if (errstr != "") {
			throw_exception(EXC_INVALID_PARAMETER, errstr, __FILE__, __LINE__);
		}

		return value;
	}

	std::vector<double> getAvailableSampleRates()
	{
		std::vector<double> values = {};
		std::vector<std::string> str_values;
		char buf[1024];
		ssize_t ret;

		ret = iio_device_attr_read(m_dev, "sampling_frequency_available", buf, sizeof(buf));

		if (ret > 0) {
			str_values = Utils::split(buf, " ");

			for (auto it : str_values) {
				values.push_back(std::stod(it));
			}
		}

		if (values.empty()) {
			ret = iio_device_attr_read(m_dev, "sampling_frequency", buf, sizeof(buf));

			if (!ret) {
				__try {
					values.push_back(std::stoul(buf));
				} __catch (exception_type &) {
					throw_exception(EXC_INVALID_PARAMETER, "Device: Not a valid samplerate.",
							__FILE__, __LINE__);
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
					std::string(std::strerror(-ret)), __FILE__, __LINE__);
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
			throw_exception(EXC_OUT_OF_RANGE, "Device: No such channel", __FILE__, __LINE__);
		}
	}

	void convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src)
	{
		if (chn_idx < m_channel_list.size()) {
			m_channel_list.at(chn_idx)->convert(avg, src);
		} else {
			throw_exception(EXC_OUT_OF_RANGE, "Device: No such channel", __FILE__, __LINE__);
		}
	}

	void setKernelBuffersCount(unsigned int count)
	{
		if (!m_dev) {
			throw_exception(EXC_RUNTIME_ERROR, "Device: no such device", __FILE__, __LINE__);
		}
		iio_device_set_kernel_buffers_count(m_dev, count);
	}

	bool isValidDmmChannel(unsigned int chnIdx)
	{
		if (chnIdx >= m_channel_list.size()) {
			throw_exception(EXC_OUT_OF_RANGE, "Device: no such DMM channel", __FILE__, __LINE__);
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
					std::to_string(attrIdx), __FILE__, __LINE__);
		}
		pair.first = std::string(name);
		pair.second = std::string(value);
		return pair;
	}

	void setCyclic(bool enable)
	{
		if (m_buffer) {
			m_buffer->setCyclic(enable);
		}
	}

private:
	struct iio_context *m_context;
	struct iio_device *m_dev;
	std::vector<Channel*> m_channel_list;
	Buffer* m_buffer;
};

