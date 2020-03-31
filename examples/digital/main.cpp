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

#include <iostream>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/digital/m2kdigital.hpp>
#include <bitset>

using namespace std;
using namespace libm2k;
using namespace libm2k::digital;
using namespace libm2k::context;

#define N_BITS (4)

int main()

{
	M2k *ctx = m2kOpen();
	if (!ctx) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
		return 1;
	}

	M2kDigital *dig = ctx->getDigital();

	dig->setSampleRateIn(100000);
	dig->setSampleRateOut(100000);

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

	bufferIn = dig->getSamples(1000);
	for(auto val : bufferIn)
	{
		cout<<bitset<16>(val)<<endl;
	}
	dig->stopBufferOut();
	contextClose(ctx);

	return 0;
}
