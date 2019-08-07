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
	DMM(struct iio_context *ctx, std::string dev, bool sync);


	/**
	* @private
	*/
	virtual ~DMM();


	/**
	* @private
	*/
	void init();


	/**
	* @brief Retrieve all channels
	*
	* @return A list containing the name of all channels
	*/
	std::vector<std::string> getAllChannels();


	/**
	* @brief Retrieve additional information about the given channel
	*
	* @param index The index corresponding to the channel
	* @return A structure containing additional information
	*/
	libm2k::analog::DMM_READING readChannel(unsigned int index);


	/**
	* @brief Retrieve additional information about the given channel
	*
	* @param chn_name The name corresponding to the channel
	* @return A structure containing additional information
	*/
	libm2k::analog::DMM_READING readChannel(std::string chn_name);


	/**
	* @brief Retrieve additional information about all channels
	*
	* @return A list containing structures for each channel
	*/
	std::vector<libm2k::analog::DMM_READING> readAll();


	/**
	* @brief Retrieve the device's name
	*
	* @return The name of the device
	*/
	std::string getName();

private:
	class DMMImpl;
	std::unique_ptr<DMMImpl> m_pimpl;
};
}
}



#endif //GENERICDMM_HPP
