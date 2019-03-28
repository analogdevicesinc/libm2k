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

#include <libm2k/m2kglobal.hpp>
#include <libm2k/utils/enums.hpp>
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
	class PowerSupply;
	class DMM;
}

namespace digital {
	class GenericDigital;
}

namespace devices {
class M2K;
class LIBM2K_API GenericDevice : public std::enable_shared_from_this<GenericDevice> {
public:
	GenericDevice(std::string uri, struct iio_context*, std::string name);
	virtual ~GenericDevice();

	virtual void scanAllAnalogIn();
	virtual void scanAllAnalogOut();
	virtual void scanAllPowerSupply();
	virtual void scanAllDigital();
	void scanAllDMM();
	std::string getUri();

	libm2k::analog::GenericAnalogIn* getAnalogIn(unsigned int);
	libm2k::analog::GenericAnalogIn* getAnalogIn(std::string);
	libm2k::analog::GenericAnalogOut* getAnalogOut(unsigned int);
	libm2k::analog::GenericAnalogOut* getAnalogOut(std::string);
	libm2k::analog::DMM* getDMM(unsigned int);
	libm2k::analog::DMM* getDMM(std::string);
	std::vector<libm2k::analog::DMM*> getAllDmm();

	void blinkLed();
	struct iio_context* ctx();
	std::string getContextAttributes();

	libm2k::devices::M2K* toM2k();
protected:
	std::vector<libm2k::analog::GenericAnalogIn*> m_instancesAnalogIn;
	std::vector<libm2k::analog::GenericAnalogOut*> m_instancesAnalogOut;
	std::vector<libm2k::analog::DMM*> m_instancesDMM;
	std::vector<libm2k::analog::PowerSupply*> m_instancesPowerSupply;
	std::vector<libm2k::digital::GenericDigital*> m_instancesDigital;
private:
	bool isIioDeviceBufferCapable(std::string);
	DEVICE_TYPE getIioDeviceType(std::string);
	DEVICE_DIRECTION getIioDeviceDirection(std::string);
	std::string m_uri;
	struct iio_context* m_ctx;

};
}
}

#endif // GENERICDEVICE_HPP
