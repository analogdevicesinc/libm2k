#include "libm2k/devicebuilder.hpp"
#include "libm2k/devices.hpp"

int main (int argc, char **argv)
{
	bool detectDevices = false;
	std::string uri = "";
	if (argc < 2) {
		std::cout << "Usage: ./generic_connect <ip:xxx.xxx.xxx.xxx or usb:x.x.x>\n"
			     "Trying to use the detected USB devices..." << std::endl;
		detectDevices = true;
	} else {
		uri = argv[1];
	}

	auto lst = DeviceBuilder::listDevices();
	int i = 1;

	while ((i <= lst.size()) || !detectDevices) {
		if (detectDevices) {
			uri = lst.at(i - 1);
		}

		std::cout << "Detected device " << i << " "  << uri << std::endl;
		auto device = DeviceBuilder::deviceOpen(uri.c_str());

		std::cout << device->getContextDescription() << std::endl;

		if (!detectDevices) {
			break;
		} else {
			i++;
		}
	}

	return 0;
}


