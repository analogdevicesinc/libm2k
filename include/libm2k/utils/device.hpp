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

#ifndef DEVICE_HPP
#define DEVICE_HPP
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

class LIBM2K_API Device
{
public:
	class DeviceImpl;

	Device(struct iio_context* context, std::string dev_name = "", bool input = false);
	virtual ~Device();

	Channel *getChannel(unsigned int chnIdx);
	Channel *getChannel(std::string id);
	bool isChannel(unsigned int chnIdx, bool output);

	virtual void enableChannel(unsigned int chnIdx, bool enable);
	bool isChannelEnabled(unsigned int chnIdx);

	void push(std::vector<short> const &data, unsigned int channel,
		  bool cyclic = true, bool multiplex = false);
	void push(std::vector<double> const &data, unsigned int channel, bool cyclic = true);
	void stop();

	std::vector<unsigned short> getSamples(unsigned int nb_samples);
	std::vector<std::vector<double> > getSamples(unsigned int nb_samples,
					std::function<double (int16_t, unsigned int)> process);

	std::string getName();
	double getDoubleValue(std::string attr);
	double getDoubleValue(unsigned int, std::string attr, bool output=false);
	double setDoubleValue(double value, std::string attr);
	double setDoubleValue(unsigned int chn_idx, double value, std::string attr, bool output=false);

	bool getBoolValue(std::string attr);
	bool getBoolValue(unsigned int, std::string attr, bool output=false);
	bool setBoolValue(bool value, std::string attr);
	bool setBoolValue(unsigned int chn_idx, bool value, std::string attr, bool output=false);

	std::string setStringValue(std::string attr, std::string value);
	std::string setStringValue(unsigned int chn, std::string attr,
				   std::string value, bool output=false);
	std::string getStringValue(std::string attr);
	std::string getStringValue(unsigned int chn, std::string attr, bool output=false);

	std::vector<double> getAvailableSampleRates();

	void writeRegister(uint32_t address, uint32_t value);

	std::string getHardwareRevision();
	unsigned int getNbChannels();

	void convertChannelHostFormat(unsigned int chn_idx, int16_t *avg, int16_t *src);
	void convertChannelHostFormat(unsigned int chn_idx, double *avg, int16_t *src);

	void setKernelBuffersCount(unsigned int count);
	bool isValidDmmChannel(unsigned int chnIdx);

	std::pair<std::string, std::string> getContextAttr(unsigned int attrIdx);

protected:
	std::shared_ptr<DeviceImpl> m_pimpl;
	Device(DeviceImpl*);
};
}
}


#endif //DEVICE_HPP
