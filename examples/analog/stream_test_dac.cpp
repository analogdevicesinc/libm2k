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
// This example shows the analog DAC streaming feature of the m2k
//
// Running example: ./stream_test_dac uri=usb:1.9.5  kernel=64 oversampling_ratio=10 no_samples=15384 amplitude_out=2.0 timeout=3
//
// Help:
// -kernel : integer number of desired kernel buffers
// -oversampling_ratio : integer number to determine sample_rate for ADC (sample_rate = 100MHz/oversampling-ratio
// -no_samples : integer number to determine the buffer size for ADC
// -frequency_out : The frequency of sawtooth signal generated on the DAC channels (in Hz)
// -amplitude_out: The frequency of sawtooth signal generated on the DAC channels (in Volts)
// -timeout: How long should this test run. Leave empty to run forever.
//
// The application will generate a sawtooth wave on W1 and W2.
// (The amplitude and frequency should be provided as arguments to this application).
// If a timeout value is provided, the program will exit
// The DAC streaming should be analyzed using an external Oscilloscope.

#include <iostream>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <bitset>
#include <queue>

#include <thread>
#include <mutex>
#include <iomanip>
#include <condition_variable>
#include <math.h>
#include <fstream>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::context;

#define MAX_SAMPLE_RATE_OUT 75000000

static int KERNEL_BUFFERS_COUNT;
static string URI;
static int OVERSAMPLING_RATIO;
static int OUT_NB_SAMPLES;
static double AMPLITUDE_SIGNAL_OUT;
static double TIMEOUT = 0;
static double OFFSET_SIGNAL_OUT;
static double SAMPLE_RATE_OUT;

static bool DONT_STOP_STREAMING = true;


std::vector<double> generate_sawtooth_wave(int nb_samples, double signal_amplitude, double signal_offset)
{
	std::vector<double> best_buffer_out;
	for (int i = 0; i < nb_samples; i++) {
		best_buffer_out.push_back((signal_amplitude * ((double)i / nb_samples)) + signal_offset );
	}

	return best_buffer_out;
}

void push_thread(M2kAnalogOut *aout, vector<vector<double>> bufferOut) {
	int buffer_counter = 0;
	while (DONT_STOP_STREAMING) {
		aout->push(bufferOut);
		buffer_counter++;
		std::cout << "buffer cnt: " << buffer_counter << " buffer size: " << bufferOut[0].size() << std::endl;
	}
}

std::vector<std::string> split(std::string s, std::string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;
	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}
	res.push_back(s.substr(pos_start));
	return res;
}


int main(int argc, char **argv)
{
	if (argc != 7) {
		std::cout << "Missing parameters: 'stream_test_dac.exe "
			     "uri=<usb:x.y.z or ip:192.168.2.1> "
			     "kernel=<1..64> "
			     "oversampling_ratio=<1..75E6> "
			     "no_samples=<int> "
			     "amplitude_out=<1.0..5.0> "
			     "timeout=<seconds> ' \n\n"
			     "Help:\n"
			     "-uri : The URI of ADALM2000.\n"
			     "-kernel : integer number of desired kernel buffers\n"
			     "-oversampling_ratio : integer number to determine sample_rate for DAC (sample_rate = 75MHz/oversampling-ratio\n"
			     "-nb_samples_out : The buffer size for the output signal\n"
			     "-amplitude_out: The frequency of sawtooth signal generated on the DAC channels (in Volts)\n"
			     "-timeout: How long should this test run. Leave empty to run forever."
			  << std::endl;
		return 1;
	}

	URI = split(argv[1], "=")[1];
	KERNEL_BUFFERS_COUNT = stoi(split(argv[2], "=")[1]);
	OVERSAMPLING_RATIO =  stoi(split(argv[3], "=")[1]);
	OUT_NB_SAMPLES =  stoi(split(argv[4], "=")[1]);
	AMPLITUDE_SIGNAL_OUT = stod(split(argv[5], "=")[1]);
	auto tmp = split(argv[6], "=");
	if (tmp[1] != "") {
		TIMEOUT = stod(tmp[1]);
	} else {
		TIMEOUT = 0;
	}
	OFFSET_SIGNAL_OUT = -(AMPLITUDE_SIGNAL_OUT/2);

	SAMPLE_RATE_OUT = MAX_SAMPLE_RATE_OUT / OVERSAMPLING_RATIO;
	printf("DAC stream speed test parameters\n"
	       "Signal amplitude: %f Volts\n"
	       "Signal buffer size: %d  samples\n"
	       "Kernel Buffers: %d\n"
	       "Generating %f seconds worth of data\n"
	       "Streaming @ %f\n",
	       AMPLITUDE_SIGNAL_OUT, OUT_NB_SAMPLES, KERNEL_BUFFERS_COUNT, TIMEOUT, SAMPLE_RATE_OUT);

	M2k *ctx = m2kOpen(URI.c_str());
	if (!ctx) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
		return 1;
	}

	M2kAnalogOut *aout = ctx->getAnalogOut();

	// Prevent bad initial config
	aout->reset();

	ctx->calibrateDAC();

	aout->setSampleRate(0, MAX_SAMPLE_RATE_OUT);
	aout->setSampleRate(1, MAX_SAMPLE_RATE_OUT);

	// set number of kernel buffers for the analog output interface
	aout->setKernelBuffersCount(0, KERNEL_BUFFERS_COUNT);
	aout->setKernelBuffersCount(1, KERNEL_BUFFERS_COUNT);

	aout->setOversamplingRatio(0, OVERSAMPLING_RATIO);
	aout->setOversamplingRatio(1, OVERSAMPLING_RATIO);

	auto buf_ch = generate_sawtooth_wave(OUT_NB_SAMPLES, AMPLITUDE_SIGNAL_OUT, OFFSET_SIGNAL_OUT);
	vector<vector<double>> bufferOut {buf_ch, buf_ch};

	/* Enable the DAC channels before every push(after every stop) in order to setup powerdown */
	aout->enableChannel(0, true);
	aout->enableChannel(1, true);
	aout->setCyclic(false);

	// Startup push threads
	std::thread producer = std::thread([](M2kAnalogOut *aout, vector<vector<double>> buf){
					push_thread(aout, buf);
				}, aout, bufferOut);

	if (TIMEOUT != 0) {
		std::this_thread::sleep_for(std::chrono::duration<double>(TIMEOUT));
		DONT_STOP_STREAMING = false;
	}

	producer.join();

	aout->stop();
	aout->setOversamplingRatio(0, 1);
	aout->setOversamplingRatio(1, 1);
	contextClose(ctx);

	return 0;
}
