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

#include <libm2k/utils/channel.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/utils/utils.hpp>

using namespace libm2k::utils;

class Channel::ChannelImpl {
public:
	ChannelImpl(iio_device *device, unsigned int channel = 0) {
		m_device = device;
		if (m_device) {
			m_channel = iio_device_get_channel(m_device, channel);
		}

		if (!m_channel) {
			m_channel = nullptr;
		}
	}

	ChannelImpl(iio_device *device, std::string channel_name, bool output)
	{
		m_device = device;
		if (m_device) {
			m_channel = iio_device_find_channel(device, channel_name.c_str(), output);
		}

		if (!m_channel) {
			m_channel = nullptr;

		}

	}

	virtual ~ChannelImpl() {
	}

	std::string getName()
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		std::string name = "";
		auto n = iio_channel_get_name(m_channel);
		if (n) {
			name = std::string(n);
		}
		return name;
	}

	std::string getId()
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		return iio_channel_get_id(m_channel);
	}

	unsigned int getIndex()
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		return iio_channel_get_index(m_channel);
	}

	bool isOutput()
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		return iio_channel_is_output(m_channel);
	}

	bool isEnabled()
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		return iio_channel_is_enabled(m_channel);
	}

	bool hasAttribute(std::string attr)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		if (iio_channel_find_attr(m_channel, attr.c_str()) != NULL) {
			return true;
		}
		return false;
	}

	void enableChannel(bool enable)
	{
		if (!m_channel) {
			throw_exception(EXC_OUT_OF_RANGE, "Channel: Cannot find associated channel");
		}

		if (enable) {
			iio_channel_enable(m_channel);
		} else {
			iio_channel_disable(m_channel);
		}
	}

	uintptr_t getFirst(iio_buffer *buffer)
	{
		return (uintptr_t)iio_buffer_first(buffer, m_channel);
	}

	void write(struct iio_buffer* buffer, std::vector<short> const &data)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}

		size_t size = data.size();
		size_t ret = iio_channel_write(m_channel, buffer, data.data(),
					       size * sizeof(short));

		if (ret == 0) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot write; result is 0 bytes");
		}
	}

	void write(struct iio_buffer* buffer, std::vector<unsigned short> const &data)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}

		size_t size = data.size();
		size_t ret = iio_channel_write(m_channel, buffer, data.data(),
                                           size * sizeof(unsigned short));

		if (ret == 0) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot write; result is 0 bytes");
		}
	}

	void write(struct iio_buffer* buffer, std::vector<double> const &data)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}

		size_t size = data.size();
		size_t ret = iio_channel_write(m_channel, buffer, data.data(),
					       size * sizeof(double));

		if (ret == 0) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot write; result is 0 bytes");
		}
	}

	void write(struct iio_buffer* buffer, double *data, unsigned int nb_samples)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Can not find associated channel");
		}

		size_t ret = iio_channel_write(m_channel, buffer, data,
					       nb_samples * sizeof(double));

		if (ret == 0) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: could not write; result is 0 bytes");
		}
	}

	void write(struct iio_buffer* buffer, short *data, unsigned int nb_samples)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Can not find associated channel");
		}

		size_t ret = iio_channel_write(m_channel, buffer, data,
					       nb_samples * sizeof(short));

		if (ret == 0) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: could not write; result is 0 bytes");
		}
	}

	void write(struct iio_buffer* buffer, unsigned short *data, unsigned int nb_samples)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Can not find associated channel");
		}

		size_t ret = iio_channel_write(m_channel, buffer, data,
					       nb_samples * sizeof(unsigned short));

		if (ret == 0) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: could not write; result is 0 bytes");
		}
	}

	void convert(int16_t *avg, int16_t *src)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		iio_channel_convert(m_channel, (void *)avg, (const void *)src);
	}

	void convert(double *avg, int16_t *src)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		iio_channel_convert(m_channel, (void *)avg, (const void *)src);
	}

	double getDoubleValue(std::string attr)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		double value = 0.0;
		int ret = iio_channel_attr_read_double(m_channel, attr.c_str(), &value);
		if (ret < 0) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot read " + attr);
		}
		return value;
	}

	void setDoubleValue(std::string attr, double val)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		int ret = iio_channel_attr_write_double(m_channel, attr.c_str(), val);
		if (ret < 0) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot write " + attr);
		}
	}

	void setLongValue(std::string attr, long long val)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		int ret = iio_channel_attr_write_longlong(m_channel, attr.c_str(), val);
		if (ret < 0) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot write " + attr);
		}
	}

	void setStringValue(std::string attr, std::string val)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		int ret = iio_channel_attr_write(m_channel, attr.c_str(), val.c_str());
		if (ret < 0) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot write " + attr);
		}
	}

	std::string getStringValue(std::string attr)
	{
		if (!m_channel) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot find associated channel");
		}
		char value[1024];
		int ret = iio_channel_attr_read(m_channel, attr.c_str(), value, sizeof(value));
		if (ret < 0) {
			throw_exception(EXC_INVALID_PARAMETER, "Channel: Cannot write " + attr);
		}
		return std::string(value);
	}

	bool isValid()
	{
		return !!m_channel;
	}

private:
	struct iio_device *m_device;
	struct iio_channel *m_channel;
};
