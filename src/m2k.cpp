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

#include "private/m2k_impl.cpp"

using namespace std;
using namespace libm2k::devices;
using namespace libm2k::analog;
using namespace libm2k::digital;
using namespace libm2k::utils;

M2k::M2k(std::string uri, iio_context* ctx, std::string name, bool sync) :
	Context(new M2kImpl(uri, ctx, name, sync))
{
	m_pimpl = dynamic_pointer_cast<M2kImpl>(Context::m_pimpl);
}

M2k::~M2k()
{
}

void M2k::init()
{
	m_pimpl->init();
}

void M2k::deinitialize()
{
	m_pimpl->deinitialize();
}

void M2k::setTimeout(unsigned int timeout)
{
	m_pimpl->setTimeout(timeout);
}

void M2k::setLed(bool on)
{
	m_pimpl->setLed(on);
}

bool M2k::getLed()
{
	return m_pimpl->getLed();
}

void M2k::scanAllAnalogIn()
{
	m_pimpl->scanAllAnalogIn();
}

void M2k::scanAllAnalogOut()
{
	m_pimpl->scanAllAnalogOut();
}

void M2k::scanAllPowerSupply()
{
	m_pimpl->scanAllPowerSupply();
}

void M2k::scanAllDigital()
{
	m_pimpl->scanAllDigital();
}

void M2k::calibrate()
{
	m_pimpl->calibrate();
}

bool M2k::resetCalibration()
{
	return m_pimpl->resetCalibration();
}

bool M2k::calibrateADC()
{
	return m_pimpl->calibrateADC();
}

bool M2k::calibrateDAC()
{
	return m_pimpl->calibrateDAC();
}

double M2k::getAdcCalibrationGain(unsigned int chn)
{
	return m_pimpl->getAdcCalibrationGain(chn);
}

int M2k::getAdcCalibrationOffset(unsigned int chn)
{
	return m_pimpl->getAdcCalibrationOffset(chn);
}

double M2k::getDacACalibrationGain()
{
	return m_pimpl->getDacACalibrationGain();
}

double M2k::getDacBCalibrationGain()
{
	return m_pimpl->getDacBCalibrationGain();
}

int M2k::getDacACalibrationOffset()
{
	return m_pimpl->getDacACalibrationOffset();
}

int M2k::getDacBCalibrationOffset()
{
	return m_pimpl->getDacBCalibrationOffset();
}

M2kAnalogIn* M2k::getAnalogIn()
{
	return m_pimpl->getAnalogIn();
}

M2kAnalogIn* M2k::getAnalogIn(string dev_name)
{
	return m_pimpl->getAnalogIn(dev_name);
}

M2kPowerSupply* M2k::getPowerSupply()
{
	return m_pimpl->getPowerSupply();
}

M2kDigital* M2k::getDigital()
{
	return m_pimpl->getDigital();
}

M2kAnalogOut* M2k::getAnalogOut()
{
	return m_pimpl->getAnalogOut();
}

std::vector<M2kAnalogIn*> M2k::getAllAnalogIn()
{
	return m_pimpl->getAllAnalogIn();
}

std::vector<M2kAnalogOut*> M2k::getAllAnalogOut()
{
	return m_pimpl->getAllAnalogOut();
}
