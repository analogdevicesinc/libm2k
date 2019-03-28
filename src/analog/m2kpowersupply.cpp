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

#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <iio.h>

using namespace libm2k::analog;

M2kPowerSupply::M2kPowerSupply(iio_context *ctx, std::string write_dev,
			       std::string read_dev) :
	Device(ctx, ""),
	m_pos_powerdown_idx(2),
	m_neg_powerdown_idx(3),
	m_individual_powerdown(false)
{
	try {
		if (write_dev != "") {
			m_dev_write = make_shared<Device>(ctx, write_dev);
			if (!m_dev_write) {
				m_dev_write = nullptr;
				throw invalid_parameter_exception("M2K Power Supply: No device was found for writing");
			}
		}

		if (read_dev != "") {
			m_dev_read = make_shared<Device>(ctx, read_dev);
			if (!m_dev_read) {
				m_dev_read = nullptr;
				throw invalid_parameter_exception("M2K Power Supply: No device was found for reading");
			}
		}

		if (m_dev_read->isChannel(2, false)) {
			m_read_channel_idx.push_back(2);
		} else {
			throw invalid_parameter_exception("M2K Power Supply: Unable to find 1st read channel");
		}

		if (m_dev_read->isChannel(1, false)) {
			m_read_channel_idx.push_back(1);
		} else {
			throw invalid_parameter_exception("M2K Power Supply: Unable to find 2nd read channels");
		}

		if (m_dev_write->isChannel(0, true)) {
			m_write_channel_idx.push_back(0);
		} else {
			throw invalid_parameter_exception("M2K Power Supply: Unable to find 1st write channel");
		}

		if (m_dev_write->isChannel(1, true)) {
			m_write_channel_idx.push_back(1);
		} else {
			throw invalid_parameter_exception("M2K Power Supply: Unable to find 2nd write channels");
		}

		m_channels_enabled.push_back(false);
		m_channels_enabled.push_back(false);


		m_m2k_fabric = make_shared<Device>(ctx, "m2k-fabric");
		if (!m_m2k_fabric) {
			throw invalid_parameter_exception("M2K Power supply: Can not find m2k fabric device");
		}

		if (!m_m2k_fabric->isChannel(2, true)) {
			throw invalid_parameter_exception("M2K Power supply: Can not find powerdown channels");
		}

		/* If neg powerdown channel is available */
		if (m_m2k_fabric->isChannel(3, true)) {
			m_individual_powerdown = true;
		}

		powerDownDacs(true);
		loadCalibrationCoefficients();

		for (unsigned int i : m_write_channel_idx) {
			m_dev_write->setDoubleValue(i, 0.0, "raw", true);
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
	m_m2k_fabric->setBoolValue(m_pos_powerdown_idx, powerdown,
				   "user_supply_powerdown", true);
	if (m_individual_powerdown) {
		m_m2k_fabric->setBoolValue(m_neg_powerdown_idx, powerdown,
					   "user_supply_powerdown", true);
	}

	for (unsigned int i : m_write_channel_idx) {
		m_dev_write->setBoolValue(i, powerdown, "powerdown", true);
	}
}

void M2kPowerSupply::enableChannel(unsigned int chnIdx, bool en)
{
	if (chnIdx >= m_write_channel_idx.size()) {
		throw invalid_parameter_exception("Can't enable channel");
	}
	m_dev_write->setBoolValue(m_write_channel_idx.at(chnIdx), !en, "powerdown", true);

	m_channels_enabled[chnIdx] = en;

	if (m_individual_powerdown) {
		if (chnIdx == 0) {
			m_m2k_fabric->setBoolValue(m_pos_powerdown_idx, !en,
						   "user_supply_powerdown", true);
		} else {
			m_m2k_fabric->setBoolValue(m_neg_powerdown_idx, !en,
						   "user_supply_powerdown", true);
		}
	} else {
		if (en || (!en && !anyChannelEnabled())) {
			m_m2k_fabric->setBoolValue(m_pos_powerdown_idx, !en,
						   "user_supply_powerdown", true);
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
		if (!iio_context_get_attr(m_context, i, &name, &value)) {
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
	throw invalid_parameter_exception("M2k Power Supply: No such calibration coefficient");
}

void M2kPowerSupply::enableAll(bool en)
{

}

double M2kPowerSupply::readChannel(unsigned int idx)
{
	double val = 0;
	double offset = 0;
	double gain = 0;
	double value = 0;

	if (idx >= m_read_channel_idx.size()) {
		throw invalid_parameter_exception("Can't read value from channel");
	}

	try {
		if (idx == 0) {
			offset = getCalibrationCoefficient("offset_pos_dac");
			gain = getCalibrationCoefficient("gain_pos_dac");
		} else {
			offset = getCalibrationCoefficient("offset_neg_dac");
			gain = getCalibrationCoefficient("gain_neg_dac");
		}

		//voltage2 and v1
		val = m_dev_read->getDoubleValue(m_read_channel_idx.at(idx), "raw", false);
		value = ((val * m_read_coefficients.at(idx)) + offset) * gain;

		return value;
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}

void M2kPowerSupply::pushChannel(unsigned int chnIdx, double value)
{
	double offset = 0;
	double gain = 0;
	double val;

	if (chnIdx >= m_write_channel_idx.size()) {
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

		m_dev_write->setDoubleValue(m_write_channel_idx.at(chnIdx), val, "raw", true);
	} catch (std::runtime_error &e) {
		throw invalid_parameter_exception(e.what());
	}
}
