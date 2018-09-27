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

#ifndef M2K_H
#define M2K_H

#include <vector>
#include <string>
#include <iio.h>
#include "m2kglobal.h"
#include "analogin.h"

namespace m2k {
	class GenericDevice;
	class LIBM2K_API DeviceBuilder {
		static std::vector<GenericDevice*> s_connectedDevices; //should be GenericDevice
        public:
                explicit DeviceBuilder();
                ~DeviceBuilder();
		static std::vector<std::string> listDevices();
		static GenericDevice* deviceOpen(const char*); //should ret GenericDevice
		static void deviceClose(GenericDevice*); //should return GenericDevice
        private:
//                std::unique_ptr<M2KImpl> m_pimpl;
	};

	class LIBM2K_API GenericDevice {
		static std::vector<AnalogIn*> s_instancesAnalogIn;
	public:
		GenericDevice(std::string);
		~GenericDevice();
		AnalogIn* analogInOpen();
		void analogInClose(AnalogIn*);
		AnalogIn* getAnalogInInstance(std::string);
		void blinkLed();
	private:
		std::string m_uri;
		iio_context* m_ctx;
	};
}
#endif //M2K_H
