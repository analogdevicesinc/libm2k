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

#include "dmm_impl.hpp"
#include "utils/devicegeneric.hpp"
#include "utils/devicein.hpp"
#include <libm2k/m2kexceptions.hpp>
#include "utils/channel.hpp"
#include <iio.h>
#include <iostream>

using namespace libm2k::analog;
using namespace libm2k::utils;


DMMImpl::DMMImpl(struct iio_context *ctx, std::string dev, bool sync)
{
	UNUSED(sync);
	m_device_in_list.push_back(new DeviceIn(ctx, dev));
	m_dev_name = dev;
	bool output = false;
	for (unsigned int i = 0; i < getDevice(0)->getNbChannels(output); i++) {
		if (getDevice(0)->isValidDmmChannel(i)) {
			m_channel_id_list.insert(std::pair<std::string, unsigned int>
						 (getDevice(0)->getChannel(i, output)->getId(), i));
		}
	}

	generateDictionaries();
}

DMMImpl::~DMMImpl()
{
	for (auto d : m_device_in_list) {
		delete d;
	}
	m_device_in_list.clear();
}

void DMMImpl::reset()
{

}

DeviceIn* DMMImpl::getDevice(unsigned int index)
{
	if (index >= m_device_in_list.size()) {
		THROW_M2K_EXCEPTION("No such DMM device.", libm2k::EXC_OUT_OF_RANGE);
	}
	return m_device_in_list.at(index);
}


DMM_READING DMMImpl::readChannel(unsigned int index)
{
	std::string chn_name = "";
	for (auto pair : m_channel_id_list) {
		if (pair.second == index) {
			chn_name = pair.first;
			break;
		}
	}
	return readChannel(chn_name);
}

std::vector<std::string> DMMImpl::getAllChannels()
{
	std::vector<std::string> ids = {};
	for (auto pair : m_channel_id_list) {
		ids.push_back(pair.first);
	}
	return ids;
}

void DMMImpl::generateDictionaries()
{
	m_hwmonDevices[hwmon_chan_type::HWMON_VOLTAGE].key = "Voltage";
	m_hwmonDevices[hwmon_chan_type::HWMON_VOLTAGE].key_symbol = "V";
	m_hwmonDevices[hwmon_chan_type::HWMON_VOLTAGE].umScale = 0.001;

	m_hwmonDevices[hwmon_chan_type::HWMON_TEMP].key = "Degree Celsius";
	m_hwmonDevices[hwmon_chan_type::HWMON_TEMP].key_symbol = "°C";
	m_hwmonDevices[hwmon_chan_type::HWMON_TEMP].umScale = 0.001;

	m_hwmonDevices[hwmon_chan_type::HWMON_CURRENT].key = "Ampere";
	m_hwmonDevices[hwmon_chan_type::HWMON_CURRENT].key_symbol = "A";
	m_hwmonDevices[hwmon_chan_type::HWMON_CURRENT].umScale = 0.001;

	m_hwmonDevices[hwmon_chan_type::HWMON_POWER].key = "milliWatt";
	m_hwmonDevices[hwmon_chan_type::HWMON_POWER].key_symbol = "mW";
	m_hwmonDevices[hwmon_chan_type::HWMON_POWER].umScale = 0.001;

	m_hwmonDevices[hwmon_chan_type::HWMON_ENERGY].key = "milliJoule";
	m_hwmonDevices[hwmon_chan_type::HWMON_ENERGY].key_symbol = "mJ";
	m_hwmonDevices[hwmon_chan_type::HWMON_ENERGY].umScale = 0.001;

	m_hwmonDevices[hwmon_chan_type::HWMON_FAN].key = "Revolution/Min";
	m_hwmonDevices[hwmon_chan_type::HWMON_FAN].key_symbol = "RPM";

	m_hwmonDevices[hwmon_chan_type::HWMON_HUMIDITY].key = "milli-percent";
	m_hwmonDevices[hwmon_chan_type::HWMON_HUMIDITY].key_symbol = "pcm";

	m_iioDevices[iio_chan_type::IIO_VOLTAGE].key = "Voltage";
	m_iioDevices[iio_chan_type::IIO_VOLTAGE].key_symbol = "V";
	m_iioDevices[iio_chan_type::IIO_VOLTAGE].umScale = 0.001;

	m_iioDevices[iio_chan_type::IIO_TEMP].key = "Degree Celsius";
	m_iioDevices[iio_chan_type::IIO_TEMP].key_symbol = "°C";
	m_iioDevices[iio_chan_type::IIO_TEMP].umScale = 0.001;

	m_iioDevices[iio_chan_type::IIO_CURRENT].key = "Ampere";
	m_iioDevices[iio_chan_type::IIO_CURRENT].key_symbol = "A";
	m_iioDevices[iio_chan_type::IIO_CURRENT].umScale = 0.001;

	m_iioDevices[iio_chan_type::IIO_PRESSURE].key = "Pascal";
	m_iioDevices[iio_chan_type::IIO_PRESSURE].key_symbol = "Pa";
	m_iioDevices[iio_chan_type::IIO_PRESSURE].umScale = 1000;

	m_iioDevices[iio_chan_type::IIO_ACCEL].key = "Metre per second squared";
	m_iioDevices[iio_chan_type::IIO_ACCEL].key_symbol = "m/s²";

	m_iioDevices[iio_chan_type::IIO_ANGL_VEL].key = "Radian per second";
	m_iioDevices[iio_chan_type::IIO_ACCEL].key_symbol = "rad/s";

	m_iioDevices[iio_chan_type::IIO_MAGN].key = "Gauss";
	m_iioDevices[iio_chan_type::IIO_MAGN].key_symbol = "Gs";

}

