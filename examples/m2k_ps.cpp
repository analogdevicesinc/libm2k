#include <libm2k/devicebuilder.hpp>
#include <libm2k/devices.hpp>
#include <libm2k/analog/dmm.hpp>
#include <libm2k/analog/m2kpowersupply.hpp>

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

		std::cout << "############################################################################" << std::endl;
		std::cout << "Detected device " << i << " "  << uri << std::endl;
		auto device = DeviceBuilder::deviceOpen(uri.c_str());

		auto device_m2k = device->toM2k();
		auto ps = device_m2k->getPowerSupply();
		ps->pushChannel(0, -1);
		ps->enableChannel(0, true);
		ps->enableChannel(1, true);
		std::cout << "value read " << ps->readChannel(0) << std::endl;
		try {
			ps->pushChannel(1, -2);
			std::cout << "value read " << ps->readChannel(1) << std::endl;
		} catch (std::exception &e) {
			std::cout << "ERROR " << e.what() << std::endl;
		}

		if (!detectDevices) {
			break;
		} else {
			i++;
		}
	}
	return 0;
}


