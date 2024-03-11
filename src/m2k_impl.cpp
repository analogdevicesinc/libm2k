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
#include "m2k_impl_private.hpp"
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

M2kImpl::M2kImpl(M2kImplPrivate *p) :
	ContextImpl(p)
{
	m_privateM2K = p;
}

M2kImpl::~M2kImpl()
{
	// delete m_calibration;

	// if (m_trigger) {
	// 	delete m_trigger;
	// }

	// for (auto ain : m_instancesAnalogIn) {
	// 	delete ain;
	// }
	// for (auto aout : m_instancesAnalogOut) {
	// 	delete aout;
	// }
	// for (auto ps : m_instancesPowerSupply) {
	// 	delete ps;
	// }
	// for (auto d : m_instancesDigital) {
	// 	delete d;
	// }
	// m_instancesAnalogIn.clear();
	// m_instancesAnalogOut.clear();
	// m_instancesPowerSupply.clear();
}

void M2kImpl::deinitialize()
{
	m_privateM2K->deinitialize();
}

void M2kImpl::reset()
{
	m_privateM2K->reset();
}

void M2kImpl::scanAllAnalogIn()
{
	m_privateM2K->scanAllAnalogIn();
}

void M2kImpl::scanAllAnalogOut()
{
	m_privateM2K->scanAllAnalogOut();
}

void M2kImpl::scanAllPowerSupply()
{
	m_privateM2K->scanAllPowerSupply();
}

void M2kImpl::scanAllDigital()
{
	m_privateM2K->scanAllDigital();
}

bool M2kImpl::calibrate()
{
	return m_privateM2K->calibrate();
}

bool M2kImpl::resetCalibration()
{
	return m_privateM2K->resetCalibration();
}

double M2kImpl::calibrateFromContext()
{
	return m_privateM2K->calibrateFromContext();
}

bool M2kImpl::calibrateADC()
{
	return m_privateM2K->calibrateADC();
}

bool M2kImpl::calibrateDAC()
{
	return m_privateM2K->calibrateDAC();
}

double M2kImpl::getAdcCalibrationGain(unsigned int chn)
{
	return m_privateM2K->getAdcCalibrationGain(chn);
}

int M2kImpl::getAdcCalibrationOffset(unsigned int chn)
{
	return m_privateM2K->getAdcCalibrationOffset(chn);
}

double M2kImpl::getDacCalibrationGain(unsigned int chn)
{
	return m_privateM2K->getDacCalibrationGain(chn);
}

int M2kImpl::getDacCalibrationOffset(unsigned int chn)
{
	return m_privateM2K->getDacCalibrationOffset(chn);
}

void M2kImpl::setAdcCalibrationGain(unsigned int chn, double gain)
{
	m_privateM2K->setAdcCalibrationGain(chn, gain);
}

bool M2kImpl::hasContextCalibration()
{
	return m_privateM2K->hasContextCalibration();
}

std::map<double, std::shared_ptr<CALIBRATION_PARAMETERS> > &M2kImpl::getLUT()
{
	return m_privateM2K->getLUT();
}

bool M2kImpl::isCalibrated()
{
	return m_privateM2K->isCalibrated();
}

void M2kImpl::setLed(bool on)
{
	m_privateM2K->setLed(on);
}

bool M2kImpl::getLed()
{
	return m_privateM2K->getLed();
}

string M2kImpl::getFirmwareVersion()
{
	return m_privateM2K->getFirmwareVersion();
}

void M2kImpl::setAdcCalibrationOffset(unsigned int chn, int offset)
{
	m_privateM2K->setAdcCalibrationOffset(chn, offset);
}

void M2kImpl::setDacCalibrationOffset(unsigned int chn, int offset)
{
	m_privateM2K->setDacCalibrationOffset(chn, offset);
}

void M2kImpl::setDacCalibrationGain(unsigned int chn, double gain)
{
	m_privateM2K->setDacCalibrationGain(chn, gain);
}

M2kAnalogIn* M2kImpl::getAnalogIn()
{
	return m_privateM2K->getAnalogIn();
}

M2kAnalogIn* M2kImpl::getAnalogIn(string dev_name)
{
	return m_privateM2K->getAnalogIn(dev_name);
}

M2kPowerSupply* M2kImpl::getPowerSupply()
{
	return m_privateM2K->getPowerSupply();
}

M2kDigital* M2kImpl::getDigital()
{
	return m_privateM2K->getDigital();
}

M2kAnalogOut* M2kImpl::getAnalogOut()
{
	return m_privateM2K->getAnalogOut();
}

std::vector<M2kAnalogIn*> M2kImpl::getAllAnalogIn()
{
	return m_privateM2K->getAllAnalogIn();
}

std::vector<M2kAnalogOut*> M2kImpl::getAllAnalogOut()
{
	return m_privateM2K->getAllAnalogOut();
}

bool M2kImpl::hasMixedSignal()
{
	return m_privateM2K->hasMixedSignal();
}

void M2kImpl::startMixedSignalAcquisition(unsigned int nb_samples)
{
	m_privateM2K->startMixedSignalAcquisition(nb_samples);
}

void M2kImpl::stopMixedSignalAcquisition()
{
	m_privateM2K->stopMixedSignalAcquisition();
}
