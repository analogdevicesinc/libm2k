#ifndef GENERICDEVICE_HPP
#define GENERICDEVICE_HPP

#include "m2kglobal.hpp"
#include <string>
#include <iio.h>
#include "analogin.hpp"


namespace libm2k {
namespace analog {
	class GenericAnalogIn;
}

namespace devices {
class LIBM2K_API GenericDevice {
public:
	enum DEVICE_TYPE {
		ANALOG = 1,
		DIGITAL = 2,
		NONE = 3
	};

	enum DEVICE_DIRECTION {
		INPUT = 1,
		OUTPUT = 2,
		BOTH = 3,
		NO_DIRECTION = 4
	};

	GenericDevice(std::string uri, struct iio_context*, std::string name);
	virtual ~GenericDevice();

	virtual void scanAllAnalogIn();
	static GenericDevice* getDevice(std::string uri);

	libm2k::analog::GenericAnalogIn* getAnalogIn(int);
	libm2k::analog::GenericAnalogIn* getAnalogIn(std::string);
	void blinkLed();
	struct iio_context* ctx();
protected:
	static std::vector<libm2k::analog::GenericAnalogIn*> s_instancesAnalogIn;
private:
	bool isIioDeviceBufferCapable(std::string);
	GenericDevice::DEVICE_TYPE getIioDeviceType(std::string);
	GenericDevice::DEVICE_DIRECTION getIioDeviceDirection(std::string);
	std::string m_uri;
	struct iio_context* m_ctx;

};
}
}

#endif // GENERICDEVICE_HPP
