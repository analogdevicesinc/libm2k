#include <iostream>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/analog/m2kanalogin.hpp>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::devices;

int main(int argc, char* argv[])
{
	M2k *ctx = m2kOpen();
	ctx->calibrateADC();

	// Setup analog in
	M2kAnalogIn *ain = ctx->getAnalogIn();
	ain->enableChannel(0, true);

	double voltage = ain->getVoltage(0);
	cout << voltage;
	return 0;
}
