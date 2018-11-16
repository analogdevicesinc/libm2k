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
#include <libm2k/powersupply.hpp>
#include <vector>

namespace libm2k {
namespace analog {
class LIBM2K_API M2kPowerSupply : public PowerSupply {
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
	void loadCalibrationCoefficients();
	double getCalibrationCoefficient(std::string key);
	std::vector<std::pair<std::string, double>> m_calib_coefficients;
	std::vector<double> m_write_coefficients;
	std::vector<double> m_read_coefficients;
	struct iio_channel* m_pos_powerdown;
	struct iio_channel* m_neg_powerdown;
	bool m_individual_powerdown;
};
}
}


#endif //M2KPOWERSUPPLY_HPP
