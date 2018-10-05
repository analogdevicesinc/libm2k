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

#include "m2k.hpp"
#include "m2kanalogin.hpp"
#include "libm2k/m2kexceptions.hpp"
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
}

M2K::~M2K()
{

}

void M2K::scanAllAnalogIn()
{
	try {
		GenericAnalogIn* aIn = new M2kAnalogIn(ctx(), "m2k-adc");
		s_instancesAnalogIn.push_back(aIn);
	} catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}
}

