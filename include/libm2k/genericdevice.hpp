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

#ifndef GENERICDEVICE_HPP
#define GENERICDEVICE_HPP

#include "m2kglobal.hpp"
#include <string>
#include <vector>
#include <memory>

extern "C" {
	struct iio_context;
}

namespace libm2k {
namespace analog {
	class GenericAnalogIn;
	class GenericAnalogOut;
	class PowerSupply;
	class DMM;
}

namespace digital {
	class GenericDigital;
}

namespace devices {
class M2K;
class LIBM2K_API GenericDevice {
public:
	enum DEVICE_TYPE {
		ANALOG = 1,
		DIGITAL = 2,
		NONE = 3
	};

	enum DEVICE_DIRECTION {
		INPUT = 1,
		OUTPUT = 2,
		BOTH = 3,
		NO_DIRECTION = 4
	};

	GenericDevice(std::string uri, struct iio_context*, std::string name);
	virtual ~GenericDevice();

	virtual void scanAllAnalogIn();
	virtual void scanAllAnalogOut();
	virtual void scanAllPowerSupply();
	virtual void scanAllDigital();
	void scanAllDMM();
	static GenericDevice* getDevice(std::string uri);

	std::shared_ptr<libm2k::analog::GenericAnalogIn> getAnalogIn(unsigned int);
	std::shared_ptr<libm2k::analog::GenericAnalogIn> getAnalogIn(std::string);
	std::shared_ptr<libm2k::analog::GenericAnalogOut> getAnalogOut(unsigned int);
	std::shared_ptr<libm2k::analog::GenericAnalogOut> getAnalogOut(std::string);
	std::shared_ptr<libm2k::analog::DMM> getDMM(unsigned int);
	std::shared_ptr<libm2k::analog::DMM> getDMM(std::string);

	void blinkLed();
	struct iio_context* ctx();

	libm2k::devices::M2K* toM2k();
protected:
	static std::vector<std::shared_ptr<libm2k::analog::GenericAnalogIn>> s_instancesAnalogIn;
	static std::vector<std::shared_ptr<libm2k::analog::GenericAnalogOut>> s_instancesAnalogOut;
	static std::vector<std::shared_ptr<libm2k::analog::DMM>> s_instancesDMM;
	static std::vector<std::shared_ptr<libm2k::analog::PowerSupply>> s_instancesPowerSupply;
	static std::vector<std::shared_ptr<libm2k::digital::GenericDigital>> s_instancesDigital;
private:
	bool isIioDeviceBufferCapable(std::string);
	GenericDevice::DEVICE_TYPE getIioDeviceType(std::string);
	GenericDevice::DEVICE_DIRECTION getIioDeviceDirection(std::string);
	std::string m_uri;
	struct iio_context* m_ctx;

};
}
}

#endif // GENERICDEVICE_HPP
