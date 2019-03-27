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

#ifndef M2KPOWERSUPPLY_HPP
#define M2KPOWERSUPPLY_HPP

#include <libm2k/m2kglobal.hpp>
#include <libm2k/device.hpp>
#include <vector>
#include <memory>

using namespace libm2k::utils;

namespace libm2k {
namespace analog {
class LIBM2K_API M2kPowerSupply : public Device {
public:
	M2kPowerSupply(struct iio_context* ctx, std::string write_dev,
		       std::string read_dev);
	~M2kPowerSupply();

	void enableChannel(unsigned int chn, bool en);
	void enableAll(bool en);
	double readChannel(unsigned int chn);
	void pushChannel(unsigned int chn, double value);
	void powerDownDacs(bool powerdown);
	bool anyChannelEnabled();
private:
	std::shared_ptr<Device> m_dev_write;
	std::shared_ptr<Device> m_dev_read;
	std::shared_ptr<Device> m_m2k_fabric;

	void loadCalibrationCoefficients();
	double getCalibrationCoefficient(std::string key);
	std::vector<std::pair<std::string, double>> m_calib_coefficients;
	std::vector<double> m_write_coefficients;
	std::vector<double> m_read_coefficients;
	unsigned int m_pos_powerdown_idx;
	unsigned int m_neg_powerdown_idx;
	bool m_individual_powerdown;
	std::vector<bool> m_channels_enabled;
	std::vector<unsigned int> m_write_channel_idx;
	std::vector<unsigned int> m_read_channel_idx;
};
}
}


#endif //M2KPOWERSUPPLY_HPP
