/*
 * Copyright (c) 2024 Analog Devices Inc.
 *
 * This file is part of libm2k
 * (see http://www.github.com/analogdevicesinc/libm2k).
 *
 * This program is free software; you can redistribute it and/or modify
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

#include <cmath>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <thread>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::context;

constexpr float pi = 3.14;
constexpr double sampleRate = 75000;
constexpr unsigned int buffersCount = 32;


int main()
{
	M2k *context = m2kOpen();
	if (!context) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
		return 1;
	}

	M2kAnalogOut *analogOut = context->getAnalogOut();

	// Prevent bad initial config
	analogOut->reset();

	context->calibrateDAC();

	analogOut->setKernelBuffersCount(0, buffersCount);
	analogOut->setKernelBuffersCount(1, buffersCount);

	analogOut->setOversamplingRatio(0, 1);
	analogOut->setOversamplingRatio(1, 1);

	analogOut->setSampleRate(0, sampleRate);
	analogOut->setSampleRate(1, sampleRate);

	analogOut->enableChannel(0, true);
	analogOut->enableChannel(1, true);

	//streaming require a non-cyclic buffer
	analogOut->setCyclic(false);

	std::vector<double> samplesSine;
	std::vector<double> samplesSquare;

	for (int i = 0; i < 1024; i++) {
		samplesSine.push_back(3 * sin(2 * pi * (i / 1024.0)));
	}

	for (int i = 0; i < 1024 / 2; i++) {
		samplesSquare.push_back(3);
	}
	for (int i = 0; i < 1024 / 2; i++) {
		samplesSquare.push_back(0);
	}

	for (int i = 0; i < 64; i++) {
		//pushing faster than the samples are processed
		analogOut->push(0, samplesSine);
		analogOut->push(0, samplesSquare);
	}
	std::this_thread::sleep_for(std::chrono::seconds(2));
	contextClose(context, false);
}
