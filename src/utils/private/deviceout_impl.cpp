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

#include <libm2k/utils/deviceout.hpp>
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

class DeviceOut::DeviceOutImpl {
public:

	/*
	 * Represents an iio_device
	 */
	DeviceOutImpl(struct iio_context* context, std::string dev_name = "", bool input = false)
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

	virtual ~DeviceOutImpl()
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

	void push(std::vector<short> const &data, unsigned int channel,
		bool cyclic = true, bool multiplex = false)
	{
		if (!m_buffer) {
			throw_exception(EXC_RUNTIME_ERROR, "Device: Cannot push; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		m_buffer->push(data, channel, cyclic, multiplex);
	}

	void push(std::vector<unsigned short> const &data, unsigned int channel,
			  bool cyclic = true, bool multiplex = false)
	{
		if (!m_buffer) {
			throw_exception(EXC_RUNTIME_ERROR, "Device: Cannot push; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		m_buffer->push(data, channel, cyclic, multiplex);
	}


	void push(unsigned short *data, unsigned int channel, unsigned int nb_samples,
			  bool cyclic, bool multiplex)
	{
		if (!m_buffer) {
			throw_exception(EXC_RUNTIME_ERROR, "Device: Can not push; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		m_buffer->push(data, channel, nb_samples, cyclic, multiplex);
	}

	void push(std::vector<double> const &data, unsigned int channel, bool cyclic = true)
	{
		if (!m_buffer) {
			throw_exception(EXC_RUNTIME_ERROR, "Device: Cannot push; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		m_buffer->push(data, channel, cyclic);
	}

	void push(double *data, unsigned int channel, unsigned int nb_samples, bool cyclic)
	{
		if (!m_buffer) {
			throw_exception(EXC_RUNTIME_ERROR, "Device: Can not push; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		m_buffer->push(data, channel, nb_samples, cyclic);
	}

	void push(short *data, unsigned int channel, unsigned int nb_samples, bool cyclic)
	{
		if (!m_buffer) {
			throw_exception(EXC_RUNTIME_ERROR, "Device: Can not push; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		m_buffer->push(data, channel, nb_samples, cyclic);
	}

	void stop()
	{
		if (m_buffer) {
			m_buffer->stop();
		}
	}

private:
	struct iio_context *m_context;
	struct iio_device *m_dev;
	std::vector<Channel*> m_channel_list;
	Buffer* m_buffer;
};

