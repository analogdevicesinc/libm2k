#ifndef M2KEXCEPTIONS_H
#define M2KEXCEPTIONS_H

#include "m2kglobal.h"

#include <stdexcept>
#include <string>




class LIBM2K_API no_device_exception: public std::runtime_error {
public:
	explicit no_device_exception(const std::string& what) :
		runtime_error(what) {}
	explicit no_device_exception(const char* what) :
		runtime_error(what) {}
	~no_device_exception() {}
};

class LIBM2K_API instrument_already_in_use_exception : public std::runtime_error {
	explicit instrument_already_in_use_exception(const std::string& what) :
		runtime_error(what) {}
	explicit instrument_already_in_use_exception(const char* what) :
		runtime_error(what) {}
	~instrument_already_in_use_exception() {}
};

class LIBM2K_API invalid_parameter_exception: public std::runtime_error {
	explicit invalid_parameter_exception(const std::string& what) :
		runtime_error(what) {}
	explicit invalid_parameter_exception(const char* what) :
		runtime_error(what) {}
	~invalid_parameter_exception() {}
};

class LIBM2K_API timeout_exception: public std::runtime_error {
	explicit timeout_exception(const std::string& what) :
		runtime_error(what) {}
	explicit timeout_exception(const char* what) :
		runtime_error(what) {}
	~timeout_exception() {}
};



#endif // M2KEXCEPTIONS_H
