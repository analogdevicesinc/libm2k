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

#include "libm2k/m2k.hpp"
#include "libm2k/m2kanalogin.hpp"
#include "libm2k/m2kanalogout.hpp"
#include "libm2k/m2kexceptions.hpp"
#include "libm2k/m2kcalibration.hpp"
#include "libm2k/m2kpowersupply.hpp"
#include "libm2k/m2kdigital.hpp"
#include "utils.hpp"
#include <iio.h>
#include <iostream>

#include "logger.h"

using namespace std;
using namespace libm2k::devices;
using namespace libm2k::analog;
using namespace libm2k::digital;
using namespace libm2k::utils;

M2K::M2K(std::string uri, iio_context* ctx, std::string name) :
	GenericDevice(uri, ctx, name)
{
	std::cout << "I am M2K device " << std::endl;

	initialize();


	LOG("BOOOMMM");



	/* Initialize the AnalogIn list */
//	for (auto aIn : s_instancesAnalogIn) {
//	}
	m_instancesAnalogIn.clear();

	/* Initialize the AnalogOut list */
//	for (auto aOut : s_instancesAnalogOut) {
//	}
	m_instancesAnalogOut.clear();
	m_instancesPowerSupply.clear();

	scanAllAnalogIn();
	scanAllAnalogOut();
	scanAllPowerSupply();
	scanAllDigital();
	std::vector<std::shared_ptr<libm2k::analog::M2kAnalogIn>> lstIn = getAllAnalogIn();
	std::vector<std::shared_ptr<libm2k::analog::M2kAnalogOut>> lstOut = getAllAnalogOut();
	m_calibration = new M2kCalibration(lstIn, lstOut);
}

M2K::~M2K()
{
	/* Pre-init call to setup M2k */
	struct iio_device *m2k_fabric = iio_context_find_device(ctx(), "m2k-fabric");
	if (m2k_fabric) {
		auto chn0 = iio_device_find_channel(m2k_fabric, "voltage0", false);
		auto chn1 = iio_device_find_channel(m2k_fabric, "voltage1", false);
		if (chn0 && chn1) {
			iio_channel_attr_write_bool(chn0, "powerdown", true);
			iio_channel_attr_write_bool(chn1, "powerdown", true);
		}

		/* ADF4360 global clock power down */
		iio_device_attr_write_bool(m2k_fabric, "clk_powerdown", true);
	}
	delete m_calibration;
}

void M2K::scanAllAnalogIn()
{
	try {
		std::shared_ptr<GenericAnalogIn> aIn = std::make_shared<libm2k::analog::M2kAnalogIn>(ctx(), "m2k-adc");
		m_instancesAnalogIn.push_back(aIn);
	} catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}
}

void M2K::scanAllAnalogOut()
{
	try {
		std::shared_ptr<GenericAnalogOut> aOut = std::make_shared<libm2k::analog::M2kAnalogOut>(ctx(), "m2k-dac-a");
		m_instancesAnalogOut.push_back(aOut);

		std::shared_ptr<GenericAnalogOut> bOut = std::make_shared<libm2k::analog::M2kAnalogOut>(ctx(), "m2k-dac-b");
		m_instancesAnalogOut.push_back(bOut);
	} catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}
}

void M2K::scanAllPowerSupply()
{
	try {
		std::shared_ptr<PowerSupply> pSupply = std::make_shared<libm2k::analog::M2kPowerSupply>(ctx(), "ad5627", "ad9963");
		m_instancesPowerSupply.push_back(pSupply);
	} catch (std::runtime_error &e) {
		std::cout << e.what() << std::endl;
	}
}

void M2K::scanAllDigital()
{
	try {
		std::shared_ptr<GenericDigital> logic = std::make_shared<libm2k::digital::M2kDigital>(ctx(), "m2k-logic-analyzer");
		m_instancesDigital.push_back(logic);
	} catch (std::runtime_error &e) {
		std::cout << e.what() << std::endl;
	}
}

void M2K::calibrate()
{

}

