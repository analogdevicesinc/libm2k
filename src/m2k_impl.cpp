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
#include <digital/m2kdigital_impl.hpp>
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
#include <algorithm>
#include <thread>

using namespace std;
using namespace libm2k::context;
using namespace libm2k::analog;
using namespace libm2k::digital;
using namespace libm2k::utils;

constexpr int defaultOffset = 2048;
constexpr double defaultGain = 1;

M2kImpl::M2kImpl(std::string uri, iio_context* ctx, std::string name, bool sync) :
	ContextImpl(uri, ctx, name, sync),
	m_sync(sync)
{
	initialize();
	setTimeout(0);

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
		THROW_M2K_EXCEPTION("Can't instantiate M2K board; M2K trigger is invalid.", libm2k::EXC_INVALID_PARAMETER);
	}

	scanAllAnalogIn();
	scanAllAnalogOut();
	scanAllPowerSupply();
	scanAllDigital();
	m_calibration = new M2kCalibrationImpl(ctx, getAnalogIn(), getAnalogOut());
}

M2kImpl::~M2kImpl()
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

void M2kImpl::deinitialize()
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

	auto ain_impl = dynamic_cast<M2kAnalogInImpl*>(getAnalogIn());
	if (ain_impl) {
		ain_impl->deinitialize();
	}
	auto aout_impl = dynamic_cast<M2kAnalogOutImpl*>(getAnalogOut());
	if (aout_impl) {
		aout_impl->deinitialize();
	}
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
	libm2k::analog::M2kPowerSupply* pSupply = new libm2k::analog::M2kPowerSupplyImpl(m_context,
						m_context_attributes, "ad5627", "ad9963", m_sync);
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
	LIBM2K_LOG(INFO, "[BEGIN] Calibrate ADC");
	const bool calibrationResult = m_calibration->calibrateADC();
	LIBM2K_LOG(INFO, "[END] Calibrate ADC");
	return calibrationResult;
}

bool M2kImpl::calibrateDAC()
{
	LIBM2K_LOG(INFO, "[BEGIN] Calibrate DAC");
	bool calibrationResult = m_calibration->calibrateDAC();
	LIBM2K_LOG(INFO, "[END] Calibrate DAC");
	return calibrationResult;
}

double M2kImpl::getAdcCalibrationGain(unsigned int chn)
{
	return m_calibration->getAdcGain(chn);
}

int M2kImpl::getAdcCalibrationOffset(unsigned int chn)
{
	return m_calibration->getAdcOffset(chn);
}

double M2kImpl::getDacCalibrationGain(unsigned int chn)
{
	return m_calibration->getDacGain(chn);
}

int M2kImpl::getDacCalibrationOffset(unsigned int chn)
{
	return m_calibration->getDacOffset(chn);
}

void M2kImpl::setAdcCalibrationGain(unsigned int chn, double gain)
{
	if (chn >= getAnalogIn()->getNbChannels()) {
		THROW_M2K_EXCEPTION("No such ADC channel", libm2k::EXC_OUT_OF_RANGE);
	}
	m_calibration->setAdcGain(chn, gain);
}

void M2kImpl::setAdcCalibrationOffset(unsigned int chn, int offset)
{
	if (chn >= getAnalogIn()->getNbChannels()) {
		THROW_M2K_EXCEPTION("No such ADC channel", libm2k::EXC_OUT_OF_RANGE);
	}
	m_calibration->setAdcOffset(chn, offset);
}

void M2kImpl::setDacCalibrationOffset(unsigned int chn, int offset)
{
	if (chn >= getAnalogOut()->getNbChannels()) {
		THROW_M2K_EXCEPTION("No such DAC channel", libm2k::EXC_OUT_OF_RANGE);
	}
	m_calibration->setDacOffset(chn, offset);
}

void M2kImpl::setDacCalibrationGain(unsigned int chn, double gain)
{
	if (chn >= getAnalogOut()->getNbChannels()) {
		THROW_M2K_EXCEPTION("No such DAC channel", libm2k::EXC_OUT_OF_RANGE);
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
		THROW_M2K_EXCEPTION("No M2K power supply", libm2k::EXC_INVALID_PARAMETER);
	}
	return pSupply;
}

