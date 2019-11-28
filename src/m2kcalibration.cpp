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

#include "private/m2kcalibration_impl.cpp"

using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::utils;

M2kCalibration::M2kCalibration(struct iio_context* ctx, M2kAnalogIn* analogIn,
			       M2kAnalogOut* analogOut):
	m_pimpl(std::unique_ptr<M2kCalibrationImpl>(new M2kCalibrationImpl(ctx, analogIn, analogOut)))
{
}

M2kCalibration::~M2kCalibration()
{
}

bool M2kCalibration::initialize()
{
	return m_pimpl->initialize();
}

bool M2kCalibration::isInitialized() const
{
	return m_pimpl->isInitialized();
}

void M2kCalibration::setAdcInCalibMode()
{
	m_pimpl->setAdcInCalibMode();
}

void M2kCalibration::setDacInCalibMode()
{
	m_pimpl->setDacInCalibMode();
}

void M2kCalibration::restoreAdcFromCalibMode()
{
	m_pimpl->restoreAdcFromCalibMode();
}

void M2kCalibration::restoreDacFromCalibMode()
{
	m_pimpl->restoreDacFromCalibMode();
}

bool M2kCalibration::calibrateADCoffset()
{
	return m_pimpl->calibrateADCoffset();
}

bool M2kCalibration::calibrateADCgain()
{
	return m_pimpl->calibrateADCgain();
}

int M2kCalibration::adcOffsetChannel0() const
{
	return m_pimpl->adcOffsetChannel0();
}

int M2kCalibration::adcOffsetChannel1() const
{
	return m_pimpl->adcOffsetChannel1();
}

double M2kCalibration::adcGainChannel0() const
{
	return m_pimpl->adcGainChannel0();
}

double M2kCalibration::adcGainChannel1() const
{
	return m_pimpl->adcGainChannel1();
}

void M2kCalibration::updateDacCorrections()
{
	m_pimpl->updateDacCorrections();
}

void M2kCalibration::updateAdcCorrections()
{
	m_pimpl->updateAdcCorrections();
}

bool M2kCalibration::resetCalibration()
{
	return m_pimpl->resetCalibration();
}

int M2kCalibration::dacAoffset() const
{
	return m_pimpl->dacAoffset();
}

int M2kCalibration::dacBoffset() const
{
	return m_pimpl->dacBoffset();
}

double M2kCalibration::dacAvlsb() const
{
	return m_pimpl->dacAvlsb();
}
double M2kCalibration::dacBvlsb() const
{
	return m_pimpl->dacBvlsb();
}

bool M2kCalibration::calibrateDACoffset()
{
	return m_pimpl->calibrateDACoffset();
}

bool M2kCalibration::calibrateDACgain()
{
	return m_pimpl->calibrateDACgain();
}

bool M2kCalibration::calibrateADC()
{
	return m_pimpl->calibrateADC();
}

bool M2kCalibration::calibrateDAC()
{
	return m_pimpl->calibrateDAC();
}

bool M2kCalibration::calibrateAll()
{
	return m_pimpl->calibrateAll();
}

void M2kCalibration::cancelCalibration()
{
	m_pimpl->cancelCalibration();
}

bool M2kCalibration::setCalibrationMode(int mode)
{
	return m_pimpl->setCalibrationMode(mode);
}

