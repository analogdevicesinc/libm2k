/*
 * Copyright 2018 Analog Devices, Inc.
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

#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/utils/enums.hpp>
#include <libm2k/utils/utils.hpp>
#include <string>
#include <vector>
#include <memory>

extern "C" {
	struct iio_context;
}

using namespace libm2k::utils;
namespace libm2k {
namespace analog {
	class GenericAnalogIn;
	class GenericAnalogOut;
	class DMM;
	class PowerSupply;
}

namespace digital {
	class GenericDigital;
}

namespace devices {
class M2K;

class LIBM2K_API Context {
public:
	Context(std::string uri, struct iio_context*, std::string name);
	virtual ~Context();

	std::vector<std::string> scanAllAnalogIn();
	std::vector<std::string> scanAllAnalogOut();
	virtual void scanAllPowerSupply();
	virtual void scanAllDigital();
	virtual void scanAllDMM();

	std::string getUri();
	std::string getName();

	libm2k::analog::GenericAnalogIn* getAnalogIn(unsigned int);
	libm2k::analog::GenericAnalogIn* getAnalogIn(std::string);
	libm2k::analog::GenericAnalogOut* getAnalogOut(unsigned int);
	libm2k::analog::GenericAnalogOut* getAnalogOut(std::string);
	libm2k::analog::DMM* getDMM(unsigned int);
	libm2k::analog::DMM* getDMM(std::string);
	std::vector<libm2k::analog::DMM*> getAllDmm();

	std::string getContextAttributes();
	std::unordered_set<std::string> getAllDevices(iio_context *ctx);

	libm2k::devices::M2K* toM2k();

	static bool iioChannelHasAttribute(iio_channel *chn, const std::string &attr);
	static bool iioDevHasAttribute(iio_device *dev, const std::string &attr);
protected:
	std::vector<libm2k::analog::GenericAnalogIn*> m_instancesAnalogIn;
	std::vector<libm2k::analog::GenericAnalogOut*> m_instancesAnalogOut;
	std::vector<libm2k::analog::DMM*> m_instancesDMM;
	std::vector<libm2k::analog::PowerSupply*> m_instancesPowerSupply;
	std::vector<libm2k::digital::GenericDigital*> m_instancesDigital;

	std::vector<std::pair<std::string, std::string>> getIioDevByChannelAttrs(std::vector<std::string> attr_list);
	bool isIioDeviceBufferCapable(std::string dev_name);
	DEVICE_TYPE getIioDeviceType(std::string dev_name);
	DEVICE_DIRECTION getIioDeviceDirection(std::string dev_name);

	std::string m_uri;
	std::string m_name;
	struct iio_context* m_context;

};
}
}

#endif // CONTEXT_HPP