M2kDigital* M2kImpl::getDigital()
{
	M2kDigital* logic = dynamic_cast<M2kDigital*>(m_instancesDigital.at(0));
	if (!logic) {
		THROW_M2K_EXCEPTION("No M2K digital device found", libm2k::EXC_INVALID_PARAMETER);
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

bool M2kImpl::hasMixedSignal()
{
	auto digital_impl = dynamic_cast<M2kDigitalImpl*>(getDigital());
	return digital_impl->hasRateMux();
}

void M2kImpl::startMixedSignalAcquisition(unsigned int nb_samples)
{
	LIBM2K_LOG(INFO, "[BEGIN] Start mixed signal acquisition");
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
	LIBM2K_LOG(INFO, "[END] Start mixed signal acquisition");
}

void M2kImpl::stopMixedSignalAcquisition()
{
	LIBM2K_LOG(INFO, "[BEGIN] Stop mixed signal acquisition");
	for (auto analogIn : m_instancesAnalogIn) {
		analogIn->stopAcquisition();
	}
	for (auto digital : m_instancesDigital) {
		digital->stopAcquisition();
	}
	m_instancesDigital.at(0)->resetRateMux();
	m_trigger->setAnalogSource(analogSource);
	m_trigger->setDigitalSource(digitalSource);
	LIBM2K_LOG(INFO, "[END] Stop mixed signal acquisition");
}

bool M2kImpl::hasAnalogTrigger()
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

bool M2kImpl::hasDigitalTrigger()
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

string M2kImpl::getFirmwareVersion()
{
	std::string tmp = ContextImpl::getFirmwareVersion();
	if (tmp == "") {
		auto allDevs = getAllDevices();
		auto it = std::find(allDevs.begin(), allDevs.end(), "m2k-adc");
		if (it == allDevs.end()) {
			THROW_M2K_EXCEPTION("Can't determine firmware version.", libm2k::EXC_INVALID_FIRMWARE_VERSION);
		} else {
			// We got the m2k-adc device and no fw_version, so we have the M2K FMC version
			tmp = "v0.28";
		}
	}
	return tmp;
}

bool M2kImpl::hasContextCalibration()
{
	std::string values;
	double temperature;

	auto it_attr = m_context_attributes.find("cal,temp_lut");
	if (it_attr == m_context_attributes.end()){
		return false;
	}
	values = it_attr->second;

	if (!m_calibration_lut.empty()) {
		return true;
	}
	auto splitValues = Utils::split(values, ",");
	if (splitValues.size() % 9 != 0) {
		return false;
	}

	auto it = splitValues.begin();
	while (it != splitValues.end()) {
		try {
			temperature = Utils::safeStod(*it);
			auto parameters = std::make_shared<struct CALIBRATION_PARAMETERS>();
			it = std::next(it);
			parameters->adc_offset_ch_1 = std::stoi(*it);
			it = std::next(it);
			parameters->adc_offset_ch_2 = std::stoi(*it);
			it = std::next(it);
			parameters->adc_gain_ch_1 = Utils::safeStod(*it);
			it = std::next(it);
			parameters->adc_gain_ch_2 = Utils::safeStod(*it);
			it = std::next(it);
			parameters->dac_a_offset = std::stoi(*it);
			it = std::next(it);
			parameters->dac_b_offset = std::stoi(*it);
			it = std::next(it);
			parameters->dac_a_gain = Utils::safeStod(*it);
			it = std::next(it);
			parameters->dac_b_gain = Utils::safeStod(*it);
			it = std::next(it);

			m_calibration_lut.insert({temperature, parameters});
		} catch (const std::exception&) {
			return false;
		}
	}
	if (m_calibration_lut.empty()) {
		return false;
	}
	return true;
}

std::map<double, std::shared_ptr<struct CALIBRATION_PARAMETERS>> &M2kImpl::getLUT()
{
	return m_calibration_lut;
}

bool M2kImpl::isCalibrated()
{
	int diff = Utils::compareVersions(m_firmware_version, "v0.25");
	if (diff <= 0) {
		// for FW 0.25 and earlier we cannot conclude if the adc/dac was calibrated due to missing calibbias attribute
		return m_calibration->getAdcCalibrated() && m_calibration->getDacCalibrated();
	}

	for (unsigned int i = 0; i < 2; i++) {
		if (m_calibration->getAdcOffset(i) != defaultOffset || m_calibration->getDacOffset(i) != defaultOffset) {
			return true;
		}

		if (m_calibration->getAdcGain(i) != defaultGain || m_calibration->getDacGain(i) != defaultGain) {
			return true;
		}
	}
	return false;
}

double M2kImpl::getCalibrationTemperature(double temperature)
{
	std::map<double, shared_ptr<struct CALIBRATION_PARAMETERS>>::iterator low, prev;

	if (m_calibration_lut.empty()) {
		if (!hasContextCalibration()) {
		        THROW_M2K_EXCEPTION("Calibration from context is unavailable", libm2k::EXC_RUNTIME_ERROR);
		}
	}

	low = m_calibration_lut.lower_bound(temperature);
	if (low == m_calibration_lut.end()) {
		return m_calibration_lut.rbegin()->first;
	}
	if (low == m_calibration_lut.begin()) {
		return low->first;
	}
	prev = std::prev(low);
	if ((temperature - prev->first) < (low->first - temperature)) {
		return prev->first;
	}
	return low->first;
}

double M2kImpl::calibrateFromContext()
{
	LIBM2K_LOG(INFO, "[BEGIN] Calibrate from context");
	double temperature = getDMM("ad9963")->readChannel("temp0").value;
	double calibrationTemperature = getCalibrationTemperature(temperature);
	auto it = m_calibration_lut.find(calibrationTemperature);
	if (it == m_calibration_lut.end()) {
		THROW_M2K_EXCEPTION("Calibration from context is unavailable", libm2k::EXC_RUNTIME_ERROR);
	}
	auto calibrationParameters = it->second;

	m_calibration->setAdcOffset(0, calibrationParameters->adc_offset_ch_1);
	m_calibration->setAdcOffset(1, calibrationParameters->adc_offset_ch_2);
	m_calibration->setAdcGain(0, calibrationParameters->adc_gain_ch_1);
	m_calibration->setAdcGain(1, calibrationParameters->adc_gain_ch_2);
	m_calibration->setDacOffset(0, calibrationParameters->dac_a_offset);
	m_calibration->setDacOffset(1, calibrationParameters->dac_b_offset);
	m_calibration->setDacGain(0, calibrationParameters->dac_a_gain);
	m_calibration->setDacGain(1, calibrationParameters->dac_b_gain);
	LIBM2K_LOG(INFO, "[END] Calibrate from context");
	return calibrationTemperature;
}
