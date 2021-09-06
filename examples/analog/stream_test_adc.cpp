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
// This example shows the analog streaming feature of the m2k
//
// Running example: ./stream_test_adc uri=usb:1.6.5  kernel=64 oversampling_ratio=10 no_samples=15384 frequency_out=75000 amplitude_out=2.0 timeout=5
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
// The signal is fed back into the analog input and the voltage values are analyzed by the Analog Input.
// If a timeout value is provided, the program will exit and print whether the capture was stable
// with the Analog In running at the provided samplerate.

#include <iostream>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <bitset>
#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>
#include <fstream>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::context;

#define MAX_SAMPLE_RATE_OUT 75000000
#define MAX_SAMPLE_RATE_IN 100000000

static int KERNEL_BUFFERS_COUNT;
static string URI;
static int OVERSAMPLING_RATIO;
static int IN_NO_SAMPLES;
static double FREQUENCY_SIGNAL_OUT;
static double AMPLITUDE_SIGNAL_OUT;
static double TIMEOUT = 0;
static double OFFSET_SIGNAL_OUT;

static bool DONT_STOP_STREAMING = true;
static vector<bool> running {true, true};
static std::mutex data_mtx, process_mtx, analyze_mtx;
static std::condition_variable cv_process, cv_process_done, cv_analyze;
static const short* tmp_buffer_p = nullptr;
static std::vector<queue<double>> values = {std::queue<double>(), std::queue<double>()};


std::vector<double> generate_sawtooth_wave(double signal_frequency, double signal_amplitude, double signal_offset)
{
	std::vector<double> best_buffer_out;
	double best_fract = 1.0;
	double current_fract;
	double current_int;
	double N_BUFFER = 0;
	double N_SIGNAL = MAX_SAMPLE_RATE_OUT / signal_frequency;
	for (int i = 1; i < 10000; i++) {
		N_BUFFER = i * N_SIGNAL;
		current_fract = modf (N_BUFFER, &current_int);
		if ((current_fract == 0.0) && (((int)N_BUFFER % 4) == 0)) {
			break;
		}
		if (current_fract < best_fract) {
			best_fract = current_fract;
		}
	}

	for (int i = 0; i < N_BUFFER; i++) {
		double intpart;
		double fract = modf(i / N_SIGNAL, &intpart);
		double sawtooth_val = (signal_amplitude * fract) + signal_offset;
		best_buffer_out.push_back(sawtooth_val);
	}

	return best_buffer_out;
}

void refill_thread(M2kAnalogIn *ain) {
	while (DONT_STOP_STREAMING) {
		unique_lock<mutex> lock(data_mtx);

		cv_process.wait(lock, ([](){return (tmp_buffer_p == nullptr); }));
		tmp_buffer_p = ain->getSamplesRawInterleaved(IN_NO_SAMPLES);

		lock.unlock();
		cv_process.notify_one();
	}
}

void process_thread(M2kAnalogIn *ain){
	while (true) {
		unique_lock<mutex> lock(data_mtx);
		int nb_channels = ain->getNbChannels();

		if (!DONT_STOP_STREAMING && tmp_buffer_p == nullptr) {
			cv_analyze.notify_all();
			lock.unlock();
			break;
		}

		cv_process.wait(lock, ([](){return (tmp_buffer_p != nullptr); }));
		unique_lock<mutex> lock_analyze(analyze_mtx);
		for (int i = 0; i < IN_NO_SAMPLES; i++) {
			values[0].push(ain->convertRawToVolts(0, tmp_buffer_p[i * nb_channels]));
			values[1].push(ain->convertRawToVolts(1, tmp_buffer_p[i * nb_channels + 1]));
		}
		lock_analyze.unlock();
		tmp_buffer_p = nullptr;

		lock.unlock();
		cv_process.notify_one();
		cv_analyze.notify_all();
	}
}


