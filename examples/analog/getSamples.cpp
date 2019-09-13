#include <iostream>
#include <chrono>

#include <math.h>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/analog/m2kanalogin.hpp>
#include <libm2k/analog/m2kanalogout.hpp>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::devices;
#define M_PI 3.14
int main(int argc, char* argv[])
{
	M2k *ctx = m2kOpen();
	ctx->calibrateADC();
	ctx->calibrateDAC();

	M2kAnalogIn *ain = ctx->getAnalogIn();
	M2kAnalogOut *aout = ctx->getAnalogOut();
	M2kHardwareTrigger *trig = ain->getTrigger();

	// Setup analog in

	// Play with enable/disable channels to see changes in all the getSamples methods.
	ain->enableChannel(0, false);
	ain->enableChannel(1, true);


	ain->setSampleRate(100000);
	ain->setRange((ANALOG_IN_CHANNEL)0,-10.0,10.0); // nu are idxchannel
	ain->setRange((ANALOG_IN_CHANNEL)1,PLUS_MINUS_25V);

	// setup analog trigger
	trig->setAnalogSource(CHANNEL_1);
	trig->setAnalogCondition(0,RISING_EDGE);
	trig->setAnalogLevel(0, 0.5);
	trig->setAnalogDelay(0);
	trig->setAnalogMode(0,ALWAYS);

	// setup analog output
	aout->setSampleRate(0,750000);
	aout->setSampleRate(1,750000);
	aout->enableChannel(0, true);
	aout->enableChannel(1, true);

	// create output buffers
	vector<double> ch1;
	vector<double> ch2;

	//  Push 3V constant on the first channel and 4V constant on the second one
	for(int i=0;i<1024;i++)
	{
		ch1.push_back(3.0);
		ch2.push_back(4.0);
	}

	aout->setCyclic(true);
	aout->push({ch1, ch2});


	auto d = ain->getSamples(10);

	// Use all the provided methods to acquire samples for the analog side
	// Compare the values and the time

	auto t1 = std::chrono::high_resolution_clock::now();
	// Get Voltage for all the enabled channels
	std::vector<std::vector<double>> d1 = ain->getSamples(10);
	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
	std::cout << "\nTime - get V - std vector: " << duration << "ms" <<  endl;


	auto t3 = std::chrono::high_resolution_clock::now();
	// Get Raw values for all the enabled channels
	std::vector<std::vector<double>> d2 = ain->getSamplesRaw(10);
	auto t4 = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>( t4 - t3 ).count();
	std::cout << "\nTime - get raw - std vector: " << duration << "ms" <<  endl;


	auto t5 = std::chrono::high_resolution_clock::now();
	// Get Voltage Interleaved for all the enabled channels
	const double *d3 = ain->getSamplesInterleaved(10);
	auto t6 = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>( t6 - t5 ).count();
	std::cout << "\nTime - get V pointer: " << duration << "ms" <<  endl;


	auto t7 = std::chrono::high_resolution_clock::now();
	// Get Raw interleaved for ALL the channels
	// This is the most efficient, it does not make any copies or processing
	const short *d4 = ain->getSamplesRawInterleaved(10);
	auto t8 = std::chrono::high_resolution_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>( t8 - t7 ).count();
	std::cout << "\nTime - get raw pointer: " << duration << "ms" << endl;

	cout << endl <<  "\nVoltage CHANNEL1" << endl;

	for (int i = 0 ; i < 10; i++) {
		double val = d1[0][i];
		cout << val << " ";
	}

	cout << endl << "\nRaw CHANNEL1" << endl;

	for (int i = 0 ; i < 10; i++) {
		double val = d2[0][i];
		cout << val << " ";
	}

	cout <<  endl << "\nVoltage CHANNEL1 and CHANNEL2 interleaved" << endl;

	for (int i = 0 ; i < 10; i++) {
		double val = d3[i];
		cout << val << " ";
	}

	cout <<  endl << "\nRaw CHANNEL1 and CHANNEL2 interleaved" << endl;

	for (int i = 0 ; i < 10; i++) {
		short val = d4[i];
		cout << val << " ";
	}

	aout->stop();
	delete [] d3;
	deviceClose(ctx);
}
