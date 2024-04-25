/*
 * Copyright (c) 2024 Analog Devices Inc.
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
#define _USE_MATH_DEFINES
#include <iostream>
#include <thread>
#include <math.h>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <libm2k/digital/m2kdigital.hpp>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::digital;
using namespace libm2k::context;

#define NUM_SAMPLES (1024)

#define CFG_TRIGGER_TI
// #define CFG_TRIGGER_ADC
// #define CFG_TRIGGER_LA

// Buffers are sent one at a time with each trigger event
// #define BUFFER_REARM

std::vector<double> generate_square_wave(int num_samples) {
	std::vector<double> samples(num_samples);
	for (int i = 0; i < num_samples; i++) {
		samples[i] = (i % 2 == 0) ? 1.0 : -1.0;
	}
	return samples;
}

int main(int argc, char* argv[])
{
	M2k *ctx = m2kOpen("ip:192.168.2.1");
	if (!ctx) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
		return 1;
	} else {
		std::cout << "Connection Success: ADALM2000 device connected." << std::endl;
	}

	M2kAnalogIn *ain = ctx->getAnalogIn();
	M2kAnalogOut *aout = ctx->getAnalogOut();
	M2kDigital *dig = ctx->getDigital();
	M2kHardwareTrigger *trig = aout->getTrigger();

	ctx->reset();
    ctx->calibrateADC();
    ctx->calibrateDAC();

#ifdef BUFFER_REARM
	aout->setBufferRearmOnTrigger(true);
#endif

#ifdef CFG_TRIGGER_TI
	trig->setAnalogOutTriggerSource(TRIGGER_TI);
	trig->setAnalogOutTriggerCondition(RISING_EDGE_OUT);
	trig->setAnalogOutTriggerStatus(START);
#endif

#ifdef CFG_TRIGGER_ADC
	ain->setSampleRate(ain->getMaximumSamplerate()); // fastest response
	trig->setAnalogSource(CHANNEL_1);
	trig->setAnalogCondition(CHANNEL_1, RISING_EDGE_ANALOG);
	trig->setAnalogLevel(CHANNEL_1, 0.5);
	trig->setAnalogDelay(-100);
	trig->setAnalogMode(CHANNEL_1, ANALOG);

	trig->setAnalogOutTriggerSource(TRIGGER_ADC);
	trig->setAnalogOutTriggerStatus(START);	
#endif

#ifdef CFG_TRIGGER_LA
	dig->setSampleRateIn(75000000); // fastest response
	dig->setDirection(DIO_CHANNEL_0, DIO_INPUT);
	dig->enableChannel(DIO_CHANNEL_0, true);
	dig->setOutputMode(DIO_CHANNEL_0, DIO_PUSHPULL);
	trig->setDigitalCondition(DIO_CHANNEL_0, RISING_EDGE_DIGITAL);

	trig->setAnalogOutTriggerSource(TRIGGER_LA);
	trig->setAnalogOutTriggerStatus(START);	
#endif

	// Setup analog output
	aout->setSampleRate(0, aout->getMaximumSamplerate(0));
	aout->enableChannel(0, true);
	aout->setCyclic(true);

	auto waveform = generate_square_wave(NUM_SAMPLES);
	aout->push({waveform, waveform});

	contextClose(ctx);
	return 0;
}
