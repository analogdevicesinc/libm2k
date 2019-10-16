// This example reads the analog voltage from channel 0 of the analog input

#include <iostream>
#include <libm2k/m2k.hpp>
#include <libm2k/contextbuilder.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>
#include <libm2k/analog/m2kanalogin.hpp>

using namespace std;
using namespace libm2k;
using namespace libm2k::analog;
using namespace libm2k::contexts;

int main(int argc, char* argv[])
{
	M2k *ctx = m2kOpen();
	if (!ctx) {
		std::cout << "Connection Error: No ADALM2000 device available/connected to your PC." << std::endl;
		return 1;
	}

	ctx->calibrateADC();

	// Setup analog in
	M2kAnalogIn *ain = ctx->getAnalogIn();
	ain->enableChannel(0, true);

	double voltage = ain->getVoltage(0);
	cout << voltage;
	return 0;
}
