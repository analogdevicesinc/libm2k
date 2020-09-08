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

#include "m2k_fake_impl.hpp"
#include "fake/m2kanalogout_fake_impl.hpp"
#include "fake/m2kanalogin_fake_impl.hpp"
#include "fake/m2kpowersupply_fake_impl.hpp"
#include <fake/m2kdigital_fake_impl.hpp>
#include "fake/m2khardwaretrigger_fake_impl.hpp"
#include "fake/m2khardwaretrigger_v0.24_fake_impl.hpp"
#include "m2kcalibration_fake_impl.hpp"
#include <libm2k/analog/dmm.hpp>
#include "utils/channel.hpp"
#include <libm2k/m2kexceptions.hpp>
#include "fake/m2kdigital_fake_impl.hpp"
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

constexpr int defaultOffset = 2048;
constexpr double defaultGain = 1;

M2kFakeImpl::M2kFakeImpl(std::string uri, iio_context* ctx, std::string name, bool sync) :
	ContextImpl(uri, ctx, name, sync),
	m_sync(sync)
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
		m_trigger = new M2kFakeHardwareTriggerImpl(ctx);
	} else {
		m_trigger = new M2kFakeHardwareTriggerV024Impl(ctx);
	}

	if (!m_trigger) {
		THROW_M2K_EXCEPTION("Can't instantiate M2K board; M2K trigger is invalid.", libm2k::EXC_INVALID_PARAMETER);
	}

	scanAllAnalogIn();
	scanAllAnalogOut();
	scanAllPowerSupply();
	scanAllDigital();
	m_calibration = new M2kFakeCalibrationImpl(ctx, getAnalogIn(), getAnalogOut());
}

