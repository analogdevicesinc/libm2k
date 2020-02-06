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

// This example will generate a binary counter on the first N_BITS of the
// digital interface and read them back - no additional connection required
// This example shows the streaming feature of the m2k
// Running /digital_stream > test && sed -n 4090,4110p test
// will show data around the end of the kernel buffers. Each sample should
// be visible SAMPLE_RATE_IN / SAMPLE_RATE_OUT times to make sure there are no
// dropped samples between kernel buffers

#include <iostream>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/digital/m2kdigital.hpp>
#include <bitset>

#include <thread>
#include <mutex>
#include <iomanip>
#include <condition_variable>

using namespace std;
using namespace libm2k;
using namespace libm2k::digital;
using namespace libm2k::contexts;

#define N_BITS (16)
#define IN_NO_SAMPLES (1<<16)
#define KERNEL_BUFFERS_COUNT 8
#define MAX_SAMPLE_RATE 100000000
#define NUMBER_OF_BUFFERS 100
#define SR_DIVIDER_STEP 1
#define SR_DIVIDER_START 30


static bool running = true;
static std::mutex data_mtx, process_mtx;
static std::condition_variable cv_process, cv_process_done;
static const unsigned short* tmp_buffer_p = nullptr;
static std::vector<unsigned short> tmp_buffer;
static std::vector<uint16_t> values;

void refill_thread(M2kDigital *dig) {
	for (int refills = 0; refills < NUMBER_OF_BUFFERS; refills++) {
		unique_lock<mutex> lock(data_mtx);

//		cv_process.wait(lock, ([](){return (tmp_buffer.empty()); }));
//		tmp_buffer = dig->getSamples(IN_NO_SAMPLES);

		cv_process.wait(lock, ([](){return (tmp_buffer_p == nullptr); }));
		tmp_buffer_p = dig->getSamplesP(IN_NO_SAMPLES);

		lock.unlock();
		cv_process.notify_one();
	}
}

void process_thread(){
	for (int refills = 0; refills < NUMBER_OF_BUFFERS; refills++) {
		unique_lock<mutex> lock(data_mtx);

//		cv_process.wait(lock, ([](){return (!tmp_buffer.empty()); }));
//		for (int i = 0; i < IN_NO_SAMPLES; i++) {
//			values.push_back(tmp_buffer[i]);
//		}
//		tmp_buffer.clear();


		cv_process.wait(lock, ([](){return (tmp_buffer_p != nullptr); }));
		for (int i = 0; i < IN_NO_SAMPLES; i++) {
			values.push_back(tmp_buffer_p[i]);
		}
		tmp_buffer_p = nullptr;

		lock.unlock();
		cv_process.notify_one();
	}
	running = false;
	cv_process_done.notify_one();
}


int main()
{
	int sr_divider = SR_DIVIDER_START;
	int sample_rate_in, sample_rate_out = MAX_SAMPLE_RATE;

	printf("Digital stream speed test parameters\nBuffersize: %d\nKernel Buffers: %d\nTotal Buffers: %d\n", IN_NO_SAMPLES, KERNEL_BUFFERS_COUNT, NUMBER_OF_BUFFERS);

	M2k *ctx = m2kOpen();
	if (!ctx) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
		return 1;
	}

	M2kDigital *dig = ctx->getDigital();
	M2kHardwareTrigger *trig = dig->getTrigger();

	while(sr_divider > 1) {
		sample_rate_in = MAX_SAMPLE_RATE / sr_divider;
		sample_rate_out = MAX_SAMPLE_RATE / (sr_divider*4);

		// set sample rates for in/out interface
		dig->setSampleRateIn(sample_rate_in + 1);
		dig->setSampleRateOut(sample_rate_out + 1);
		// set number of kernel buffers for the digital input interface
		dig->setKernelBuffersCountIn(KERNEL_BUFFERS_COUNT);
		trig->setDigitalStreamingFlag(true);

		for(int i=0;i<N_BITS;i++)
		{
			dig->setDirection(i, DIO_OUTPUT);
			dig->enableChannel(i, true);
		}

		vector<unsigned short> bufferOut;

		for(int i=0;i< (1<<N_BITS); i++)
		{
			bufferOut.push_back(i);
		}
		dig->setCyclic(true);
		dig->push(bufferOut);


		// Startup refill threads
		std::thread producer = std::thread([](M2kDigital *dig){ refill_thread(dig); }, dig);
		std::thread consumer = std::thread([](){ process_thread(); });

		std::unique_lock<std::mutex> lk(process_mtx);
		cv_process_done.wait(lk, []{return !running;});

		producer.join();
		consumer.join();

#ifdef SHOW_BUFFERS
		for(auto buf : buffers)
		{
			for( auto val : buf)
				cout<<bitset<16>(val)<<endl;
			cout << "-------- BUFFER " << i << "--------"<< endl;
			i++;
		}
#else
		bool stable = true;
		uint16_t same_val;
		uint32_t same_val_cnt=0;
		int dropped = 0;
		int i;

		for(i=1;i<values.size();i++) {
			// find first transition
			if(values[i]!=values[i-1])
			{
				same_val = values[i];
				break;
			}
		}
		for(i;i<values.size();i++)
		{
			if(values[i]==same_val)
				same_val_cnt++;
			else
			{
				int divider = (int)(sample_rate_in/sample_rate_out);
				if(same_val_cnt == divider) {// || same_val_cnt == divider - 1 || same_val_cnt==divider+1) {
					{
						same_val = values[i];
						same_val_cnt=1;
					}

				} else {
					dropped = abs(values[i]-same_val)*divider+(same_val_cnt-divider);
					stable=false;
					break;
				}
			}
		}

		std::cout << "SR_DIVIDER: " << std::setw(2) << sr_divider << " SR_IN: " << std::setw(9) << sample_rate_in <<
			     " SR_OUT: " << std::setw(9) << sample_rate_out << ", " << ((stable) ? "  STABLE" : "UNSTABLE") <<
			     " dropped: " << std::setw(6) << dropped << " samples";
		if(!stable)
			std::cout <<  " @ buffer " << std::setw(3) << i/IN_NO_SAMPLES << /* "["<<i<<"] " <<*/
				" prev val: " << bitset<16>(same_val) << " next val:  "<< bitset<16>(values[i]);
		std::cout << std::endl;

		sr_divider -= SR_DIVIDER_STEP;

		dig->flushBufferIn();
		dig->stopBufferOut();
		values.clear();
	}

#endif
	contextClose(ctx);

	return 0;
}
