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

#include <cmath>
#include "m2kpowersupply_impl.hpp"
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/logger.hpp>
#include "utils/channel.hpp"
#include <libm2k/utils/utils.hpp>
#include <iio.h>

using namespace libm2k::analog;
using namespace libm2k::utils;


M2kPowerSupplyImpl::M2kPowerSupplyImpl(iio_context *ctx,
				       const std::map<std::string, std::string>& context_attrs,
				       std::string write_dev,
				       std::string read_dev, bool sync) :
	m_context_attrs(context_attrs),
	m_pos_powerdown_idx(2),
	m_neg_powerdown_idx(3),
	m_individual_powerdown(false)
{
	LIBM2K_LOG(INFO, "[BEGIN] Initialize M2kPowerSupply");
	m_generic_device = make_shared<DeviceOut>(ctx, "");

	if (write_dev != "") {
		m_dev_write = make_shared<DeviceOut>(ctx, write_dev);
		if (!m_dev_write) {
			m_dev_write = nullptr;
			THROW_M2K_EXCEPTION("M2K Power Supply: No device was found for writing", libm2k::EXC_INVALID_PARAMETER);
		}
	}

	if (read_dev != "") {
		m_dev_read = make_shared<DeviceIn>(ctx, read_dev);
		if (!m_dev_read) {
			m_dev_read = nullptr;
			THROW_M2K_EXCEPTION("M2K Power Supply: No device was found for reading", libm2k::EXC_INVALID_PARAMETER);
		}
	}

	if (m_dev_read->isChannel(2, false)) {
		m_read_channel_idx.push_back(2);
	} else {
		THROW_M2K_EXCEPTION("M2K Power Supply: Unable to find 1st read channel", libm2k::EXC_INVALID_PARAMETER);
	}

	if (m_dev_read->isChannel(1, false)) {
		m_read_channel_idx.push_back(1);
	} else {
		THROW_M2K_EXCEPTION("M2K Power Supply: Unable to find 2nd read channels", libm2k::EXC_INVALID_PARAMETER);
	}

	if (m_dev_write->isChannel(0, true)) {
		m_write_channel_idx.push_back(0);
	} else {
		THROW_M2K_EXCEPTION("M2K Power Supply: Unable to find 1st write channel", libm2k::EXC_INVALID_PARAMETER);
	}

	if (m_dev_write->isChannel(1, true)) {
		m_write_channel_idx.push_back(1);
	} else {
		THROW_M2K_EXCEPTION("M2K Power Supply: Unable to find 2nd write channels", libm2k::EXC_INVALID_PARAMETER);
	}

	m_channels_enabled.push_back(false);
	m_channels_enabled.push_back(false);


	m_m2k_fabric = make_shared<DeviceGeneric>(ctx, "m2k-fabric");
	if (!m_m2k_fabric) {
		THROW_M2K_EXCEPTION("M2K Power supply: Cannot find m2k fabric device", libm2k::EXC_INVALID_PARAMETER);
	}

	if (!m_m2k_fabric->isChannel(2, true)) {
		THROW_M2K_EXCEPTION("M2K Power supply: Cannot find powerdown channels", libm2k::EXC_INVALID_PARAMETER);
	}

	/* If neg powerdown channel is available */
	if (m_m2k_fabric->isChannel(3, true)) {
		m_individual_powerdown = true;
	}

	loadCalibrationCoefficients();

	m_write_coefficients.push_back(4095.0 / (5.02 * 1.2 ));
	m_write_coefficients.push_back(4095.0 / (-5.1 * 1.2 ));
	m_read_coefficients.push_back(6.4 / 4095.0);
	m_read_coefficients.push_back((-6.4)  / 4095.0);

	if (sync) {
		syncDevice();
	}
	LIBM2K_LOG(INFO, "[END] Initialize M2kPowerSupply");
}

M2kPowerSupplyImpl::~M2kPowerSupplyImpl()
{

}

void M2kPowerSupplyImpl::syncDevice()
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kPowerSupply sync");
	m_channels_enabled.at(0) = !m_dev_write->getBoolValue(m_write_channel_idx.at(0), "powerdown", true);
	m_channels_enabled.at(1) = !m_dev_write->getBoolValue(m_write_channel_idx.at(1), "powerdown", true);
	LIBM2K_LOG(INFO, "[END] M2kPowerSupply sync");
}