void analyze_thread(M2kAnalogIn *ain, unsigned int channel) {
	int buffer_counter = 0;
	int current_index = 0;
	int samp_cnt = 0;		// count the samples between zero crossings
	int prev_samp_cnt = 0;		// the previous count
	int nb_crossings = 0;
	double last_sample = 0;
	bool stable = true;


	std::ofstream outfile;
	std::string filename = "stream-ch" + to_string(channel) + ".csv";
	outfile.open(filename);
	outfile.close();

	while (true) {
		unique_lock<mutex> lock(analyze_mtx);

		if (!DONT_STOP_STREAMING && values[channel].empty()) {
			lock.unlock();
			break;
		}

		cv_analyze.wait(lock, ([&channel]{ return (!values[channel].empty()); }));
		lock.unlock();

		stable = true;

		std::ofstream outfile;
		std::string filename = "stream-ch" + to_string(channel) + ".csv";
		outfile.open(filename, std::ofstream::app);

		/* If this is the first aquired buffer, find the first transition
			 * And also initialize the counters */
		if (nb_crossings == 0) {
			last_sample = values[channel].front();
			unique_lock<mutex> lock(analyze_mtx);
			values[channel].pop();
			lock.unlock();
			outfile << last_sample << "\n";
			current_index++;
			while (values[channel].size() > 0) {
				double current_sample = values[channel].front();
				// find the difference between the first 2 zero crossings on falling edge
				if ((current_sample <= 0) && (last_sample > 0) && (abs(current_sample - last_sample) >= 0.1)) {
					prev_samp_cnt = samp_cnt;
					samp_cnt = 1;
					nb_crossings++;
				} else {
					samp_cnt++;
				}
				outfile << current_sample << "\n";
				last_sample = current_sample;
				unique_lock<mutex> lock(analyze_mtx);
				values[channel].pop();
				lock.unlock();
				current_index++;

				if (nb_crossings > 1) {
					break;
				}
			}
		}

		while (values[channel].size() > 0) {
			double current_sample = values[channel].front();

			if ((current_sample <= 0) && (last_sample > 0) && (abs(last_sample - current_sample) >= 0.1)) {
				if (abs(prev_samp_cnt - samp_cnt) > 1) {
					stable = false;
					std::cout << std::endl << "UNSTABLE channel: " << channel
						  << " cross. len: " << samp_cnt
						  << " from len: " << prev_samp_cnt
						  << " @ index " << current_index;
				}
				samp_cnt = 1;
				nb_crossings++;
			} else {
				samp_cnt++;
			}

			if (current_index % IN_NO_SAMPLES == 0) {
				std::cout << " channel: " << channel
					  << " @ buffer: " << current_index/IN_NO_SAMPLES << std::endl;
			}
			outfile << current_sample << "\n";
			last_sample = current_sample;
			unique_lock<mutex> lock(analyze_mtx);
			values[channel].pop();
			lock.unlock();
			current_index++;
		}

		if (nb_crossings == 0) {
			stable = false;
		}

		outfile.close();
		buffer_counter++;
	}
	std::cout << ((stable) ? "  STABLE, " : "UNSTABLE, ") << " channel " << channel << std::endl;
	running[channel] = false;
	cv_process_done.notify_one();
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
	if (argc != 8) {
		std::cout << "Missing parameters: 'stream_test_adc.exe uri=<usb:x.y.z or ip:192.168.2.1>"
			     "kernel=<1..64> "
			     "oversampling_ratio=<1..1E6> "
			     "no_samples=<int> "
			     "frequency_out=<double> "
			     "amplitude_out=<1.0..5.0> "
			     "timeout=<seconds> ' \n\n"
			     "Help:\n"
			     "-uri : The URI of ADALM2000.\n"
			     "-kernel : integer number of desired kernel buffers\n"
			     "-oversampling_ratio : integer number to determine sample_rate for ADC (sample_rate = 100MHz/oversampling-ratio\n"
			     "-no_samples : integer number to determine the buffer size for ADC\n"
			     "-frequency_out : The frequency of sawtooth signal generated on the DAC channels (in Hz)\n"
			     "-amplitude_out: The frequency of sawtooth signal generated on the DAC channels (in Volts)\n"
			     "-timeout: How long should this test run. Leave empty to run forever."
			  << std::endl;
		return 1;
	}

	URI = split(argv[1], "=")[1];
	KERNEL_BUFFERS_COUNT = stoi(split(argv[2], "=")[1]);
	OVERSAMPLING_RATIO =  stoi(split(argv[3], "=")[1]);
	IN_NO_SAMPLES =  stoi(split(argv[4], "=")[1]);
	FREQUENCY_SIGNAL_OUT =  stod(split(argv[5], "=")[1]);
	AMPLITUDE_SIGNAL_OUT = stod(split(argv[6], "=")[1]);
	auto tmp = split(argv[7], "=");
	if (tmp[1] != "") {
		TIMEOUT = stod(tmp[1]);
	} else {
		TIMEOUT = 0;
	}
	OFFSET_SIGNAL_OUT = -(AMPLITUDE_SIGNAL_OUT/2);


	printf("ADC stream speed test parameters\nDAC generating a %f sawtooth of amplitude %f\nBuffersize: %d\nKernel Buffers: %d\nADC Samplerate: %f\n",
	       FREQUENCY_SIGNAL_OUT, AMPLITUDE_SIGNAL_OUT, IN_NO_SAMPLES, KERNEL_BUFFERS_COUNT, (double)MAX_SAMPLE_RATE_IN / OVERSAMPLING_RATIO);

	M2k *ctx = m2kOpen(URI.c_str());
	if (!ctx) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
		return 1;
	}

	M2kAnalogIn *ain = ctx->getAnalogIn();
	M2kAnalogOut *aout = ctx->getAnalogOut();
	M2kHardwareTrigger *trig = ain->getTrigger();

	// Prevent bad initial config
	ain->reset();
	aout->reset();

	ctx->calibrateADC();
	ctx->calibrateDAC();

	/* Always use MAX_SR_IN, adjust the samplerate using oversampling_ratio (sr_divider) */
	ain->setSampleRate(MAX_SAMPLE_RATE_IN);
	aout->setSampleRate(0, MAX_SAMPLE_RATE_OUT);
	aout->setSampleRate(1, MAX_SAMPLE_RATE_OUT);

	// set number of kernel buffers for the analog input interface
	ain->setKernelBuffersCount(KERNEL_BUFFERS_COUNT);

	for(unsigned int i = 0; i < ain->getNbChannels(); i++) {
		ain->enableChannel(i, true);
	}

	trig->setAnalogMode(0, ALWAYS);
	trig->setAnalogMode(1, ALWAYS);

	auto buf_ch1 = generate_sawtooth_wave(FREQUENCY_SIGNAL_OUT, AMPLITUDE_SIGNAL_OUT, OFFSET_SIGNAL_OUT);
	vector<vector<double>> bufferOut {buf_ch1, buf_ch1};

	/* Enable the DAC channels before every push(after every stop) in order to setup powerdown */
	aout->enableChannel(0, true);
	aout->enableChannel(1, true);
	aout->setOversamplingRatio(0, 1);
	aout->setOversamplingRatio(1, 1);

	aout->setCyclic(true);
	aout->push(bufferOut);

	ain->setOversamplingRatio(OVERSAMPLING_RATIO);
	trig->setAnalogStreamingFlag(false);
	trig->setAnalogStreamingFlag(true);

	// Startup refill threads
	std::thread producer = std::thread([](M2kAnalogIn *ain){ refill_thread(ain); }, ain);
	std::thread consumer = std::thread([](M2kAnalogIn *ain){ process_thread(ain); }, ain);
	std::thread analyzer_ch0 = std::thread([](M2kAnalogIn *ain, unsigned int ch){ analyze_thread(ain, ch); }, ain, 0);
	std::thread analyzer_ch1 = std::thread([](M2kAnalogIn *ain, unsigned int ch){ analyze_thread(ain, ch); }, ain, 1);

	if (TIMEOUT != 0) {
		std::this_thread::sleep_for(std::chrono::duration<double>(TIMEOUT));
		DONT_STOP_STREAMING = false;
	}

	std::unique_lock<std::mutex> lk(process_mtx);
	cv_process_done.wait(lk, []{return (!running[0] && ! running[1]);});

	producer.join();
	consumer.join();
	analyzer_ch0.join();
	analyzer_ch1.join();


	ain->cancelAcquisition();
	ain->stopAcquisition();
	aout->stop();
	ain->setOversamplingRatio(1);
	contextClose(ctx);

	return 0;
}
