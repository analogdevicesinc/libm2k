#include <iostream>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/analog/m2kanalogin.hpp>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::devices;

#define PS_CHANNEL (0)
#define PS_VOLTAGE (1.7)

int main()
{
	M2k *ctx = m2kOpen();
	ctx->calibrateADC();

	// Will turn on the power supply if we need smth to measure
	M2kPowerSupply *ps = ctx->getPowerSupply();
	ps->enableChannel(PS_CHANNEL, true);
	ps->pushChannel(PS_CHANNEL, PS_VOLTAGE);

	// Setup analog in
	M2kAnalogIn *ain = ctx->getAnalogIn();
	ain->enableChannel(0, true);

	double voltage = ain->getVoltage(0);
	cout << voltage;
	return 0;
}