M2kFakeImpl::~M2kFakeImpl()
{
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

void M2kFakeImpl::deinitialize()
{
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

	auto ain_impl = dynamic_cast<M2kFakeAnalogInImpl*>(getAnalogIn());
	if (ain_impl) {
		ain_impl->deinitialize();
	}
	auto aout_impl = dynamic_cast<M2kFakeAnalogOutImpl*>(getAnalogOut());
	if (aout_impl) {
		aout_impl->deinitialize();
	}
}

void M2kFakeImpl::reset()
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

void M2kFakeImpl::scanAllAnalogIn()
{
	M2kAnalogIn* aIn = new libm2k::analog::M2kFakeAnalogInImpl(m_context, "m2k-adc", m_sync, m_trigger);
	m_instancesAnalogIn.push_back(aIn);
}

void M2kFakeImpl::scanAllAnalogOut()
{
	std::vector<std::string> devs = {"m2k-dac-a", "m2k-dac-b"};
	M2kAnalogOut* aOut = new libm2k::analog::M2kFakeAnalogOutImpl(m_context, devs, m_sync);
	m_instancesAnalogOut.push_back(aOut);
}

void M2kFakeImpl::scanAllPowerSupply()
{
	libm2k::analog::M2kPowerSupply* pSupply = new libm2k::analog::M2kFakePowerSupplyImpl(m_context, "ad5627", "ad9963", m_sync);
	m_instancesPowerSupply.push_back(pSupply);
}

void M2kFakeImpl::scanAllDigital()
{
	libm2k::digital::M2kDigital* logic = new libm2k::digital::M2kFakeDigitalImpl(m_context, "m2k-logic-analyzer", m_sync, m_trigger);
	m_instancesDigital.push_back(logic);
}

bool M2kFakeImpl::calibrate()
{
	bool ok_adc, ok_dac = false;
	ok_adc = calibrateADC();
	ok_dac = calibrateDAC();
	return ok_adc && ok_dac;
}

bool M2kFakeImpl::resetCalibration()
{
	return m_calibration->resetCalibration();
}

bool M2kFakeImpl::calibrateADC()
{
	return m_calibration->calibrateADC();
}

bool M2kFakeImpl::calibrateDAC()
{
	return m_calibration->calibrateDAC();
}

double M2kFakeImpl::getAdcCalibrationGain(unsigned int chn)
{
	return m_calibration->getAdcGain(chn);
}

int M2kFakeImpl::getAdcCalibrationOffset(unsigned int chn)
{
	return m_calibration->getAdcOffset(chn);
}

double M2kFakeImpl::getDacCalibrationGain(unsigned int chn)
{
	return m_calibration->getDacGain(chn);
}

int M2kFakeImpl::getDacCalibrationOffset(unsigned int chn)
{
	return m_calibration->getDacOffset(chn);
}

void M2kFakeImpl::setAdcCalibrationGain(unsigned int chn, double gain)
{
	if (chn >= getAnalogIn()->getNbChannels()) {
		THROW_M2K_EXCEPTION("No such ADC channel", libm2k::EXC_OUT_OF_RANGE);
	}
	m_calibration->setAdcGain(chn, gain);
}

void M2kFakeImpl::setAdcCalibrationOffset(unsigned int chn, int offset)
{
	if (chn >= getAnalogIn()->getNbChannels()) {
		THROW_M2K_EXCEPTION("No such ADC channel", libm2k::EXC_OUT_OF_RANGE);
	}
	m_calibration->setAdcOffset(chn, offset);
}

void M2kFakeImpl::setDacCalibrationOffset(unsigned int chn, int offset)
{
	if (chn >= getAnalogOut()->getNbChannels()) {
		THROW_M2K_EXCEPTION("No such DAC channel", libm2k::EXC_OUT_OF_RANGE);
	}
	m_calibration->setDacOffset(chn, offset);
}

void M2kFakeImpl::setDacCalibrationGain(unsigned int chn, double gain)
{
	if (chn >= getAnalogOut()->getNbChannels()) {
		THROW_M2K_EXCEPTION("No such DAC channel", libm2k::EXC_OUT_OF_RANGE);
	}
	m_calibration->setDacGain(chn, gain);
}

M2kAnalogIn* M2kFakeImpl::getAnalogIn()
{
	auto aIn = dynamic_cast<libm2k::analog::M2kAnalogIn*>(
				m_instancesAnalogIn.at(0));
	if (aIn) {
		return aIn;
	} else {
		return nullptr;
	}
}

M2kAnalogIn* M2kFakeImpl::getAnalogIn(string dev_name)
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

M2kPowerSupply* M2kFakeImpl::getPowerSupply()
{
	M2kPowerSupply* pSupply = dynamic_cast<M2kPowerSupply*>(m_instancesPowerSupply.at(0));
	if (!pSupply) {
		THROW_M2K_EXCEPTION("No M2K power supply", libm2k::EXC_INVALID_PARAMETER);
	}
	return pSupply;
}

M2kDigital* M2kFakeImpl::getDigital()
{
	M2kDigital* logic = dynamic_cast<M2kDigital*>(m_instancesDigital.at(0));
	if (!logic) {
		THROW_M2K_EXCEPTION("No M2K digital device found", libm2k::EXC_INVALID_PARAMETER);
	}
	return logic;
}

M2kAnalogOut* M2kFakeImpl::getAnalogOut()
{
	if (m_instancesAnalogOut.size() > 0) {
		return m_instancesAnalogOut.at(0);
	}
	return nullptr;
}

std::vector<M2kAnalogIn*> M2kFakeImpl::getAllAnalogIn()
{
	return m_instancesAnalogIn;
}

std::vector<M2kAnalogOut*> M2kFakeImpl::getAllAnalogOut()
{
	return m_instancesAnalogOut;
}

bool M2kFakeImpl::hasMixedSignal()
{
	auto digital_impl = dynamic_cast<M2kFakeDigitalImpl*>(getDigital());
	return digital_impl->hasRateMux();
}

void M2kFakeImpl::startMixedSignalAcquisition(unsigned int nb_samples)
{
	const bool hasAnalogTrigger = this->hasAnalogTrigger();
	const bool hasDigitalTrigger = this->hasDigitalTrigger();

	analogSource = m_trigger->getAnalogSource();
	digitalSource = m_trigger->getDigitalSource();

	if (!hasAnalogTrigger && !hasDigitalTrigger) {
		// no trigger
		m_trigger->setAnalogSource(NO_SOURCE);
		m_trigger->setDigitalSource(SRC_ANALOG_IN);
	} else if (!hasDigitalTrigger) {
		// analog trigger
		m_trigger->setAnalogSource(NO_SOURCE);
		m_trigger->setDigitalSource(SRC_ANALOG_IN);
	} else if (!hasAnalogTrigger) {
		// digital trigger
		m_trigger->setDigitalSource(SRC_DISABLED);
		m_trigger->setAnalogSource(SRC_DIGITAL_IN);
	}

	// share the same rate
	m_instancesDigital.at(0)->setRateMux();

	// start acquisition
	for (auto analogIn : m_instancesAnalogIn) {
		analogIn->startAcquisition(nb_samples);
	}
	for (auto digital : m_instancesDigital) {
		digital->startAcquisition(nb_samples);
	}

	// release the trigger
	if (!hasAnalogTrigger && !hasDigitalTrigger) {
		m_trigger->setAnalogMode(CHANNEL_1, ALWAYS);
		m_trigger->setAnalogSource(CHANNEL_1);
	} else if (!hasDigitalTrigger) {
		m_trigger->setAnalogSource(analogSource);
	} else if (!hasAnalogTrigger) {
		m_trigger->setDigitalSource(digitalSource);
	}
}

void M2kFakeImpl::stopMixedSignalAcquisition()
{
	for (auto analogIn : m_instancesAnalogIn) {
		analogIn->stopAcquisition();
	}
	for (auto digital : m_instancesDigital) {
		digital->stopAcquisition();
	}
	m_instancesDigital.at(0)->resetRateMux();
	m_trigger->setAnalogSource(analogSource);
	m_trigger->setDigitalSource(digitalSource);
}

bool M2kFakeImpl::hasAnalogTrigger()
{
	enum M2K_TRIGGER_SOURCE_ANALOG source;
	enum M2K_TRIGGER_MODE mode;

	source = m_trigger->getAnalogSource();
	switch (source) {
	case CHANNEL_1:
	case CHANNEL_2:
		mode = m_trigger->getAnalogMode(source);
		if (mode != ALWAYS) {
			return true;
		}
		break;
	case CHANNEL_1_OR_CHANNEL_2:
	case CHANNEL_1_AND_CHANNEL_2:
	case CHANNEL_1_XOR_CHANNEL_2:
		return m_trigger->getAnalogMode(CHANNEL_1) || m_trigger->getAnalogMode(CHANNEL_2);
	case SRC_DIGITAL_IN:
		return false;
	case CHANNEL_1_OR_SRC_LOGIC_ANALYZER:
		if (m_trigger->getDigitalSource() == SRC_ANALOG_IN) {
			return true;
		}
		mode = m_trigger->getAnalogMode(CHANNEL_1);
		return mode || hasDigitalTrigger();
	case CHANNEL_2_OR_SRC_LOGIC_ANALYZER:
		if (m_trigger->getDigitalSource() == SRC_ANALOG_IN) {
			return true;
		}
		mode = m_trigger->getAnalogMode(CHANNEL_2);
		return mode || hasDigitalTrigger();
	case CHANNEL_1_OR_CHANNEL_2_OR_SRC_LOGIC_ANALYZER:
		if (m_trigger->getDigitalSource() == SRC_ANALOG_IN) {
			return true;
		}
		return m_trigger->getAnalogMode(CHANNEL_1) || m_trigger->getAnalogMode(CHANNEL_2) || hasDigitalTrigger();
	case NO_SOURCE:
		return false;
	}
	return false;
}

bool M2kFakeImpl::hasDigitalTrigger()
{
	enum M2K_TRIGGER_SOURCE_DIGITAL source;
	enum M2K_TRIGGER_CONDITION_DIGITAL condition;
	unsigned int ch_index;

	source = m_trigger->getDigitalSource();
	switch (source) {
	case SRC_NONE:
		for (auto digital : m_instancesDigital) {
			for (ch_index = 0; ch_index < digital->getNbChannelsIn(); ch_index++) {
				condition = m_trigger->getDigitalCondition(ch_index);
				if (condition != NO_TRIGGER_DIGITAL) {
					return true;
				}
			}
		}
		return false;
	case SRC_ANALOG_IN:
		return false;
	case SRC_TRIGGER_IN:
		return true;
	case SRC_DISABLED:
		return false;
	}
	return false;
}

void M2kFakeImpl::initialize()
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
	/*m_ad9963->writeRegister(0x68, config1);
	m_ad9963->writeRegister(0x6B, config2);
	m_ad9963->writeRegister(0x69, 0x1C);  // IGAIN2 +-2.5%
	m_ad9963->writeRegister(0x6C, 0x1C);
	m_ad9963->writeRegister(0x6A, 0x20);  // IRSET +-20%
	m_ad9963->writeRegister(0x6D, 0x20);*/

	/* Pre-init call to setup M2k */
	/*m_m2k_fabric->setBoolValue(0, false, "powerdown", false);
	m_m2k_fabric->setBoolValue(1, false, "powerdown", false);
	m_m2k_fabric->setBoolValue(false, "clk_powerdown");*/
}

