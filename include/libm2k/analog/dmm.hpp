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

#ifndef GENERICDMM_HPP
#define GENERICDMM_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/analog/enums.hpp>
#include <vector>
#include <map>
#include <string>

namespace libm2k {
/**
 * @addtogroup analog
 * @{
 */
namespace analog {

/**
 * @defgroup dmm DMM
 * @brief Contains the representation of the digital multimeter
 * @{
 * @class DMM
 * @brief Controls the digital multimeter
 */
class LIBM2K_API DMM {
public:
	/**
	* @private
	*/
	virtual ~DMM() {}


	/**
	* @private
	*/
	virtual void init() = 0;


	/**
	* @brief Retrieve all channels
	*
	* @return A list containing the name of all channels
	*/
	virtual std::vector<std::string> getAllChannels() = 0;


	/**
	* @brief Retrieve additional information about the given channel
	*
	* @param index The index corresponding to the channel
	* @return A structure containing additional information
	*/
	virtual libm2k::analog::DMM_READING readChannel(unsigned int index) = 0;


	/**
	* @brief Retrieve additional information about the given channel
	*
	* @param chn_name The name corresponding to the channel
	* @return A structure containing additional information
	*/
	virtual libm2k::analog::DMM_READING readChannel(std::string chn_name) = 0;


	/**
	* @brief Retrieve additional information about all channels
	*
	* @return A list containing structures for each channel
	*/
	virtual std::vector<libm2k::analog::DMM_READING> readAll() = 0;


	/**
	* @brief Retrieve the device's name
	*
	* @return The name of the device
	*/
	virtual std::string getName() = 0;

};
}
}



#endif //GENERICDMM_HPP
