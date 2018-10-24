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

#ifndef M2KEXCEPTIONS_H
#define M2KEXCEPTIONS_H

#include "m2kglobal.hpp"

#include <stdexcept>
#include <string>

class LIBM2K_API no_device_exception: public std::runtime_error {
public:
	explicit no_device_exception(const std::string& what) :
		runtime_error(what) {}
	explicit no_device_exception(const char* what) :
		runtime_error(what) {}
	~no_device_exception() {}
};

class LIBM2K_API instrument_already_in_use_exception : public std::runtime_error {
public:
	explicit instrument_already_in_use_exception(const std::string& what) :
		runtime_error(what) {}
	explicit instrument_already_in_use_exception(const char* what) :
		runtime_error(what) {}
	~instrument_already_in_use_exception() {}
};

class LIBM2K_API invalid_parameter_exception: public std::runtime_error {
public:
	explicit invalid_parameter_exception(const std::string& what) :
		runtime_error(what) {}
	explicit invalid_parameter_exception(const char* what) :
		runtime_error(what) {}
	~invalid_parameter_exception() {}
};

class LIBM2K_API timeout_exception: public std::runtime_error {
public:
	explicit timeout_exception(const std::string& what) :
		runtime_error(what) {}
	explicit timeout_exception(const char* what) :
		runtime_error(what) {}
	~timeout_exception() {}
};



#endif // M2KEXCEPTIONS_H
