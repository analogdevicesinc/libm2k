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

using namespace std;
using namespace libm2k;
using namespace libm2k::digital;
using namespace libm2k::contexts;

#define N_BITS (6)
#define IN_NO_SAMPLES 1000
#define KERNEL_BUFFERS_COUNT 4
#define MAX_SAMPLE_RATE 100000000

#define SAMPLE_RATE_IN 2000000
#define SAMPLE_RATE_OUT 250000

#define NUMBER_OF_BUFFERS 100
//#define SR_DIVIDER 67
//#define SAMPLE_RATE_IN (int)(MAX_SAMPLE_RATE/SR_DIVIDER)
//#define SAMPLE_RATE_OUT  (int)(MAX_SAMPLE_RATE/(SR_DIVIDER*4))


int main()

{
	M2k *ctx = m2kOpen();
	std::vector<std::vector<uint16_t>> buffers;
	if (!ctx) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
		return 1;
	}

	M2kDigital *dig = ctx->getDigital();

	// set sample rates for in/out interface
	dig->setSampleRateIn(SAMPLE_RATE_IN);
	dig->setSampleRateOut(SAMPLE_RATE_OUT);
	// set number of kernel buffers for the digital input interface
	dig->setKernelBuffersCountIn(KERNEL_BUFFERS_COUNT);

	for(int i=0;i<N_BITS;i++)
	{
		dig->setDirection(i, DIO_OUTPUT);
		dig->enableChannel(i, true);
	}

	vector<unsigned short> bufferOut;
	vector<unsigned short> bufferIn;

	for(int i=0;i< (1<<N_BITS); i++)
	{
		bufferOut.push_back(i);
	}
	dig->setCyclic(true);
	dig->push(bufferOut);


	for(int i=0;i<NUMBER_OF_BUFFERS;i++) {
		// always get same number of samples so buffer configuration does not reset
		/*auto val = dig->getSamplesP(IN_NO_SAMPLES);
		buffers.push_back(std::vector<uint16_t>(val,val+IN_NO_SAMPLES));*/
		buffers.push_back(dig->getSamples(IN_NO_SAMPLES));
	}

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
	std::vector<uint16_t> values;
	uint16_t same_val;
	uint32_t same_val_cnt=0;
	int i;

	for(auto buf : buffers)
		for( auto val : buf)
			values.push_back(val);
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
			if(same_val_cnt==(int)(SAMPLE_RATE_IN/SAMPLE_RATE_OUT)) {
				{
					same_val = values[i];
					same_val_cnt=1;
				}

			} else {
				stable=false;
				break;
			}
		}
	}
	printf("%s\n", (stable) ? "STABLE" : "UNSTABLE");

#endif
	dig->stopBufferOut();
	contextClose(ctx);

	return 0;
}
