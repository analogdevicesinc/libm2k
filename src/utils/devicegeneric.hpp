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

#ifndef DEVICEGENERIC_HPP
#define DEVICEGENERIC_HPP
#include <iio.h>
#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include <memory>
#include <libm2k/m2kglobal.hpp>

namespace libm2k {
namespace utils {
class Channel;
class Buffer;

/**
 * The DeviceGeneric class is to be used in interacting with any IIO device (it should express a correspondent
 * to the struct iio_device in underlying libiio. It provides basic functionality such as:
 * - creating and initializing the iio_device and its corresponding channels;
 * - channel handling is done in a generic way, scanning all the input and output channels;
 * - all the mutators and accessors are as generic as possible, while providing a fairly simple usage scenario;
 * these can be used for iio_device related attributes or for channel related attributes; in the latter case,
 * the direction of the channels should be known and provided (input/output);
 * - can be used to manage iio_devices with both input and output channels
 * (can handle any channel type (whether it is voltage in, voltage out, altvoltage, temp, etc.);
 */
class DeviceGeneric
{
public:
	DeviceGeneric(struct iio_context* context, std::string dev_name = "");
	virtual ~DeviceGeneric();

	virtual Channel *getChannel(unsigned int chnIdx, bool output);
	virtual Channel *getChannel(std::string id, bool output);
	virtual bool isChannel(unsigned int chnIdx, bool output);

	virtual void enableChannel(unsigned int chnIdx, bool enable, bool output);
	virtual bool isChannelEnabled(unsigned int chnIdx, bool output);

	virtual std::string getName();
	virtual std::string getId();
	virtual unsigned int getNbAttributes();
	virtual unsigned int getNbBufferAttributes();
	virtual std::string getAttributeName(unsigned int idx);
	virtual std::string getBufferAttributeName(unsigned int idx);
	virtual double getDoubleValue(std::string attr);
	virtual double getDoubleValue(unsigned int, std::string attr, bool output=false);
	virtual double setDoubleValue(double value, std::string attr);
	virtual double setDoubleValue(unsigned int chn_idx, double value, std::string attr, bool output=false);

	virtual int getLongValue(std::string attr);
	virtual int getLongValue(unsigned int, std::string attr, bool output=false);
	virtual int getBufferLongValue(std::string attr);
	virtual int setLongValue(long long value, std::string attr);
	virtual int setLongValue(unsigned int chn_idx, long long value, std::string attr, bool output=false);
	virtual int setBufferLongValue(int value, std::string attr);

	virtual bool getBoolValue(std::string attr);
	virtual bool getBoolValue(unsigned int, std::string attr, bool output=false);
	virtual bool setBoolValue(bool value, std::string attr);
	virtual bool setBoolValue(unsigned int chn_idx, bool value, std::string attr, bool output=false);

	virtual std::string setStringValue(std::string attr, std::string value);
	virtual std::string setStringValue(unsigned int chn, std::string attr,
				   std::string value, bool output=false);
	virtual std::string setBufferStringValue(std::string attr, std::string value);
	virtual	std::string getStringValue(std::string attr);
	virtual std::string getStringValue(unsigned int chn, std::string attr, bool output=false);
	virtual	std::string getBufferStringValue(std::string attr);

	std::vector<std::string> getAvailableAttributeValues(const std::string &attr);
	std::vector<std::string> getAvailableAttributeValues(unsigned int chn_idx, const std::string &attr, bool output=false);

	virtual void writeRegister(uint32_t address, uint32_t value);

	virtual std::string getHardwareRevision();
	virtual unsigned int getNbChannels(bool output);

	virtual void convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src, bool output);
	virtual void convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src, bool output);

	virtual void setKernelBuffersCount(unsigned int count);
	virtual bool isValidDmmChannel(unsigned int chnIdx);

	virtual std::pair<std::string, std::string> getContextAttr(unsigned int attrIdx);

	virtual void setCyclic(bool en);
	virtual bool hasGlobalAttribute(std::string attr);
	virtual bool hasBufferAttribute(std::string attr);

	virtual ssize_t getSampleSize();
	virtual unsigned int getNbSamples() const;
protected:
	struct iio_context *m_context;
	struct iio_device *m_dev;
	std::vector<Channel*> m_channel_list_in;
	std::vector<Channel*> m_channel_list_out;
	Buffer* m_buffer;
	const char *m_dev_name;
};
}
}


#endif //DEVICEGENERIC_HPP
