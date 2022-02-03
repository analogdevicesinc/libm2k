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

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iio.h>
#include <vector>
#include <string>
#include <memory>
#include <libm2k/m2kglobal.hpp>

namespace libm2k {
namespace utils {
class Channel
{
public:
	Channel(struct iio_device* device, unsigned int channel = 0);
	Channel(struct iio_device* device, std::string channel_name, bool output);
	virtual ~Channel();

	std::string getName();
	std::string getId();
	unsigned int getIndex();
	bool isOutput();
	bool isEnabled();
	unsigned int getNbAttributes();
	std::string getAttributeName(unsigned int idx);
	bool hasAttribute(std::string attr);
	void write(struct iio_buffer* buffer, std::vector<short> const &data);
	void write(struct iio_buffer* buffer, std::vector<unsigned short> const &data);
	void write(struct iio_buffer* buffer, std::vector<double> const &data);
	void write(struct iio_buffer* buffer, double *data, unsigned int nb_samples);
	void write(struct iio_buffer* buffer, short *data, unsigned int nb_samples);
	void write(struct iio_buffer* buffer, unsigned short *data, unsigned int nb_samples);

	void convert(int16_t *avg, int16_t *src);
	void convert(double *avg, int16_t *src);

	void setDoubleValue(std::string attr, double val);
	double getDoubleValue(std::string attr);

	void setStringValue(std::string attr, std::string val);
	std::string getStringValue(std::string attr);

	void setLongValue(std::string attr, long long val);
	long long getLongValue(std::string attr);

	void setBoolValue(std::string attr, bool val);
	bool getBoolValue(std::string attr);

	std::vector<std::string> getAvailableAttributeValues(const std::string &attr);

	void enableChannel(bool enable);
	uintptr_t getFirst(struct iio_buffer* buffer);
	void *getFirstVoid(iio_buffer *buffer);

	bool isValid();
	struct iio_channel* getChannel();
	iio_device* getDevice();
private:
	struct iio_device *m_device;
	struct iio_channel *m_channel;

	const char *m_dev_name;
	const char *m_channel_id;

};
}
}
#endif //CHANNEL_HPP
