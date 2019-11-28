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

// This exammple assumes connection:
// 1+ to V+
// 1- to GND
// This example turns on the power supply and sets it to 1.7V and then reads back the value

#include <iostream>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/analog/m2kanalogin.hpp>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::contexts;

#define PS_CHANNEL (0)
#define PS_VOLTAGE (1.7)

int main()
{
	M2k *ctx = m2kOpen();
	if (!ctx) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
		return 1;
	}

	ctx->calibrateADC();

	// Will turn on the power supply if we need smth to measure
	M2kPowerSupply *ps = ctx->getPowerSupply();
	ps->enableChannel(PS_CHANNEL, true);
	ps->pushChannel(PS_CHANNEL, PS_VOLTAGE);

	// Setup analog in
	M2kAnalogIn *ain = ctx->getAnalogIn();
	ain->enableChannel(0, true);

	double voltage = ain->getVoltage(0);
	cout << voltage;
	return 0;
}
