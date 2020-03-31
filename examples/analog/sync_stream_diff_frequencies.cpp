/*
 * Streaming on both channels with different sampling frequencies can cause filling all kernel buffers for one channel,
 * blocking the push function for both channels. This example will illustrate a method of avoiding blocking.
 */

#include <cmath>
#include <algorithm>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kanalogout.hpp>
#include <thread>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::context;

constexpr float pi = 3.14;
std::vector<double> sampleRates = {75000, 7500};
std::vector<unsigned int> kernelBuffers = {20, 32};


void
pushNTimes(M2kAnalogOut *analogOut, unsigned int channelIndex, double *samples, unsigned int nbSamples, unsigned int n)
{
	for (int i = 0; i < n; i++) {
		analogOut->pushBytes(channelIndex, samples, nbSamples);
	}
}

int main()
{
	M2k *context = m2kOpen();
	if (!context) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
		return 1;
	}

	context->calibrateDAC();

	M2kAnalogOut *analogOut = context->getAnalogOut();

	analogOut->setKernelBuffersCount(0, kernelBuffers[0]);
	analogOut->setKernelBuffersCount(1, kernelBuffers[1]);

	analogOut->setOversamplingRatio(0, 1);
	analogOut->setOversamplingRatio(1, 1);

	analogOut->setSampleRate(0, sampleRates[0]);
	analogOut->setSampleRate(1, sampleRates[1]);

	analogOut->enableChannel(0, true);
	analogOut->enableChannel(1, true);

	//streaming require a non-cyclic buffer
	analogOut->setCyclic(false);

	std::vector<double> samplesSine;
	std::vector<std::vector<double>> samples;

	for (int i = 0; i < 1024; i++) {
		samplesSine.push_back(3 * sin(2 * pi * (i / 1024.0)));
	}

	samples.push_back(samplesSine);
	samples.push_back(samplesSine);

	unsigned int minKernelBuffers = std::min(kernelBuffers[0], kernelBuffers[1]);
	unsigned int minSampleRate = std::min(sampleRates[0], sampleRates[1]);
	unsigned int chnIdxMaxSampleRate = (sampleRates[0] > sampleRates[1]) ? 0 : 1;


	std::vector<unsigned int> numberCycles;
	for (int i = 0; i < 2; i++) {
		unsigned int cycles = minKernelBuffers * (sampleRates[i] / minSampleRate);
		numberCycles.push_back(cycles);
	}

	for (int i = 0; i < minKernelBuffers; i++) {
		//pushing faster than the samples are processed
		analogOut->push(samples);
	}

	unsigned int cyclesLeftToBePushed = numberCycles[chnIdxMaxSampleRate] - minKernelBuffers;

	std::thread thread_ch1(pushNTimes, analogOut, chnIdxMaxSampleRate, samples[chnIdxMaxSampleRate].data(),
			       samples[chnIdxMaxSampleRate].size(), cyclesLeftToBePushed);
	thread_ch1.join();

	std::this_thread::sleep_for(std::chrono::milliseconds(400));
	contextClose(context, false);
}
