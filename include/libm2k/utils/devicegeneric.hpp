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
	virtual double getDoubleValue(std::string attr);
	virtual double getDoubleValue(unsigned int, std::string attr, bool output=false);
	virtual double setDoubleValue(double value, std::string attr);
	virtual double setDoubleValue(unsigned int chn_idx, double value, std::string attr, bool output=false);

	virtual int getLongValue(std::string attr);
	virtual int getLongValue(unsigned int, std::string attr, bool output=false);
	virtual int setLongValue(int value, std::string attr);
	virtual int setLongValue(unsigned int chn_idx, int value, std::string attr, bool output=false);

	virtual bool getBoolValue(std::string attr);
	virtual bool getBoolValue(unsigned int, std::string attr, bool output=false);
	virtual bool setBoolValue(bool value, std::string attr);
	virtual bool setBoolValue(unsigned int chn_idx, bool value, std::string attr, bool output=false);

	virtual std::string setStringValue(std::string attr, std::string value);
	virtual std::string setStringValue(unsigned int chn, std::string attr,
				   std::string value, bool output=false);
	virtual	std::string getStringValue(std::string attr);
	virtual std::string getStringValue(unsigned int chn, std::string attr, bool output=false);

	virtual std::vector<double> getAvailableSampleRates();

	virtual void writeRegister(uint32_t address, uint32_t value);

	virtual std::string getHardwareRevision();
	virtual unsigned int getNbChannels(bool output);

	virtual void convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src, bool output);
	virtual void convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src, bool output);

	virtual void setKernelBuffersCount(unsigned int count);
	virtual bool isValidDmmChannel(unsigned int chnIdx);

	virtual std::pair<std::string, std::string> getContextAttr(unsigned int attrIdx);

	virtual void setCyclic(bool en);

private:
	class DeviceGenericImpl;
	std::unique_ptr<DeviceGenericImpl> m_pimpl;
	DeviceGeneric(DeviceGenericImpl*);
};
}
}


#endif //DEVICE_HPP
