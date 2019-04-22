#include <libm2k/contextbuilder.hpp>
#include <libm2k/devices.hpp>
#include <libm2k/analog/dmm.hpp>
#include <libm2k/m2kexceptions.hpp>

int main (int argc, char **argv)
{
	std::exception e;
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

		std::cout << "############################################################################" << std::endl;
		std::cout << "Detected device " << i << " "  << uri << std::endl;
		auto device = ContextBuilder::deviceOpen(uri.c_str());

		std::cout << device->getContextDescription() << std::endl;
		auto dmms = device->getAllDmm();
		for (auto dmm : dmms) {
			std::cout << "###  IIO Device name: " << dmm->getName() << std::endl;
			std::cout << "READ all channels: " << std::endl;
			auto readings = dmm->readAll();
			for (auto r : readings) {
				std::cout << "	" << r.name << " " << r.value << " " << r.unit;
			}

			__try {
				auto r = dmm->readChannel("voltage0");
				std::cout << "READ one channel: " <<  r.name << " " << r.value << " " << r.unit;
			} __catch (exception_type &e) {
				std::cout << e.what() << std::endl;
			}
		}

		if (!detectDevices) {
			break;
		} else {
			i++;
		}
	}
	return 0;
}


