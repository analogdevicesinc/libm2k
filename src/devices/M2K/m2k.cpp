#include "m2k.hpp"
#include "m2kanalogin.hpp"
#include "libm2k/m2kexceptions.hpp"
#include <iostream>

using namespace std;
using namespace libm2k::devices;
using namespace libm2k::analog;

M2K::M2K(std::string uri, iio_context* ctx, std::string name) :
	GenericDevice(uri, ctx, name)
{
	std::cout << "I am M2K device " << std::endl;

	/* Initialize the AnalogIn list */
	for (auto aIn : s_instancesAnalogIn) {
		delete aIn;
	}
	s_instancesAnalogIn.clear();
	scanAllAnalogIn();
}

M2K::~M2K()
{

}

void M2K::scanAllAnalogIn()
{
	try {
		GenericAnalogIn* aIn = new M2kAnalogIn(ctx(), "m2k-adc");
		s_instancesAnalogIn.push_back(aIn);
	} catch (std::runtime_error& e) {
		std::cout << e.what() << std::endl;
	}
}

