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
#include <libm2k/enums.hpp>
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

namespace context {
class M2k;
class Generic;

/** @defgroup context Context
 * @brief Contains the representation of a generic IIO context
 * @{
 * @class Context
 * @brief Controls the IIO context
 */
class LIBM2K_API Context {
public:
	/**
	 * @private
	 */
	virtual ~Context() {}


	/**
	 * @private
	 */
	virtual void reset() = 0;


	/**
	 * @private
	 */
	virtual void deinitialize() = 0;


	/**
	 * @brief Get the URI of the current context
	 * @return string containt the context URI (ex: "usb:1.2.3", "ip:192.168.2.1")
	 */
	virtual std::string getUri() = 0;


	/**
	 * @brief Retrieve the DMM object at a certain index
	 * @param index The index corresponding to the DMM
	 * @return On success, a pointer to a DMM object
	 * @return On error, nullptr is returned
	 */
	virtual libm2k::analog::DMM* getDMM(unsigned int index) = 0;


	/**
	 * @brief Retrieve the DMM object which has the provided name
	 * @param name The name corresponding to the DMM
	 * @return On success, a pointer to a DMM object
	 * @return On error, nullptr is returned
	 */
	virtual libm2k::analog::DMM* getDMM(std::string name) = 0;


	/**
	 * @brief Retrieve all the DMM objects
	 * @return A list of DMM objects\n
	 */
	virtual std::vector<libm2k::analog::DMM*> getAllDmm() = 0;


	/**
	 * @brief Retrieve all the available context attributes
	 * @return A list of strings containg all the context attributes
	 */
	virtual std::vector<std::string> getAvailableContextAttributes() = 0;


	/**
	 * @brief Retrieve the value of one context attribute
	 * @param attr The name of the required attribute
	 * @return On success, return a string containing the value of the attribute
	 * @throw On error, throw an exception if the attribute is not found
	 */
	virtual std::string getContextAttributeValue(std::string attr) = 0;


	/**
	 * @brief Retrieve a detailed context description
	 * @return string containing the current context description
	 */
	virtual std::string getContextDescription() = 0;


	/**
	 * @brief Retrieve the current context serial number
	 * @return string containing the current context serial number
	 */
	virtual std::string getSerialNumber() = 0;


	/**
	 * @brief Retrieve all the available devices
	 * @return a list of strings containing all the devices
	 */
	virtual std::unordered_set<std::string> getAllDevices() const = 0;


	/**
	 * @brief Log all the attributes
	 * @note The logging system should be enabled
	 */
	virtual void logAllAttributes() const = 0;


	/**
	 * @brief Convert the current context to M2k context, if possible
	 * @return On success, return the corresponding M2k object
	 * @return On error, return a nullptr
	 */
	virtual M2k* toM2k() = 0;


	/**
	 * @brief Convert the current context to Generic context, if possible
	 * @return On success, return the corresponding Generic object
	 * @return On error, return a nullptr
	 */
	virtual Generic* toGeneric() = 0;


	/**
	 * @brief Retrieve the number of DMM objects
	 * @return an unsigned int representing the number of DMM objects in the context
	 */
	virtual unsigned int getDmmCount() = 0;


	/**
	 * @brief Retrieve the firmware version of the current context
	 * @return a string containing the firmware version
	 */
	virtual std::string getFirmwareVersion() = 0;


	/**
	 * Get the version of the backend in use
	 * @return IIO_CONTEXT_VERSION struct
	 */
	virtual const struct libm2k::IIO_CONTEXT_VERSION getIioContextVersion() = 0;


	/**
	 * Retrieve the iio context
	 * @return A pointer to the iio context
	 */
	virtual struct iio_context *getIioContext() = 0;


	/**
	* @brief Set a timeout for I/O operations
	* @param timeout A positive integer representing the time in milliseconds after which a timeout occurs. A value of 0 is used to specify that no timeout should occur.
	*/
	virtual void setTimeout(unsigned int timeout) = 0;

};
}
}

#endif // CONTEXT_HPP
