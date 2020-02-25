/*
 * Copyright (c) 2019 Analog Devices Inc.
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

namespace libm2k {
namespace analog {
	class DMM;
}

namespace contexts {
class Lidar;
class M2k;

class LIBM2K_API Context {
public:
	virtual ~Context() {}

	virtual void init() = 0;
	virtual void deinitialize() = 0;

	virtual std::string getUri() = 0;
	virtual libm2k::analog::DMM* getDMM(unsigned int) = 0;
	virtual libm2k::analog::DMM* getDMM(std::string) = 0;
	virtual std::vector<libm2k::analog::DMM*> getAllDmm() = 0;


	virtual std::vector<std::string> getAvailableContextAttributes() = 0;
	virtual std::string getContextAttributeValue(std::string attr) = 0;
	virtual std::string getContextDescription() = 0;
	virtual std::string getSerialNumber() = 0;
	virtual std::unordered_set<std::string> getAllDevices() = 0;

	virtual M2k* toM2k() = 0;

	virtual unsigned int getDmmCount() = 0;
	virtual std::string getFirmwareVersion() = 0;

	/**
	* @brief Set a timeout for I/O operations
	* @param timeout A positive integer representing the time in milliseconds after which a timeout occurs. A value of 0 is used to specify that no timeout should occur.
	*/
	virtual void setTimeout(unsigned int timeout) = 0;

};
}
}

#endif // CONTEXT_HPP
