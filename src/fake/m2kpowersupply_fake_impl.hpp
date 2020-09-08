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

#ifndef M2KPOWERSUPPLY_FAKE_IMPL_HPP
#define M2KPOWERSUPPLY_FAKE_IMPL_HPP

#include <libm2k/analog/m2kpowersupply.hpp>
#include "utils/devicein.hpp"
#include "utils/deviceout.hpp"
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
class M2kFakePowerSupplyImpl : public M2kPowerSupply
{
public:
	/**
	* @private
	*/
	M2kFakePowerSupplyImpl(struct iio_context* ctx, std::string write_dev,
		std::string read_dev, bool sync);


	/**
	* @private
	*/
	virtual ~M2kFakePowerSupplyImpl();


	/**
	* @private
	*/
	void reset();


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
	std::shared_ptr<libm2k::utils::DeviceOut> m_dev_write;
	std::shared_ptr<libm2k::utils::DeviceIn> m_dev_read;
	std::shared_ptr<libm2k::utils::DeviceGeneric> m_m2k_fabric;
	std::shared_ptr<libm2k::utils::DeviceGeneric> m_generic_device;

	std::vector<std::pair<std::string, double>> m_calib_coefficients;
	std::vector<double> m_write_coefficients;
	std::vector<double> m_read_coefficients;
	unsigned int m_pos_powerdown_idx;
	unsigned int m_neg_powerdown_idx;
	bool m_individual_powerdown;
	std::vector<bool> m_channels_enabled;
	std::vector<unsigned int> m_write_channel_idx;
	std::vector<unsigned int> m_read_channel_idx;

	void loadCalibrationCoefficients();
	double getCalibrationCoefficient(std::string key);
	void syncDevice();
};
}
}



#endif //M2KPOWERSUPPLY_IMPL_HPP
