#define SWIG_ON 1
%module libm2k
#ifdef __WIN32__
	%include <windows.i>
#endif
%include "std_map.i"
%include "std_pair.i"
%include "std_string.i"
%include "std_vector.i"
%include "exception.i"
%allowexception;
%feature("autodoc", "3");
namespace std {
	%template(VectorI) vector<int>;
	%template(VectorS) vector<short>;
	%template(VectorUS) vector<unsigned short>;
	%template(VectorD) vector<double>;
	%template(VectorStr) vector<string>;
	%template(VectorVectorD) vector< vector<double> >;
	%template(VectorVectorS) vector< vector<int> >;
	%template(PairDD) std::pair<double, double>;
	%template(VectorPairDD) std::vector<std::pair<std::string, std::pair <double, double>>>;
}

%{

	extern "C" {
		struct iio_context;
	}

	#include <libm2k/m2kglobal.hpp>
	#include <libm2k/enums.hpp>
	#include <libm2k/utils/enums.hpp>
	#include <libm2k/analog/dmm.hpp>
	#include <libm2k/analog/enums.hpp>
	#include <libm2k/analog/genericanalogin.hpp>
	#include <libm2k/analog/genericanalogout.hpp>
	#include <libm2k/m2khardwaretrigger.hpp>
	#include <libm2k/analog/m2kanalogin.hpp>
	#include <libm2k/analog/m2kanalogout.hpp>
	#include <libm2k/analog/m2kpowersupply.hpp>
	#include <libm2k/analog/powersupply.hpp>

	#include <libm2k/digital/genericdigital.hpp>
	#include <libm2k/digital/enums.hpp>
	#include <libm2k/digital/m2kdigital.hpp>

	#include <libm2k/context.hpp>
	#include <libm2k/contextbuilder.hpp>
	#include <libm2k/fmcomms.hpp>
	#include <libm2k/logger.hpp>
	#include <libm2k/m2kcalibration.hpp>
	#include <libm2k/m2kexceptions.hpp>
	#include <libm2k/m2k.hpp>
	typedef std::vector<libm2k::analog::DMM_READING> DMMReading;
	typedef std::vector<libm2k::analog::DMM*> DMMs;
	typedef std::vector<libm2k::analog::M2kAnalogIn*> M2kAnalogIns;
	typedef std::vector<libm2k::analog::M2kAnalogOut*> M2kAnalogOuts;
	typedef std::vector<libm2k::M2K_TRIGGER_CONDITION_ANALOG> M2kConditionAnalog;
	typedef std::vector<libm2k::M2K_TRIGGER_CONDITION_DIGITAL> M2kConditionDigital;
	typedef std::vector<libm2k::M2K_TRIGGER_MODE> M2kModes;
%}

#ifdef SWIGPYTHON
	%exception {
		try {
			$action
		} catch (exception_type &e) {
			std::string s("Module libm2k error: "), s2(e.what());
			s = s + s2;
			PyErr_SetString(PyExc_ValueError, s.c_str());
			return NULL;
		}
	}
#endif

#ifdef SWIGCSHARP
	%exception {
		try {
			$action
		} catch (exception_type &e) {
			std::string s("Module libm2k error: "), s2(e.what());
			s = s + s2;
			SWIG_CSharpSetPendingException(SWIG_CSharpApplicationException, s.c_str());
		}
	}
#endif

%include <std_shared_ptr.i>
%include <libm2k/m2kglobal.hpp>
%include <libm2k/enums.hpp>
%include <libm2k/utils/enums.hpp>
%include <libm2k/analog/dmm.hpp>
%include <libm2k/analog/enums.hpp>
%include <libm2k/analog/genericanalogin.hpp>
%include <libm2k/analog/genericanalogout.hpp>
%include <libm2k/m2khardwaretrigger.hpp>
%include <libm2k/analog/m2kanalogin.hpp>
%include <libm2k/analog/m2kanalogout.hpp>
%include <libm2k/analog/m2kpowersupply.hpp>
%include <libm2k/analog/powersupply.hpp>

%include <libm2k/digital/genericdigital.hpp>
%include <libm2k/digital/enums.hpp>
%include <libm2k/digital/m2kdigital.hpp>

%include <libm2k/context.hpp>
%include <libm2k/contextbuilder.hpp>
%include <libm2k/fmcomms.hpp>
%include <libm2k/logger.hpp>
%include <libm2k/m2kcalibration.hpp>
%include <libm2k/m2kexceptions.hpp>
%include <libm2k/m2k.hpp>

%template(DMMReading) std::vector<libm2k::analog::DMM_READING>;
%template(DMMs) std::vector<libm2k::analog::DMM*>;
%template(M2kAnalogIns) std::vector<libm2k::analog::M2kAnalogIn*>;
%template(M2kAnalogOuts) std::vector<libm2k::analog::M2kAnalogOut*>;
%template(M2kConditionAnalog) std::vector<libm2k::M2K_TRIGGER_CONDITION_ANALOG>;
%template(M2kConditionDigital) std::vector<libm2k::M2K_TRIGGER_CONDITION_DIGITAL>;
%template(M2kModes) std::vector<libm2k::M2K_TRIGGER_MODE>;
