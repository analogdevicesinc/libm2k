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

#ifndef M2KPOWERSUPPLY_HPP
#define M2KPOWERSUPPLY_HPP

#include <libm2k/m2kglobal.hpp>
#include <vector>
#include <memory>

namespace libm2k {
/**
 * @addtogroup analog
 * @{
 */
namespace analog {
/**
 * @defgroup powersupply PowerSupply
 * @brief Contains the representation of the power supply
 * @{
 * @class M2kPowerSupply
 * @brief Controls the power supply
 */
class LIBM2K_API M2kPowerSupply {
public:
	/**
	* @private
	*/
	M2kPowerSupply(struct iio_context* ctx, std::string write_dev,
		std::string read_dev, bool sync);


	/**
	* @private
	*/
	virtual ~M2kPowerSupply();


	/**
	* @private
	*/
	void init();


	/**
	* @brief Enable or disable the given channel
	*
	* @param chn The index corresponding to the channel
	* @param en A boolean value corresponding to the state of the channel
	*/
	void enableChannel(unsigned int chn, bool en);


	/**
	* @brief  Enable or disable all channels
	*
	* @param en A boolean value corresponding to the state of the channels
	*/
	void enableAll(bool en);


	/**
	* @brief Retrieve the value of a given channel
	*
	* @param chn The index corresponding to the channel
	* @return double The voltage transmitted by the given channel
	*/
	double readChannel(unsigned int chn);


	/**
	* @brief Set the voltage for the given channel
	*
	* @param chn The index corresponding to the channel
	* @param value The voltage (up to 5V)
	*/
	void pushChannel(unsigned int chn, double value);


	/**
	* @private
	*/
	void powerDownDacs(bool powerdown);


	/**
	* @brief Check if there is at least one enabled channel
	*
	* @return On succes, true
	* @return Otherwise, false
	*/
	bool anyChannelEnabled();


private:
	class M2kPowerSupplyImpl;
	std::unique_ptr<M2kPowerSupplyImpl> m_pimpl;
};
}
}



#endif //M2KPOWERSUPPLY_HPP
