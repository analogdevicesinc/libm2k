#include "../include/libm2k/M2K.h"
#include <iostream>

using namespace m2k;
int main(int argc, char **argv)
{
	std::vector<std::string> lst = DeviceBuilder::listDevices();
	if (lst.size() > 0) {
		DeviceBuilder *d = DeviceBuilder::deviceOpen(lst.at(0).c_str());
		if (d) {
			DeviceBuilder::deviceClose(d);
		}
	}
	return 0;
}

