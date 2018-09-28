
#ifndef INSTALLED_DEVICES_H
#define INSTALLED_DEVICES_H

#include <map>
#include <string>

#include "libm2k/genericdevice.hpp"

// generated includes for plugins

#include "devices/FMCOMMS/fmcomms.hpp"
#include "devices/M2K/m2k.hpp"

using namespace libm2k::devices;

enum DeviceTypes {
	DevFMCOMMS,
	DevM2K,

	Other
};

typedef std::map<std::string, DeviceTypes> device_name_to_type_map;

device_name_to_type_map dev_map = {
 {"FMCOMMS", DeviceTypes::DevFMCOMMS},
 {"M2K", DeviceTypes::DevM2K}
};

DeviceTypes get_type_from_name(std::string name)
{
	DeviceTypes t = DeviceTypes::Other;
	try {
		t = dev_map.at(name);
		return t;
	} catch (std::out_of_range e){

	}
	return t;
}

GenericDevice* buildDevice(std::string name) // enum Device Name
{
	DeviceTypes t = get_type_from_name(name);
	switch (t) {
//case DevFMCOMMS: return new FMCOMMS();//case DevM2K: return new M2K();
	case Other: return new GenericDevice("", nullptr, "");
	}
}

#endif // INSTALLED_DEVICES_H

