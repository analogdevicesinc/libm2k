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

#include "m2k_impl.hpp"
#include "analog/m2kanalogout_impl.hpp"
#include "analog/m2kanalogin_impl.hpp"
#include "analog/m2kpowersupply_impl.hpp"
#include "m2khardwaretrigger_impl.hpp"
#include "m2khardwaretrigger_v0.24_impl.hpp"
#include "m2kcalibration_impl.hpp"
#include <libm2k/analog/dmm.hpp>
#include "utils/channel.hpp"
#include <libm2k/m2kexceptions.hpp>
#include "digital/m2kdigital_impl.hpp"
#include <libm2k/utils/utils.hpp>
#include "utils/devicegeneric.hpp"
#include <libm2k/logger.hpp>
#include <iio.h>
#include <iostream>
#include <thread>

using namespace std;
using namespace libm2k::context;
using namespace libm2k::analog;
using namespace libm2k::digital;
using namespace libm2k::utils;

M2kImpl::M2kImpl(std::string uri, iio_context* ctx, std::string name, bool sync) :
	ContextImpl(uri, ctx, name, sync),
	m_sync(sync),
	m_deinit(false)
{
	initialize();
	setTimeout(UINT_MAX);

	for (auto ain : m_instancesAnalogIn) {
		delete ain;
	}
	for (auto aout : m_instancesAnalogOut) {
		delete aout;
	}
	for (auto ps : m_instancesPowerSupply) {
		delete ps;
	}
	m_instancesAnalogIn.clear();
	m_instancesAnalogOut.clear();
	m_instancesPowerSupply.clear();

	m_firmware_version = getFirmwareVersion();

	int diff = Utils::compareVersions(m_firmware_version, "v0.24");
	if (diff < 0) {	//m_firmware_version < 0.24
		m_trigger = new M2kHardwareTriggerImpl(ctx);
	} else {
		m_trigger = new M2kHardwareTriggerV024Impl(ctx);
	}

	if (!m_trigger) {
		throw_exception(EXC_INVALID_PARAMETER, "Can't instantiate M2K board; M2K trigger is invalid.");
	}

	scanAllAnalogIn();
	scanAllAnalogOut();
	scanAllPowerSupply();
	scanAllDigital();
	m_calibration = new M2kCalibrationImpl(ctx, getAnalogIn(), getAnalogOut());
}

M2kImpl::~M2kImpl()
{
	if (m_deinit) {
		std::shared_ptr<DeviceGeneric> m_m2k_fabric = make_shared<DeviceGeneric>(m_context, "m2k-fabric");
		if (m_m2k_fabric) {
			m_m2k_fabric->setBoolValue(0, true, "powerdown", false);
			m_m2k_fabric->setBoolValue(1, true, "powerdown", false);

			/* ADF4360 global clock power down */
			m_m2k_fabric->setBoolValue(true, "clk_powerdown");
			for (auto ps : m_instancesPowerSupply) {
				ps->powerDownDacs(true);
			}
		}
	}

	// The vertical offset register in the device has dual purpose:
	// 1. use as ADC offset for calibration
	// 2. use as ADC vertical offset for measurement
	// This can cause some confusion when initializing the board
	// because it is not clear whether the value in the register is
	// the calibration value or the calibration+vertical offset value
	// This workaround will always set the vertical offset to 0 on deinitialization
	// and upon initialization the offset will be loaded from the register
	// (when no calibration is done)
	//
	// The correct fix would be adding a separate caliboffset register in the firmware
	// which will clear up the confusion

	getAnalogIn()->setVerticalOffset(ANALOG_IN_CHANNEL_1,0);
	getAnalogIn()->setVerticalOffset(ANALOG_IN_CHANNEL_2,0);

	delete m_calibration;

	if (m_trigger) {
		delete m_trigger;
	}

	for (auto ain : m_instancesAnalogIn) {
		delete ain;
	}
	for (auto aout : m_instancesAnalogOut) {
		delete aout;
	}
	for (auto ps : m_instancesPowerSupply) {
		delete ps;
	}
	for (auto d : m_instancesDigital) {
		delete d;
	}
	m_instancesAnalogIn.clear();
	m_instancesAnalogOut.clear();
	m_instancesPowerSupply.clear();
}

void M2kImpl::deinitialize()
{
	m_deinit = true;
}

void M2kImpl::reset()
{
	for (auto ain : m_instancesAnalogIn) {
		ain->reset();
	}
	for (auto aout : m_instancesAnalogOut) {
		aout->reset();
	}
	for (auto ps : m_instancesPowerSupply) {
		ps->reset();
	}
	for (auto d : m_instancesDigital) {
		d->reset();
	}
	for (auto dmm : m_instancesDMM) {
		dmm->reset();
	}
	m_trigger->reset();
	initialize();

}

