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

#ifndef CHANNEL_V1_HPP
#define CHANNEL_V1_HPP

#include <iio/iio.h>
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
	bool isEnabled(iio_channels_mask *mask);
	unsigned int getNbAttributes();
	std::string getAttributeName(unsigned int idx);
	bool hasAttribute(std::string attr);
//	void write(const struct iio_block* block, std::vector<short> const &data);
//	void write(const struct iio_block* block, std::vector<unsigned short> const &data);
//	void write(const struct iio_block* block, std::vector<double> const &data);
//	void write(const struct iio_block* block, double *data, unsigned int nb_samples);
//	void write(const struct iio_block* block, short *data, unsigned int nb_samples);
//	void write(const struct iio_block* block, unsigned short *data, unsigned int nb_samples);
	void write(const iio_block *block, void *data, ssize_t data_type_size, size_t nb_samples, bool raw);

	void convert(int16_t *avg, int16_t *src);
	void convert(double *avg, int16_t *src);

	void setDoubleValue(std::string attr_name, double val);
	double getDoubleValue(std::string attr_name);

	void setStringValue(std::string attr_name, std::string val);
	std::string getStringValue(std::string attr_name);

	void setLongValue(std::string attr_name, long long val);
	long long getLongValue(std::string attr_name);

	void setBoolValue(std::string attr_name, bool val);
	bool getBoolValue(std::string attr_name);

	std::vector<std::string> getAvailableAttributeValues(const std::string &attr);

	void enableChannel(bool enable, struct iio_channels_mask *);
	uintptr_t getFirst(const struct iio_block* block);
	void *getFirstVoid(const struct iio_block *block);

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
#endif //CHANNEL_V1_HPP