bool M2K::resetCalibration()
{
	try {
		return m_calibration->resetCalibration();
	} catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

bool M2K::calibrateADC()
{
	try {
		return m_calibration->calibrateADC();
	} catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

bool M2K::calibrateDAC()
{
	try {
		return m_calibration->calibrateDAC();
	} catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

double M2K::getAdcCalibrationGain(unsigned int chn)
{
	if (chn >= getAnalogIn(0)->numChannels()) {
		throw std::runtime_error("No such ADC channel");
	}
	if (chn == 0) {
		return m_calibration->adcGainChannel0();
	} else {
		return m_calibration->adcGainChannel1();
	}
}

int M2K::getAdcCalibrationOffset(unsigned int chn)
{
	if (chn >= getAnalogIn(0)->numChannels()) {
		throw std::runtime_error("No such ADC channel");
	}
	if (chn == 0) {
		return m_calibration->adcOffsetChannel0();
	} else {
		return m_calibration->adcOffsetChannel1();
	}
}

double M2K::getDacACalibrationGain()
{
	return m_calibration->dacAvlsb();
}

double M2K::getDacBCalibrationGain()
{
	return m_calibration->dacBvlsb();
}

int M2K::getDacACalibrationOffset()
{
	return m_calibration->dacAoffset();
}

int M2K::getDacBCalibrationOffset()
{
	return m_calibration->dacBoffset();
}

std::shared_ptr<M2kAnalogIn> M2K::getAnalogIn(unsigned int index)
{
	if (index < m_instancesAnalogIn.size()) {
		return dynamic_pointer_cast<libm2k::analog::M2kAnalogIn>(
					m_instancesAnalogIn.at(index));
	} else {
		throw no_device_exception("No such analog in");
//		return nullptr;
	}
}

std::shared_ptr<M2kAnalogIn> M2K::getAnalogIn(string dev_name)
{
	for (std::shared_ptr<GenericAnalogIn> d : m_instancesAnalogIn) {
		if (d->getDeviceName() == dev_name) {
			std::shared_ptr<libm2k::analog::M2kAnalogIn> analogIn =
				dynamic_pointer_cast<libm2k::analog::M2kAnalogIn>(d);
			if (analogIn) {
				return analogIn;
			}
		}
	}
	return nullptr;
}

std::shared_ptr<M2kPowerSupply> M2K::getPowerSupply()
{
	std::shared_ptr<M2kPowerSupply> pSupply = dynamic_pointer_cast<M2kPowerSupply>(m_instancesPowerSupply.at(0));
	if (pSupply) {
		return pSupply;
	}
	throw no_device_exception("No M2K power supply");
}

std::shared_ptr<M2kDigital> M2K::getDigital()
{
	std::shared_ptr<M2kDigital> logic = dynamic_pointer_cast<M2kDigital>(m_instancesDigital.at(0));
	if (logic) {
		return logic;
	}
	throw no_device_exception("No M2K digital device found");
}

std::shared_ptr<M2kAnalogOut> M2K::getAnalogOut(string dev_name)
{
	for (std::shared_ptr<GenericAnalogOut> d : m_instancesAnalogOut) {
		if (d->getDeviceName() == dev_name) {
			std::shared_ptr<libm2k::analog::M2kAnalogOut> analogOut =
				dynamic_pointer_cast<M2kAnalogOut>(d);
			if (analogOut) {
				return analogOut;
			}
		}
	}
	return nullptr;
}

std::vector<std::shared_ptr<M2kAnalogIn>> M2K::getAllAnalogIn()
{
	std::vector<std::shared_ptr<libm2k::analog::M2kAnalogIn>> allAnalogIn = {};
	for (std::shared_ptr<GenericAnalogIn> inst : m_instancesAnalogIn) {
		try {
			std::shared_ptr<libm2k::analog::M2kAnalogIn> analogIn =
				dynamic_pointer_cast<M2kAnalogIn>(inst);
			if (analogIn) {
				allAnalogIn.push_back(analogIn);
			}
		} catch (std::runtime_error &e) {
			throw no_device_exception(e.what());
		}
	}
	return allAnalogIn;
}

std::vector<std::shared_ptr<M2kAnalogOut>> M2K::getAllAnalogOut()
{
	std::vector<std::shared_ptr<libm2k::analog::M2kAnalogOut>> allAnalogOut;
	for (std::shared_ptr<GenericAnalogOut> inst : m_instancesAnalogOut) {
		try {
			std::shared_ptr<libm2k::analog::M2kAnalogOut> analogOut =
				dynamic_pointer_cast<libm2k::analog::M2kAnalogOut>(inst);
			if (analogOut) {
				allAnalogOut.push_back(analogOut);
			}
		} catch (std::runtime_error &e) {
			throw no_device_exception(e.what());
		}
	}
	return allAnalogOut;
}

void M2K::startSyncedDacs()
{
	for (auto analogOut : getAllAnalogOut()) {
		analogOut->syncedStart();
	}

	struct iio_device *m2k_fabric = iio_context_find_device(ctx(), "m2k-fabric");
	if (m2k_fabric) {
		auto chn0 = iio_device_find_channel(m2k_fabric, "voltage0", true);
		auto chn1 = iio_device_find_channel(m2k_fabric, "voltage1", true);
		if (chn0 && chn1) {
			iio_channel_attr_write_bool(chn0, "powerdown", false);
			iio_channel_attr_write_bool(chn1, "powerdown", false);
		}
	}
}

void M2K::initialize()
{
	/* Pre-init call to setup M2k */
	struct iio_device *m2k_fabric = iio_context_find_device(ctx(), "m2k-fabric");
	if (m2k_fabric) {
		auto chn0 = iio_device_find_channel(m2k_fabric, "voltage0", false);
		auto chn1 = iio_device_find_channel(m2k_fabric, "voltage1", false);
		if (chn0 && chn1) {
			iio_channel_attr_write_bool(chn0, "powerdown", false);
			iio_channel_attr_write_bool(chn1, "powerdown", false);
		}
		iio_device_attr_write_bool(m2k_fabric, "clk_powerdown", false);
	}

	/* Apply M2k fixes */
	std::string hw_rev = Utils::getHardwareRevision(ctx());

	struct iio_device *dev = iio_context_find_device(ctx(), "ad9963");

	int config1 = 0x05;
	int config2 = 0x05;

	if (hw_rev == "A") {
		config1 = 0x1B; // IGAIN1 +-6db  0.25db steps
		config2 = 0x1B;
	}

	/* Configure TX path */
	iio_device_reg_write(dev, 0x68, config1);
	iio_device_reg_write(dev, 0x6B, config2);
	iio_device_reg_write(dev, 0x69, 0x1C);  // IGAIN2 +-2.5%
	iio_device_reg_write(dev, 0x6C, 0x1C);
	iio_device_reg_write(dev, 0x6A, 0x20);  // IRSET +-20%
	iio_device_reg_write(dev, 0x6D, 0x20);
}
