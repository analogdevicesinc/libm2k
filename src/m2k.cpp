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

M2K::M2K(std::string uri, iio_context* ctx, std::string name) :
	Context(new M2KImpl(uri, ctx, name))
{
	m_pimpl = dynamic_pointer_cast<M2KImpl>(Context::m_pimpl);
}

M2K::~M2K()
{
}

void M2K::setTimeout(unsigned int timeout)
{
	m_pimpl->setTimeout(timeout);
}

void M2K::setLed(bool on)
{
	m_pimpl->setLed(on);
}

bool M2K::getLed()
{
	return m_pimpl->getLed();
}

void M2K::scanAllAnalogIn()
{
	m_pimpl->scanAllAnalogIn();
}

void M2K::scanAllAnalogOut()
{
	m_pimpl->scanAllAnalogOut();
}

void M2K::scanAllPowerSupply()
{
	m_pimpl->scanAllPowerSupply();
}

void M2K::scanAllDigital()
{
	m_pimpl->scanAllDigital();
}

void M2K::calibrate()
{
	m_pimpl->calibrate();
}

bool M2K::resetCalibration()
{
	return m_pimpl->resetCalibration();
}

bool M2K::calibrateADC()
{
	return m_pimpl->calibrateADC();
}

bool M2K::calibrateDAC()
{
	return m_pimpl->calibrateDAC();
}

double M2K::getAdcCalibrationGain(unsigned int chn)
{
	return m_pimpl->getAdcCalibrationGain(chn);
}

int M2K::getAdcCalibrationOffset(unsigned int chn)
{
	return m_pimpl->getAdcCalibrationOffset(chn);
}

double M2K::getDacACalibrationGain()
{
	return m_pimpl->getDacACalibrationGain();
}

double M2K::getDacBCalibrationGain()
{
	return m_pimpl->getDacBCalibrationGain();
}

int M2K::getDacACalibrationOffset()
{
	return m_pimpl->getDacACalibrationOffset();
}

int M2K::getDacBCalibrationOffset()
{
	return m_pimpl->getDacBCalibrationOffset();
}

M2kAnalogIn* M2K::getAnalogIn()
{
	return m_pimpl->getAnalogIn();
}

M2kAnalogIn* M2K::getAnalogIn(string dev_name)
{
	return m_pimpl->getAnalogIn(dev_name);
}

M2kPowerSupply* M2K::getPowerSupply()
{
	return m_pimpl->getPowerSupply();
}

M2kDigital* M2K::getDigital()
{
	return m_pimpl->getDigital();
}

M2kAnalogOut* M2K::getAnalogOut()
{
	return m_pimpl->getAnalogOut();
}

std::vector<M2kAnalogIn*> M2K::getAllAnalogIn()
{
	return m_pimpl->getAllAnalogIn();
}

std::vector<M2kAnalogOut*> M2K::getAllAnalogOut()
{
	return m_pimpl->getAllAnalogOut();
}
