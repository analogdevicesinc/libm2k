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

#include "context_impl.cpp"
#include <libm2k/m2k.hpp>
#include <libm2k/utils/channel.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <libm2k/m2kexceptions.hpp>
#include <libm2k/m2kcalibration.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/digital/m2kdigital.hpp>
#include <libm2k/utils/utils.hpp>
#include <libm2k/utils/device.hpp>
#include <libm2k/logger.hpp>
#include <iio.h>
#include <iostream>
#include <thread>

using namespace std;
using namespace libm2k::devices;
using namespace libm2k::analog;
using namespace libm2k::digital;
using namespace libm2k::utils;

class M2K::M2KImpl : public ContextImpl {
public:
	M2KImpl(std::string uri, iio_context* ctx, std::string name) :
		ContextImpl(uri, ctx, name)
	{
		std::cout << "I am M2K device " << std::endl;

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

		scanAllAnalogIn();
		scanAllAnalogOut();
		scanAllPowerSupply();
		scanAllDigital();
		m_calibration = new M2kCalibration(ctx, getAnalogIn(), getAnalogOut());
	}

	~M2KImpl()
	{
		std::shared_ptr<Device> m_m2k_fabric = make_shared<Device>(m_context, "m2k-fabric");
		if (m_m2k_fabric) {
			m_m2k_fabric->setBoolValue(0, true, "powerdown", false);
			m_m2k_fabric->setBoolValue(1, true, "powerdown", false);

			/* ADF4360 global clock power down */
			m_m2k_fabric->setBoolValue(true, "powerdown");
		}
		delete m_calibration;

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

	void setTimeout(unsigned int timeout)
	{
		iio_context_set_timeout(m_context, timeout);
	}

	void scanAllAnalogIn()
	{
		Device* aIn = new libm2k::analog::M2kAnalogIn(m_context, "m2k-adc");
		m_instancesAnalogIn.push_back(aIn);
	}

	void scanAllAnalogOut()
	{
		std::vector<std::string> devs = {"m2k-dac-a", "m2k-dac-b"};
		Device* aOut = new libm2k::analog::M2kAnalogOut(m_context, devs);
		m_instancesAnalogOut.push_back(aOut);
	}

	void scanAllPowerSupply()
	{
		Device* pSupply = new libm2k::analog::M2kPowerSupply(m_context, "ad5627", "ad9963");
		m_instancesPowerSupply.push_back(pSupply);
	}

	void scanAllDigital()
	{
		Device* logic = new libm2k::digital::M2kDigital(m_context, "m2k-logic-analyzer");
		m_instancesDigital.push_back(logic);
	}

	void calibrate()
	{

	}

	bool resetCalibration()
	{
		return m_calibration->resetCalibration();
	}

	bool calibrateADC()
	{
		return m_calibration->calibrateADC();
	}

	bool calibrateDAC()
	{
		return m_calibration->calibrateDAC();
	}

	double getAdcCalibrationGain(unsigned int chn)
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

	int getAdcCalibrationOffset(unsigned int chn)
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

	double getDacACalibrationGain()
	{
		return m_calibration->dacAvlsb();
	}

	double getDacBCalibrationGain()
	{
		return m_calibration->dacBvlsb();
	}

	int getDacACalibrationOffset()
	{
		return m_calibration->dacAoffset();
	}

	int getDacBCalibrationOffset()
	{
		return m_calibration->dacBoffset();
	}

	M2kAnalogIn* getAnalogIn()
	{
		auto aIn = dynamic_cast<libm2k::analog::M2kAnalogIn*>(
					m_instancesAnalogIn.at(0));
		if (aIn) {
			return aIn;
		} else {
			return nullptr;
		}
	}

	M2kAnalogIn* getAnalogIn(string dev_name)
	{
		for (Device* d : m_instancesAnalogIn) {
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

	M2kPowerSupply* getPowerSupply()
	{
		M2kPowerSupply* pSupply = dynamic_cast<M2kPowerSupply*>(m_instancesPowerSupply.at(0));
		if (!pSupply) {
			throw_exception(EXC_INVALID_PARAMETER, "No M2K power supply");
		}
		return pSupply;
	}

	M2kDigital* getDigital()
	{
		M2kDigital* logic = dynamic_cast<M2kDigital*>(m_instancesDigital.at(0));
		if (!logic) {
			throw_exception(EXC_INVALID_PARAMETER, "No M2K digital device found");
		}
		return logic;
	}

	M2kAnalogOut* getAnalogOut()
	{
		for (Device* d : m_instancesAnalogOut) {
			libm2k::analog::M2kAnalogOut* analogOut =
					dynamic_cast<M2kAnalogOut*>(d);
			if (analogOut) {
				return analogOut;
			}
		}
		return nullptr;
	}

	std::vector<M2kAnalogIn*> getAllAnalogIn()
	{
		std::vector<libm2k::analog::M2kAnalogIn*> allAnalogIn = {};
		for (Device* inst : m_instancesAnalogIn) {
			libm2k::analog::M2kAnalogIn* analogIn =
					dynamic_cast<M2kAnalogIn*>(inst);
			if (analogIn) {
				allAnalogIn.push_back(analogIn);
			}
		}
		return allAnalogIn;
	}

	std::vector<M2kAnalogOut*> getAllAnalogOut()
	{
		std::vector<libm2k::analog::M2kAnalogOut*> allAnalogOut;
		for (Device* inst : m_instancesAnalogOut) {
			libm2k::analog::M2kAnalogOut* analogOut =
					dynamic_cast<libm2k::analog::M2kAnalogOut*>(inst);
			if (analogOut) {
				allAnalogOut.push_back(analogOut);
			}
		}
		return allAnalogOut;
	}

	void initialize()
	{
		std::string hw_rev = Utils::getHardwareRevision(m_context);

		std::shared_ptr<Device> m_ad9963 = make_shared<Device>(m_context, "ad9963");
		std::shared_ptr<Device> m_m2k_fabric = make_shared<Device>(m_context, "m2k-fabric");
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

	void blinkLed(const double duration = 4, bool blocking = false)
	{
		std::shared_ptr<Device> m_m2k_fabric = make_shared<Device>(m_context, "m2k-fabric");
		if (m_m2k_fabric->getChannel("voltage4")->hasAttribute("done_led_overwrite_powerdown")) {
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

	void setLed(bool on)
	{
		std::shared_ptr<Device> m_m2k_fabric = make_shared<Device>(m_context, "m2k-fabric");
		if (m_m2k_fabric->getChannel("voltage4")->hasAttribute("done_led_overwrite_powerdown")) {
			m_m2k_fabric->setBoolValue(4, !on, "done_led_overwrite_powerdown", true);
		}
	}

	bool getLed()
	{
		bool on = false;
		std::shared_ptr<Device> m_m2k_fabric = make_shared<Device>(m_context, "m2k-fabric");
		if (m_m2k_fabric->getChannel("voltage4")->hasAttribute("done_led_overwrite_powerdown")) {
			on = !m_m2k_fabric->getBoolValue(4, "done_led_overwrite_powerdown", true);
		}
		return on;
	}

private:
	M2kCalibration* m_calibration;
	std::vector<utils::Device*> m_instancesAnalogOut;
	std::vector<utils::Device*> m_instancesAnalogIn;
	std::vector<utils::Device*> m_instancesPowerSupply;
	std::vector<utils::Device*> m_instancesDigital;
};
