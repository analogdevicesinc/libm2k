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

#include "libm2k/m2kpowersupply.hpp"
#include "libm2k/m2kexceptions.hpp"
#include <iio.h>

using namespace libm2k::analog;

M2kPowerSupply::M2kPowerSupply(iio_context *ctx, std::string write_dev,
			       std::string read_dev) :
	PowerSupply(ctx, write_dev, read_dev),
	m_individual_powerdown(false)
{
	try {
		m_read_channels.push_back(iio_device_find_channel(getReadDevice(),
								   "voltage2", false));
		m_read_channels.push_back(iio_device_find_channel(getReadDevice(),
								   "voltage1", false));
		m_write_channels.push_back(iio_device_find_channel(getWriteDevice(),
								  "voltage0", true));
		m_write_channels.push_back(iio_device_find_channel(getWriteDevice(),
								   "voltage1", true));
		if ((m_read_channels.size() != 2) || (m_write_channels.size() != 2)) {
			throw invalid_parameter_exception("Unable to find channels");
		}

		m_nb_read_channels = m_read_channels.size();
		m_nb_write_channels = m_write_channels.size();

		m_channels_enabled.push_back(false);
		m_channels_enabled.push_back(false);

		struct iio_device* powerdown_dev = iio_context_find_device(getContext(), "m2k-fabric");

		m_pos_powerdown = iio_device_find_channel(powerdown_dev, "voltage2", true);
		/* For HW Rev. C */
		m_neg_powerdown = iio_device_find_channel(powerdown_dev, "voltage3", true);

		if (!m_pos_powerdown) {
			throw invalid_parameter_exception("Unable to find channels");
		}
		if (m_neg_powerdown) {
			m_individual_powerdown = true;
		}

		powerDownDacs(true);
		loadCalibrationCoefficients();

		for (struct iio_channel* chn : m_write_channels) {
			iio_channel_attr_write_longlong(chn, "raw", 0LL);
		}

		m_write_coefficients.push_back(4095.0 / (5.02 * 1.2 ));
		m_write_coefficients.push_back(4095.0 / (-5.1 * 1.2 ));
		m_read_coefficients.push_back(6.4 / 4095.0);
		m_read_coefficients.push_back((-6.4)  / 4095.0);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

M2kPowerSupply::~M2kPowerSupply()
{

}

void M2kPowerSupply::powerDownDacs(bool powerdown)
{
	iio_channel_attr_write_bool(m_pos_powerdown, "user_supply_powerdown", powerdown);
	if (m_individual_powerdown) {
		iio_channel_attr_write_bool(m_neg_powerdown, "user_supply_powerdown", powerdown);
	}

	for (struct iio_channel* chn : m_write_channels) {
		iio_channel_attr_write_bool(chn, "powerdown", powerdown);
	}
}

void M2kPowerSupply::enableChannel(unsigned int chnIdx, bool en)
{
	if (chnIdx >= m_write_channels.size()) {
		throw invalid_parameter_exception("Can't enable channel");
	}
	iio_channel_attr_write_bool(m_write_channels.at(chnIdx), "powerdown", !en);
	m_channels_enabled[chnIdx] = en;

	if (m_individual_powerdown) {
		struct iio_channel* chn;
		if (chnIdx == 0) {
			chn = m_pos_powerdown;
		} else {
			chn = m_neg_powerdown;
		}
		iio_channel_attr_write_bool(chn, "user_supply_powerdown", !en);
	} else {
		if (en || (!en && !anyChannelEnabled())) {
			iio_channel_attr_write_bool(m_pos_powerdown, "user_supply_powerdown", !en);
		}
	}
}

bool M2kPowerSupply::anyChannelEnabled()
{
	for (bool en : m_channels_enabled) {
		if (en) {
			return en;
		}
	}
	return false;
}

void M2kPowerSupply::loadCalibrationCoefficients()
{
	/*Load calibration parameters from iio context*/
	const char *name;
	const char *value;
	m_calib_coefficients.clear();
	for (int i = 4; i < 12; i++) {
		if (!iio_context_get_attr(getContext(), i, &name, &value)) {
			std::pair<std::string, double> calib_pair;
			calib_pair.first = std::string(name + 4);
			calib_pair.second = std::stod(std::string(value));
			m_calib_coefficients.push_back(calib_pair);
		}
	}
}

double M2kPowerSupply::getCalibrationCoefficient(std::string key)
{
	for (std::pair<std::string, double> calib_pair : m_calib_coefficients) {
		if (calib_pair.first == key) {
			return calib_pair.second;
		}
	}
	throw invalid_parameter_exception("No such calibration coefficient");
}

void M2kPowerSupply::enableAll(bool en)
{

}

double M2kPowerSupply::readChannel(unsigned int chnIdx)
{
	long long val = 0;
	double offset = 0;
	double gain = 0;
	double value = 0;

	if (chnIdx >= m_read_channels.size()) {
		throw invalid_parameter_exception("Can't read value from channel");
	}

	try {
		if (chnIdx == 0) {
			offset = getCalibrationCoefficient("offset_pos_dac");
			gain = getCalibrationCoefficient("gain_pos_dac");
		} else {
			offset = getCalibrationCoefficient("offset_neg_dac");
			gain = getCalibrationCoefficient("gain_neg_dac");
		}

		iio_channel_attr_read_longlong(m_read_channels.at(chnIdx), "raw", &val);
		value = (((double) val * m_read_coefficients.at(chnIdx)) + offset) * gain;

		return value;
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kPowerSupply::pushChannel(unsigned int chnIdx, double value)
{
	double offset = 0;
	double gain = 0;
	long long val;

	if (chnIdx >= m_write_channels.size()) {
		throw invalid_parameter_exception("Can't write value to channel");
	}

	if (abs(value) > 5) {
		throw invalid_parameter_exception("M2K power supplies are limited to 5V");
	}

	try {
		if (chnIdx == 0) {
			offset = getCalibrationCoefficient("offset_pos_dac");
			gain = getCalibrationCoefficient("gain_pos_dac");
		} else {
			offset = getCalibrationCoefficient("offset_neg_dac");
			gain = getCalibrationCoefficient("gain_neg_dac");
		}

		val = (value * gain + offset) * m_write_coefficients.at(chnIdx);
		if (val < 0) {
			val = 0;
		}

		iio_channel_attr_write_longlong(m_write_channels.at(chnIdx), "raw", val);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}