DMM_READING DMMImpl::readChannel(std::string chn_name)
{
	DMM_READING result;
	double value = 0;
	double offset = 0;
	double scale = 1;
	std::string id = getDevice(0)->getChannel(m_channel_id_list.at(chn_name), false)->getId();
	std::string name = getDevice(0)->getChannel(m_channel_id_list.at(chn_name), false)->getName();
	unsigned int index = m_channel_id_list.at(chn_name);
	auto channel = getDevice(0)->getChannel(index, false);
	if (channel->hasAttribute("raw")) {
		value = channel->getDoubleValue("raw");
	} else if (channel->hasAttribute("processed")) {
		value = channel->getDoubleValue("processed");
	} else if (channel->hasAttribute("input")) {
		value = channel->getDoubleValue("input");
	} else {
		THROW_M2K_EXCEPTION("DMM: Cannot read channel " + getName() + " : " + chn_name, libm2k::EXC_OUT_OF_RANGE);
	}

	if (channel->hasAttribute("offset")) {
		offset = channel->getDoubleValue("offset");
	}

	if (channel->hasAttribute("scale")) {
		scale = channel->getDoubleValue("scale");
	}

	bool isHwmon = iio_device_is_hwmon(channel->getDevice());

	dmm_info dmm;

	if (isHwmon) {
		int type = hwmon_channel_get_type(channel->getChannel());

		if (type != hwmon_chan_type::HWMON_CHAN_TYPE_UNKNOWN) {
			dmm = m_hwmonDevices[type];
			dmm.value = (value + offset) * scale * dmm.umScale;
		} else {
			dmm.key = "";
			dmm.key_symbol = "";
		}
	} else {
		int type = iio_channel_get_type(channel->getChannel());

		if (type != iio_chan_type::IIO_CHAN_TYPE_UNKNOWN) {
			dmm = m_iioDevices[type];
			dmm.value = (value + offset) * scale * dmm.umScale;
		} else {
			dmm.key = "";
			dmm.key_symbol = "";
		}
	}

	result.id = id;
	result.name = name;
	result.unit_name = dmm.key;
	result.unit_symbol = dmm.key_symbol;
	result.value = dmm.value;
	return result;
}

std::vector<DMM_READING> DMMImpl::readAll()
{
	std::vector<DMM_READING> result = {};
	for (auto pair : m_channel_id_list) {
		DMM_READING res = readChannel(pair.first);
		result.push_back(res);
	}
	return result;
}

string DMMImpl::getName()
{
	return getDevice(0)->getName();
}