void M2kPowerSupplyImpl::reset()
{
	powerDownDacs(true);
	for (unsigned int i : m_write_channel_idx) {
		m_dev_write->setDoubleValue(i, 2048, "raw", true);
	}

	m_write_coefficients.at(0) = 4095.0 / (5.02 * 1.2 );
	m_write_coefficients.at(1) = 4095.0 / (-5.1 * 1.2 );
	m_read_coefficients.at(0) = 6.4 / 4095.0;
	m_read_coefficients.at(1) = (-6.4)  / 4095.0;
}

void M2kPowerSupplyImpl::powerDownDacs(bool powerdown)
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

void M2kPowerSupplyImpl::enableChannel(unsigned int chnIdx, bool en)
{
	if (chnIdx >= m_write_channel_idx.size()) {
		THROW_M2K_EXCEPTION("M2k PowerSupply: No such channel", libm2k::EXC_OUT_OF_RANGE);
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

bool M2kPowerSupplyImpl::anyChannelEnabled()
{
	for (bool en : m_channels_enabled) {
		if (en) {
			return en;
		}
	}
	return false;
}

void M2kPowerSupplyImpl::loadCalibrationCoefficients()
{
	/*Load calibration parameters from iio context*/
	m_calib_coefficients.clear();
	for (auto pair : m_context_attrs) {
		std::pair<std::string, double> calib_pair;
		__try {
			std::string cal_string = pair.first.substr(0, 4);
			if (cal_string != "cal,") {
				continue;
			}
			calib_pair.first = std::string(pair.first.c_str() + 4);
			calib_pair.second = Utils::safeStod(pair.second);

			m_calib_coefficients.push_back(calib_pair);
		} __catch (exception_type &) {
			continue;
		}
	}
}

double M2kPowerSupplyImpl::getCalibrationCoefficient(std::string key)
{
	for (std::pair<std::string, double> calib_pair : m_calib_coefficients) {
		if (calib_pair.first == key) {
			return calib_pair.second;
		}
	}
	THROW_M2K_EXCEPTION("M2k Power Supply: No such calibration coefficient", libm2k::EXC_INVALID_PARAMETER);
	return 0;
}

void M2kPowerSupplyImpl::enableAll(bool en)
{
	UNUSED(en);
}

double M2kPowerSupplyImpl::readChannel(unsigned int idx, bool calibrated)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kPowerSupply readChannel");
	double val = 0;
	double offset = 0.0;
	double gain = 1.0;
	double value = 0;

	if (idx >= m_read_channel_idx.size()) {
		THROW_M2K_EXCEPTION("M2k PowerSupply: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}

	if (calibrated) {
		if (idx == 0) {
			offset = getCalibrationCoefficient("offset_pos_dac");
			gain = getCalibrationCoefficient("gain_pos_dac");
		} else {
			offset = getCalibrationCoefficient("offset_neg_dac");
			gain = getCalibrationCoefficient("gain_neg_dac");
		}
	}

	//voltage2 and v1
	auto chn_name = "voltage" + to_string(m_read_channel_idx.at(idx));
	auto chn = m_dev_read->getChannel(chn_name, false);
	val = chn->getDoubleValue("raw");
	value = ((val * m_read_coefficients.at(idx)) + offset) * gain;
	LIBM2K_LOG(INFO, "[END] M2kPowerSupply readChannel");
	return value;
}

void M2kPowerSupplyImpl::pushChannel(unsigned int chnIdx, double value, bool calibrated)
{
	LIBM2K_LOG(INFO, "[BEGIN] M2kPowerSupply pushChannel");
	double offset = 0.0;
	double gain = 1.0;
	double val;

	if (chnIdx >= m_write_channel_idx.size()) {
		THROW_M2K_EXCEPTION("M2k PowerSupply: No such channel", libm2k::EXC_OUT_OF_RANGE);
	}

	if (std::abs(value) > 5) {
		THROW_M2K_EXCEPTION("M2K power supplies are limited to 5V", libm2k::EXC_INVALID_PARAMETER);
	}

	if (calibrated) {
		if (chnIdx == 0) {
			offset = getCalibrationCoefficient("offset_pos_dac");
			gain = getCalibrationCoefficient("gain_pos_dac");
		} else {
			offset = getCalibrationCoefficient("offset_neg_dac");
			gain = getCalibrationCoefficient("gain_neg_dac");
		}
	}

	val = (value * gain + offset) * m_write_coefficients.at(chnIdx);
	if (val < 0) {
		val = 0;
	}

	m_dev_write->setDoubleValue(m_write_channel_idx.at(chnIdx), val, "raw", true);
	LIBM2K_LOG(INFO, "[END] M2kPowerSupply pushChannel");
}
