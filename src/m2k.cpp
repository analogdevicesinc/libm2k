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
#include "libm2k/m2kexceptions.hpp"
#include "libm2k/m2kcalibration.hpp"
#include <iostream>

using namespace std;
using namespace libm2k::devices;
using namespace libm2k::analog;

M2K::M2K(std::string uri, iio_context* ctx, std::string name) :
	GenericDevice(uri, ctx, name)
{
	std::cout << "I am M2K device " << std::endl;

	/* Initialize the AnalogIn list */
	for (auto aIn : s_instancesAnalogIn) {
		delete aIn;
	}
	s_instancesAnalogIn.clear();
	scanAllAnalogIn();
	std::vector<libm2k::analog::M2kAnalogIn*> lst = getAllAnalogIn();
	m_calibration = new M2kCalibration(lst);
}

M2K::~M2K()
{

}

void M2K::scanAllAnalogIn()
{
	try {
		GenericAnalogIn* aIn = new libm2k::analog::M2kAnalogIn(ctx(), "m2k-adc");
		s_instancesAnalogIn.push_back(aIn);
	} catch (std::runtime_error& e) {
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

}

M2kAnalogIn* M2K::getAnalogIn(unsigned int index)
{
	if (index < s_instancesAnalogIn.size()) {
		return dynamic_cast<libm2k::analog::M2kAnalogIn*>(
					s_instancesAnalogIn.at(index));
	} else {
		throw no_device_exception("No such analog in");
//		return nullptr;
	}
}

M2kAnalogIn *M2K::getAnalogIn(string dev_name)
{
	for (GenericAnalogIn* d : s_instancesAnalogIn) {
		if (d->getDeviceName() == dev_name) {
			libm2k::analog::M2kAnalogIn* analogIn =
				dynamic_cast<libm2k::analog::M2kAnalogIn*>(d);
			if (analogIn) {
				return analogIn;
			}
		}
	}
	return nullptr;
}

std::vector<M2kAnalogIn*> M2K::getAllAnalogIn()
{
	std::vector<libm2k::analog::M2kAnalogIn*> allAnalogIn = {};
	for (GenericAnalogIn* inst : s_instancesAnalogIn) {
		try {
			libm2k::analog::M2kAnalogIn* analogIn =
				dynamic_cast<libm2k::analog::M2kAnalogIn*>(inst);
			if (analogIn) {
				allAnalogIn.push_back(analogIn);
			}
		} catch (std::runtime_error &e) {
			throw no_device_exception(e.what());
		}
	}
	return allAnalogIn;
}