void M2kFakeImpl::blinkLed(const double duration, bool blocking)
{
/*
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
	*/
}

void M2kFakeImpl::setLed(bool on)
{
	/*std::shared_ptr<DeviceGeneric> m_m2k_fabric = make_shared<DeviceGeneric>(m_context, "m2k-fabric");
	if (m_m2k_fabric->getChannel("voltage4", true)->hasAttribute("done_led_overwrite_powerdown")) {
		m_m2k_fabric->setBoolValue(4, !on, "done_led_overwrite_powerdown", true);
	}*/
}

bool M2kFakeImpl::getLed()
{
	/*bool on = false;
	std::shared_ptr<DeviceGeneric> m_m2k_fabric = make_shared<DeviceGeneric>(m_context, "m2k-fabric");
	if (m_m2k_fabric->getChannel("voltage4", true)->hasAttribute("done_led_overwrite_powerdown")) {
		on = !m_m2k_fabric->getBoolValue(4, "done_led_overwrite_powerdown", true);
	}
	return on;*/
	return true;
}

bool M2kFakeImpl::isCalibrated()
{
	/*for (unsigned int i = 0; i < 2; i++) {
		if (m_calibration->getAdcOffset(i) != defaultOffset || m_calibration->getDacOffset(i) != defaultOffset) {
			return true;
		}

		if (m_calibration->getAdcGain(i) != defaultGain || m_calibration->getDacGain(i) != defaultGain) {
			return true;
		}
	}
	return false;*/
	return true;
}
