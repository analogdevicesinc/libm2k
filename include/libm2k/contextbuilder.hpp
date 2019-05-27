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
#include <libm2k/m2kglobal.hpp>
#include <libm2k/context.hpp>
#include <unordered_set>
#include <map>

enum DeviceTypes {
	DevFMCOMMS,
	DevM2K,
	Other
};

namespace libm2k {
namespace devices {
class LIBM2K_API ContextBuilder {
	static std::vector<Context*> s_connectedDevices;
public:

	explicit ContextBuilder();
	~ContextBuilder();
	static std::vector<std::string> listDevices();
	static Context* deviceOpen(const char*);
	static Context* deviceOpen();
	static M2k* m2kOpen(const char*);
	static M2k* m2kOpen();
	static void deviceClose(Context*);
	static void deviceCloseAll();
private:
	static std::map<DeviceTypes, std::vector<std::string>> m_dev_map;
	static std::map<DeviceTypes, std::string> m_dev_name_map;
	//                std::shared_ptr<M2KImpl> m_pimpl;
	static DeviceTypes identifyDevice(iio_context *ctx);
	static Context* buildDevice(DeviceTypes type,
					std::string uri,
					struct iio_context *ctx,
					bool sync);
};

LIBM2K_API Context* deviceOpen()
{
	return ContextBuilder::deviceOpen();
}

LIBM2K_API Context* deviceOpen(const char* uri)
{
	return ContextBuilder::deviceOpen(uri);
}

LIBM2K_API M2k* m2kOpen(const char* uri)
{
	return ContextBuilder::m2kOpen(uri);
}

LIBM2K_API M2k* m2kOpen()
{
	return ContextBuilder::m2kOpen();
}

LIBM2K_API std::vector<std::string> listDevices()
{
	return ContextBuilder::listDevices();
}

LIBM2K_API void deviceClose(Context* ctx)
{
	ContextBuilder::deviceClose(ctx);
}

LIBM2K_API void deviceCloseAll()
{
	ContextBuilder::deviceCloseAll();
}
}
}
#endif //M2K_H
