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

#include <libm2k/utils/devicein.hpp>
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

class DeviceIn::DeviceInImpl {
public:

	/*
	 * Represents an iio_device
	 */
	DeviceInImpl(struct iio_context* context, std::string dev_name = "")
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

			unsigned int nb_channels = iio_device_get_channels_count(m_dev);
			for (unsigned int i = 0; i < nb_channels; i++) {
				Channel *chn = nullptr;

				chn = new Channel(m_dev, i);
				if (!chn->isValid()) {
					delete chn;
					chn = nullptr;
					continue;
				}

				if (!chn->isOutput()) {
					m_channel_list.push_back(chn);
				} else {
					delete chn;
					chn = nullptr;
					continue;
				}
			}

			std::sort(m_channel_list.begin(), m_channel_list.end(), [](Channel* lchn, Channel* rchn)
			{
				return Utils::compareNatural(lchn->getId(), rchn->getId());
			});
		}
	}

	virtual ~DeviceInImpl()
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

	std::vector<unsigned short> getSamples(unsigned int nb_samples)
	{
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: Cannot refill; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		return m_buffer->getSamples(nb_samples);

	}

	const unsigned short* getSamplesP(unsigned int nb_samples)
	{
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: Can not refill; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		return m_buffer->getSamplesP(nb_samples);

	}

	std::vector<std::vector<double> > getSamples(unsigned int nb_samples,
					std::function<double(int16_t, unsigned int)> process)
	{
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: Cannot refill; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		return m_buffer->getSamples(nb_samples, process);
	}

	void* getSamplesRawInterleavedVoid(unsigned int nb_samples)
	{
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: Can not refill; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		return m_buffer->getSamplesRawInterleavedVoid(nb_samples);
	}

	const double *getSamplesInterleaved(unsigned int nb_samples,
					std::function<double(int16_t, unsigned int)> process)
	{
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: Can not refill; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		return m_buffer->getSamplesInterleaved(nb_samples, process);
	}

	const short *getSamplesRawInterleaved(unsigned int nb_samples)
	{
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: Can not refill; device not buffer capable");
		}
		m_buffer->setChannels(m_channel_list);
		return m_buffer->getSamplesRawInterleaved(nb_samples);
	}

	void cancelBuffer()
	{
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: not buffer capable");
		}
		m_buffer->cancelBuffer();
	}

	void flushBuffer()
	{
		if (!m_buffer) {
			throw_exception(EXC_INVALID_PARAMETER, "Device: Can not refill; device not buffer capable");
		}
		m_buffer->flushBuffer();
	}

	struct IIO_OBJECTS getIioObjects()
	{
		IIO_OBJECTS iio_object = {};
		iio_object.buffers_rx.push_back(m_buffer->getBuffer());

		for (auto chn : m_channel_list) {
			iio_object.channels_in.push_back(chn->getChannel());
		}
		iio_object.devices.push_back(m_dev);
		iio_object.context = m_context;
		return iio_object;
	}

private:
	struct iio_context *m_context;
	struct iio_device *m_dev;
	std::vector<Channel*> m_channel_list;
	Buffer* m_buffer;
};