void M2kImpl::scanAllAnalogIn()
{
	M2kAnalogIn* aIn = new libm2k::analog::M2kAnalogInImpl(m_context, "m2k-adc", m_sync, m_trigger);
	m_instancesAnalogIn.push_back(aIn);
}

void M2kImpl::scanAllAnalogOut()
{
	std::vector<std::string> devs = {"m2k-dac-a", "m2k-dac-b"};
	M2kAnalogOut* aOut = new libm2k::analog::M2kAnalogOutImpl(m_context, devs, m_sync);
	m_instancesAnalogOut.push_back(aOut);
}

void M2kImpl::scanAllPowerSupply()
{
	libm2k::analog::M2kPowerSupply* pSupply = new libm2k::analog::M2kPowerSupplyImpl(m_context, "ad5627", "ad9963", m_sync);
	m_instancesPowerSupply.push_back(pSupply);
}

void M2kImpl::scanAllDigital()
{
	libm2k::digital::M2kDigital* logic = new libm2k::digital::M2kDigitalImpl(m_context, "m2k-logic-analyzer", m_sync, m_trigger);
	m_instancesDigital.push_back(logic);
}

bool M2kImpl::calibrate()
{
	bool ok_adc, ok_dac = false;
	ok_adc = calibrateADC();
	ok_dac = calibrateDAC();
	return ok_adc && ok_dac;
}

bool M2kImpl::resetCalibration()
{
	return m_calibration->resetCalibration();
}

bool M2kImpl::calibrateADC()
{
	return m_calibration->calibrateADC();
}

bool M2kImpl::calibrateDAC()
{
	return m_calibration->calibrateDAC();
}

double M2kImpl::getAdcCalibrationGain(unsigned int chn)
{
	if (chn >= getAnalogIn()->getNbChannels()) {
		throw_exception(EXC_OUT_OF_RANGE, "No such ADC channel");
	}
	if (chn == 0) {
		return m_calibration->adcGainChannel0();
	} else {
		return m_calibration->adcGainChannel1();
	}
}

int M2kImpl::getAdcCalibrationOffset(unsigned int chn)
{
	if (chn >= getAnalogIn()->getNbChannels()) {
		throw_exception(EXC_OUT_OF_RANGE, "No such ADC channel");
	}
	if (chn == 0) {
		return m_calibration->adcOffsetChannel0();
	} else {
		return m_calibration->adcOffsetChannel1();
	}
}

double M2kImpl::getDacCalibrationGain(unsigned int chn)
{
	if (chn >= getAnalogOut()->getNbChannels()) {
		throw_exception(EXC_OUT_OF_RANGE, "No such DAC channel");
	}
	if (chn == 0) {
		return m_calibration->dacAvlsb();
	} else {
		return m_calibration->dacBvlsb();
	}
}

int M2kImpl::getDacCalibrationOffset(unsigned int chn)
{
	if (chn >= getAnalogOut()->getNbChannels()) {
		throw_exception(EXC_OUT_OF_RANGE, "No such DAC channel");
	}
	if (chn == 0) {
		return m_calibration->dacAoffset();
	} else {
		return m_calibration->dacBoffset();
	}
}

void M2kImpl::setAdcCalibrationGain(unsigned int chn, double gain)
{
	if (chn >= getAnalogIn()->getNbChannels()) {
		throw_exception(EXC_OUT_OF_RANGE, "No such ADC channel");
	}
	m_calibration->setAdcGain(chn, gain);
}

void M2kImpl::setAdcCalibrationOffset(unsigned int chn, int offset)
{
	if (chn >= getAnalogIn()->getNbChannels()) {
		throw_exception(EXC_OUT_OF_RANGE, "No such ADC channel");
	}
	m_calibration->setAdcOffset(chn, offset);
}

void M2kImpl::setDacCalibrationOffset(unsigned int chn, int offset)
{
	if (chn >= getAnalogOut()->getNbChannels()) {
		throw_exception(EXC_OUT_OF_RANGE, "No such DAC channel");
	}
	m_calibration->setDacOffset(chn, offset);
}

void M2kImpl::setDacCalibrationGain(unsigned int chn, double gain)
{
	if (chn >= getAnalogOut()->getNbChannels()) {
		throw_exception(EXC_OUT_OF_RANGE, "No such DAC channel");
	}
	m_calibration->setDacGain(chn, gain);
}

M2kAnalogIn* M2kImpl::getAnalogIn()
{
	auto aIn = dynamic_cast<libm2k::analog::M2kAnalogIn*>(
				m_instancesAnalogIn.at(0));
	if (aIn) {
		return aIn;
	} else {
		return nullptr;
	}
}

