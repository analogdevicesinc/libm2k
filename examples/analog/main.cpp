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
// This example assumes the following connections:
// W1 -> 1+
// W2 -> 2+
// GND -> 1-
// GND -> 2-
//
// The application will generate a sine and triangular wave on W1 and W2. The signal is fed back into the analog input
// and the voltage values are displayed on the screen

#include <iostream>
#include <math.h>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::contexts;

#define M_PI 3.14
// uncomment the following definition to test triggering
//#define TRIGGERING

int main(int argc, char* argv[])
{
	M2k *ctx = m2kOpen();
	if (!ctx) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
		return 1;
	}

	ctx->calibrateADC();
	ctx->calibrateDAC();

	M2kAnalogIn *ain = ctx->getAnalogIn();
	M2kAnalogOut *aout = ctx->getAnalogOut();
	M2kHardwareTrigger *trig = ain->getTrigger();

	// Setup analog in
	ain->enableChannel(0, true);
	ain->enableChannel(1, true);
	ain->setSampleRate(100000);
	ain->setRange((ANALOG_IN_CHANNEL)0,-10.0,10.0);
	ain->setRange((ANALOG_IN_CHANNEL)1,PLUS_MINUS_25V);

#ifdef TRIGGERING
	// setup analog trigger
	trig->setAnalogSource(CHANNEL_1);
	trig->setAnalogCondition(0,RISING_EDGE);
	trig->setAnalogLevel(0, 0.5);
	trig->setAnalogDelay(0);
	trig->setAnalogMode(0,ANALOG);
#endif

	// setup analog output
	aout->setSampleRate(0,750000);
	aout->setSampleRate(1,750000);
	aout->enableChannel(0, true);
	aout->enableChannel(1, true);

	// create output buffers
	vector<double> sinv;
	vector<double> saw;

	for(int i=0;i<1024;i++)
	{
		double rad = 2*M_PI*(i/1024.0);
		double val = sin(rad);
		sinv.push_back(val);
		saw.push_back((2*i)/1024.0);
	}

	aout->setCyclic(true);
	aout->push({sinv,saw});

	auto data = ain->getSamples(1024);

	cout << "CHANNEL1"<<endl;
	for(double val : data[0])
	{
		cout<<val<<endl;
	}
	cout << "CHANNEL2"<<endl;
	for(double val : data[1])
	{
		cout<<val<<endl;
	}
	cout <<"end";
	aout->stop();
	contextClose(ctx);
}
