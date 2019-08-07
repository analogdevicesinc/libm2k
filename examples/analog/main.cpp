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
	ain->enableChannel(0, true);
	ain->enableChannel(1, true);
	ain->setSampleRate(100000);
	ain->setRange((ANALOG_IN_CHANNEL)0,-10.0,10.0); // nu are idxchannel
	ain->setRange((ANALOG_IN_CHANNEL)1,PLUS_MINUS_25V);

	// setup analog trigger
	trig->setAnalogSource(CHANNEL_1);
	trig->setAnalogCondition(0,RISING_EDGE);
	trig->setAnalogLevel(0, 0.5);
	trig->setAnalogDelay(0);
	trig->setAnalogMode(0,ANALOG);

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
	deviceClose(ctx);
}
