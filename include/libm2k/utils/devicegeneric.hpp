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

class DeviceGeneric
{
public:
	DeviceGeneric(struct iio_context* context, std::string dev_name = "", bool input = false);
	virtual ~DeviceGeneric();

	virtual Channel *getChannel(unsigned int chnIdx);
	virtual Channel *getChannel(std::string id);
	virtual bool isChannel(unsigned int chnIdx, bool output);

	virtual void enableChannel(unsigned int chnIdx, bool enable);
	virtual bool isChannelEnabled(unsigned int chnIdx);

	virtual std::string getName();
	virtual double getDoubleValue(std::string attr);
	virtual double getDoubleValue(unsigned int, std::string attr, bool output=false);
	virtual double setDoubleValue(double value, std::string attr);
	virtual double setDoubleValue(unsigned int chn_idx, double value, std::string attr, bool output=false);

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
	virtual unsigned int getNbChannels();

	virtual void convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src);
	virtual void convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src);

	virtual void setKernelBuffersCount(unsigned int count);
	virtual bool isValidDmmChannel(unsigned int chnIdx);

	virtual std::pair<std::string, std::string> getContextAttr(unsigned int attrIdx);

private:
	class DeviceGenericImpl;
	std::unique_ptr<DeviceGenericImpl> m_pimpl;
	DeviceGeneric(DeviceGenericImpl*);
};
}
}


#endif //DEVICE_HPP
