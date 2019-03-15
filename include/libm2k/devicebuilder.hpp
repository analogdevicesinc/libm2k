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

#ifndef DEVICEBUILDER_HPP
#define DEVICEBUILDER_HPP

#include <vector>
#include <string>
#include "m2kglobal.hpp"
#include "genericdevice.hpp"
#include <unordered_set>
#include <map>

extern "C" {
	struct iio_context;
}

enum DeviceTypes {
	DevFMCOMMS,
	DevM2K,
	Other
};

namespace libm2k {
namespace devices {
class LIBM2K_API DeviceBuilder {
	static std::vector<std::shared_ptr<GenericDevice>> s_connectedDevices; //should be GenericDevice
public:

	explicit DeviceBuilder();
	~DeviceBuilder();
	static std::vector<std::string> listDevices();
	static std::shared_ptr<GenericDevice> deviceOpen(const char*);
	static std::shared_ptr<GenericDevice> deviceOpen();
	static void deviceClose(std::shared_ptr<GenericDevice>);
	static DeviceTypes identifyDevice(iio_context *ctx);
	static std::shared_ptr<GenericDevice> buildDevice(DeviceTypes type,
							  std::string uri,
							  struct iio_context *ctx);
private:
	static std::map<DeviceTypes, std::vector<std::string>> m_dev_map;
	static std::map<DeviceTypes, std::string> m_dev_name_map;
	//                std::unique_ptr<M2KImpl> m_pimpl;
};
}
}
#endif //M2K_H
