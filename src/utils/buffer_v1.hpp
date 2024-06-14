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

#ifndef BUFFER_V1_HPP
#define BUFFER_V1_HPP
#include <iio/iio.h>

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <libm2k/m2kglobal.hpp>

namespace libm2k {
namespace utils {
class Channel;

class Buffer
{
public:
	Buffer(struct iio_device *dev, struct iio_channels_mask *mask);
	~Buffer();

	void initializeBuffer(unsigned int size, bool cyclic, bool output, bool enableFlag = false);

	void pushGeneric(void *data, ssize_t data_type_size, size_t data_size, unsigned int channel,
		  bool cyclic, bool multiplex, bool enableFlag);
	void push(std::vector<short> const &data, unsigned int channel = 0,
		bool cyclic = true, bool multiplex = false, bool enableFlag = false);
	void push(std::vector<unsigned short> const &data, unsigned int channel = 0,
		bool cyclic = true, bool multiplex = false, bool enableFlag = false);
	void push(unsigned short *data, unsigned int channel, unsigned int nb_samples,
		  bool cyclic = true, bool multiplex = false, bool enableFlag = false);
	void push(std::vector<double> const &data, unsigned int channel = 0, bool cyclic = true, bool enableFlag = false);
	void push(std::vector<std::vector<short>> const &data);

	void push(double *data, unsigned int channel, unsigned int nb_samples, bool cyclic = true, bool enableFlag = false);
	void push(short *data, unsigned int channel, unsigned int nb_samples, bool cyclic = true, bool enableFlag = false);

	void setChannels(std::vector<Channel*> channels);
	std::vector<unsigned short> getSamples(unsigned int nb_samples);
	const unsigned short* getSamplesP(unsigned int nb_samples);

	std::vector<std::vector<double>> getSamples(unsigned int nb_samples,
					const std::function<double(int16_t, unsigned int)> &process);
	const double *getSamplesInterleaved(unsigned int nb_samples,
					const std::function<double(int16_t, unsigned int)> &process);
	const short *getSamplesRawInterleaved(unsigned int nb_samples);
	void* getSamplesRawInterleavedVoid(unsigned int nb_samples);

	void getSamples(std::vector<std::vector<double>> &data, unsigned int nb_samples,
					const std::function<double(int16_t, unsigned int)> &process);
	void getSamples(std::vector<unsigned short> &data, unsigned int nb_samples);

	void setKernelBuffersCount(unsigned int count);
	unsigned int getKernelBuffersCount();

	void stop();
	void setCyclic(bool enable);
	void cancelBuffer();
	void flushBuffer();
	unsigned int getNbSamples() const;

	unsigned int getNbAttributes();
	std::string getAttributeName(unsigned int idx);
	void setLongValue(int value, std::string attr_name);
	int getLongValue(std::string attr_name);
	struct iio_buffer* getBuffer();
	bool hasBufferAttribute(std::string attr_name);
	void setStringValue(std::string attr_name, std::string value);
	std::string getStringValue(std::string attr_name);
	struct iio_block *getBlock();
private:
	struct iio_device* m_dev;
	struct iio_buffer* m_buffer;
	struct iio_channels_mask *m_mask;
	struct iio_stream *m_stream;
	struct iio_block *m_block;
	bool m_block_enqueued;
	const char *m_dev_name;
	unsigned int m_last_nb_samples;
	unsigned int m_nb_kernel_buffers;
	bool m_cyclic;
	std::vector<Channel*> m_channel_list;
	std::vector<std::vector<double>> m_data;
	std::vector<unsigned short> m_data_short;

	void destroy();
	void destroyBuffer();
};
}
}

#endif //BUFFER_V1_HPP
