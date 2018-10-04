#ifndef M2K_H
#define M2K_H

#include "libm2k/m2kglobal.hpp"
#include "libm2k/genericdevice.hpp"

#include <iostream>
#include <iio.h>

namespace libm2k {
namespace devices {
class LIBM2K_API M2K : public libm2k::devices::GenericDevice
{
public:
	M2K(std::string uri, iio_context* ctx, std::string name);
	~M2K();
	void scanAllAnalogIn();
};
}
}
#endif // M2K_H
