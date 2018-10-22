
#ifndef INSTALLED_DEVICES_H
#define INSTALLED_DEVICES_H

#include <map>
#include <string>
#include <vector>

#include "libm2k/genericdevice.hpp"
#include "libm2k/m2k.hpp"

// generated includes for plugins

#include "libm2k/fmcomms.hpp"
//#include "devices/M2K/m2k.hpp"

using namespace libm2k::devices;

enum DeviceTypes {
	DevFMCOMMS,
	DevM2K,

	Other
};

static std::vector<std::string> devices_ini_file_path = {
	{R"(C:\msys64\home\AlTrifan\libm2k\src\devices\FMCOMMS\fmcomms.ini)"},
	{R"(C:\msys64\home\AlTrifan\libm2k\src\devices\M2K\m2k.ini)"}

};
typedef std::map<std::string, DeviceTypes> device_name_to_type_map;

static device_name_to_type_map dev_map = {
	{"FMCOMMS", DeviceTypes::DevFMCOMMS},
	{"M2K", DeviceTypes::DevM2K}
};

DeviceTypes get_type_from_name(std::string name)
{
	DeviceTypes t = DeviceTypes::Other;
	try {
		t = dev_map.at(name);
		return t;
	} catch (std::out_of_range &e){

	}
	return t;
}

GenericDevice* buildDevice(std::string name, std::string uri,
			struct iio_context* ctx) // enum Device Name
{
	DeviceTypes t = get_type_from_name(name);
	switch (t) {
//		case DevFMCOMMS: return new FMCOMMS(uri, ctx, name);
		case DevM2K: return new M2K(uri, ctx, name);

		case Other:
		default:
		return new GenericDevice(uri, ctx, name);
	}
}

#endif // INSTALLED_DEVICES_H

