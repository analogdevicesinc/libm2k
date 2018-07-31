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

#ifdef _WIN32
#   ifdef LIBM2K_EXPORTS
#	define __api __declspec(dllexport)
#   else
#	define __api __declspec(dllimport)
#   endif
#endif

#include <vector>
#include <string>
//#include <memory>

namespace m2k {
//        class DeviceBuilderImpl;
        /* @class DeviceBuilder
         * handles a device
         */
        class __api DeviceBuilder {
        public:
                explicit DeviceBuilder();
                ~DeviceBuilder();
                static __api std::vector<std::string> listDevices();
                static __api DeviceBuilder* deviceOpen(const char*); //should ret GenericDevice
                static __api void deviceClose(DeviceBuilder*); //should return GenericDevice
        private:
                std::vector<std::string> m_connectedDevices; //should be GenericDevice
//                std::unique_ptr<M2KImpl> m_pimpl;
        };
}
//#endif