M2kAnalogIn* M2kImpl::getAnalogIn(string dev_name)
{
	for (M2kAnalogIn* d : m_instancesAnalogIn) {
		if (d->getName() == dev_name) {
			libm2k::analog::M2kAnalogIn* analogIn =
					dynamic_cast<libm2k::analog::M2kAnalogIn*>(d);
			if (analogIn) {
				return analogIn;
			}
		}
	}
	return nullptr;
}

M2kPowerSupply* M2kImpl::getPowerSupply()
{
	M2kPowerSupply* pSupply = dynamic_cast<M2kPowerSupply*>(m_instancesPowerSupply.at(0));
	if (!pSupply) {
		throw_exception(EXC_INVALID_PARAMETER, "No M2K power supply");
	}
	return pSupply;
}

M2kDigital* M2kImpl::getDigital()
{
	M2kDigital* logic = dynamic_cast<M2kDigital*>(m_instancesDigital.at(0));
	if (!logic) {
		throw_exception(EXC_INVALID_PARAMETER, "No M2K digital device found");
	}
	return logic;
}

M2kAnalogOut* M2kImpl::getAnalogOut()
{
	if (m_instancesAnalogOut.size() > 0) {
		return m_instancesAnalogOut.at(0);
	}
	return nullptr;
}

std::vector<M2kAnalogIn*> M2kImpl::getAllAnalogIn()
{
	return m_instancesAnalogIn;
}

std::vector<M2kAnalogOut*> M2kImpl::getAllAnalogOut()
{
	return m_instancesAnalogOut;
}

void M2kImpl::initialize()
{
	std::string hw_rev = Utils::getHardwareRevision(m_context);

	std::shared_ptr<DeviceGeneric> m_ad9963 = make_shared<DeviceGeneric>(m_context, "ad9963");
	std::shared_ptr<DeviceGeneric> m_m2k_fabric = make_shared<DeviceGeneric>(m_context, "m2k-fabric");
	unsigned int config1 = 0x05;
	unsigned int config2 = 0x05;

	if (hw_rev == "A") {
		config1 = 0x1B; // IGAIN1 +-6db  0.25db steps
		config2 = 0x1B;
	}

	/* Configure TX path */
	m_ad9963->writeRegister(0x68, config1);
	m_ad9963->writeRegister(0x6B, config2);
	m_ad9963->writeRegister(0x69, 0x1C);  // IGAIN2 +-2.5%
	m_ad9963->writeRegister(0x6C, 0x1C);
	m_ad9963->writeRegister(0x6A, 0x20);  // IRSET +-20%
	m_ad9963->writeRegister(0x6D, 0x20);

	/* Pre-init call to setup M2k */
	m_m2k_fabric->setBoolValue(0, false, "powerdown", false);
	m_m2k_fabric->setBoolValue(1, false, "powerdown", false);
	m_m2k_fabric->setBoolValue(false, "clk_powerdown");
}

void M2kImpl::blinkLed(const double duration, bool blocking)
{
	std::shared_ptr<DeviceGeneric> m_m2k_fabric = make_shared<DeviceGeneric>(m_context, "m2k-fabric");
	if (m_m2k_fabric->getChannel("voltage4", true)->hasAttribute("done_led_overwrite_powerdown")) {
		bool currentValue = m_m2k_fabric->getBoolValue(4, "done_led_overwrite_powerdown", true);

		const double blinkInterval = 0.05;
		double remainingTime = duration;
		std::thread th([=](double remainingTime, double blinkInterval){
			while(remainingTime > 0) {
				bool value;
				value = m_m2k_fabric->getBoolValue(4, "done_led_overwrite_powerdown", true);
				m_m2k_fabric->setBoolValue(4, !value, "done_led_overwrite_powerdown", true);
				remainingTime -= blinkInterval;
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		}, remainingTime, blinkInterval);

		if (blocking) {
			th.join();
		} else if (th.joinable()) {
			th.detach();
		}
		//restore the state of the LED
		m_m2k_fabric->setBoolValue(4, currentValue, "done_led_overwrite_powerdown", true);
	}
}

void M2kImpl::setLed(bool on)
{
	std::shared_ptr<DeviceGeneric> m_m2k_fabric = make_shared<DeviceGeneric>(m_context, "m2k-fabric");
	if (m_m2k_fabric->getChannel("voltage4", true)->hasAttribute("done_led_overwrite_powerdown")) {
		m_m2k_fabric->setBoolValue(4, !on, "done_led_overwrite_powerdown", true);
	}
}

bool M2kImpl::getLed()
{
	bool on = false;
	std::shared_ptr<DeviceGeneric> m_m2k_fabric = make_shared<DeviceGeneric>(m_context, "m2k-fabric");
	if (m_m2k_fabric->getChannel("voltage4", true)->hasAttribute("done_led_overwrite_powerdown")) {
		on = !m_m2k_fabric->getBoolValue(4, "done_led_overwrite_powerdown", true);
	}
	return on;
}
