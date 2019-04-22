#include "libm2k/contextbuilder.hpp"
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

	auto lst = ContextBuilder::listDevices();
	int i = 1;

	while ((i <= lst.size()) || !detectDevices) {
		if (detectDevices) {
			uri = lst.at(i - 1);
		}

		std::cout << "Detected device " << i << " "  << uri << std::endl;
		auto gen_device = ContextBuilder::deviceOpen(uri.c_str());
		auto m2k_device = gen_device->toM2k();
		if (!m2k_device) {
			std::cerr << "Could not find M2K device, try the generic usage\n" << std::endl;
			return 1;
		}

		std::cout << m2k_device->getContextDescription() << std::endl;

		if (!detectDevices) {
			break;
		} else {
			i++;
		}
	}
	return 0;
}


