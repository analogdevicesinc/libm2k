/*
 * Copyright (c) 2020 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <libm2k/m2kexceptions.hpp>
#include <utility>

using namespace libm2k;

m2k_exception_builder m2k_exception::make(std::string what) {
	return m2k_exception_builder(what);
}

int m2k_exception::iioCode() const noexcept
{
	return m_iio_code;
}

libm2k::M2K_EXCEPTION_TYPE m2k_exception::type() const noexcept
{
	return m_type;
}

int m2k_exception::line() const noexcept
{
	return m_line;
}

std::string m2k_exception::file() const
{
	return m_file;
}

const char* m2k_exception::what() const noexcept
{
	return m_message.c_str();
}

m2k_exception_builder::m2k_exception_builder(std::string &what)
{
	m2KException = m2k_exception(what);
	m2KException.m_error = what;
	m2KException.m_message.append(m2KException.m_error);
}

m2k_exception_builder::m2k_exception_builder(const char* what)
{
	m2KException = m2k_exception(what);
	m2KException.m_error = what;
	m2KException.m_message.append(m2KException.m_error);
}

m2k_exception_builder &m2k_exception_builder::iioCode(int code)
{
	m2KException.m_iio_code = code;
	return *this;
}

m2k_exception_builder &m2k_exception_builder::type(libm2k::M2K_EXCEPTION_TYPE type)
{
	switch (type) {
	case libm2k::EXC_OUT_OF_RANGE: {
		m2KException.m_message = "ERR: Out of range - " + m2KException.m_error;
		break;
	}
	case libm2k::EXC_RUNTIME_ERROR: {
		m2KException.m_message = "ERR: Runtime - " + m2KException.m_error;
		break;
	}
	case libm2k::EXC_INVALID_PARAMETER: {
		m2KException.m_message = "ERR: Invalid argument - " + m2KException.m_error;
		break;
	}
	case libm2k::EXC_TIMEOUT: {
		m2KException.m_message = "ERR: Timeout - " + m2KException.m_error;
		break;
	}
	default: {
		m2KException.m_message = "ERR: Generic - " + m2KException.m_error;
		break;
	}
	}
	m2KException.m_type = type;
	return *this;
}

m2k_exception_builder &m2k_exception_builder::line(int lineNumber)
{
	m2KException.m_line = lineNumber;
	return *this;
}

m2k_exception_builder &m2k_exception_builder::file(const std::string &fileName)
{
	m2KException.m_file = fileName;
	return *this;
}

m2k_exception m2k_exception_builder::build() {
	return m2KException;
}
