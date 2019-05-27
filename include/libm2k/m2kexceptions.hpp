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

#include <libm2k/m2kglobal.hpp>
#include <libm2k/logger.hpp>
#include <libm2k/enums.hpp>
#include <stdexcept>
#include <string>
#include <iostream>

#if _EXCEPTIONS || defined(__cpp_exceptions)
	#define exception_type std::exception
	#if defined(_MSC_VER)
		#define __try      try
		#define __catch(X) catch(X)
	#endif
#else
	#define exception_type
	#if defined(_MSC_VER)
		#define __try      if (true)
		#define __catch(X) if (false)
	#endif
#endif

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

static std::exception e;

static void throw_exception(libm2k::M2K_EXCEPTION exc_type, std::string exception)
{
#if _EXCEPTIONS || defined(__cpp_exceptions)
	switch (exc_type) {
	case libm2k::EXC_OUT_OF_RANGE: {
		throw std::out_of_range("ERR: Out of range - " + exception);
	}
	case libm2k::EXC_RUNTIME_ERROR: {
		throw std::runtime_error("ERR: Runtime - " + exception);
	}
	case libm2k::EXC_INVALID_PARAMETER: {
		throw std::invalid_argument("ERR: Invalid argument - " + exception);
	}
	default: {
		throw std::runtime_error("ERR: Generic - " + exception);
	}
	}
#else
	LOG("exception");

	std::cout << "Exception: " << exception << std::endl;
#endif
}



#endif // M2KEXCEPTIONS_H
