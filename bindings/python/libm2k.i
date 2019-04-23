#define SWIG_ON 1

%module libm2k
%include "std_list.i"
%include "std_map.i"
%include "std_pair.i"
%include "std_set.i"
%include "std_string.i"
%include "std_vector.i"

namespace std {
	%template(VectorS) vector<int>;
	%template(VectorD) vector<double>;
	%template(VectorVectorD) vector<vector<double>>;
	%template(VectorVectorS) vector<vector<int>>;
}

%{
	#include <libm2k/m2kglobal.hpp>
	#include <libm2k/utils/buffer.hpp>
	#include <libm2k/utils/channel.hpp>
	#include <libm2k/utils/device.hpp>
	#include <libm2k/utils/enums.hpp>
	#include <libm2k/utils/utils.hpp>

	#include <libm2k/analog/dmm.hpp>
	#include <libm2k/analog/enums.hpp>
	#include <libm2k/analog/genericanalogin.hpp>
	#include <libm2k/analog/genericanalogout.hpp>
	#include <libm2k/analog/m2kanalogin.hpp>
	#include <libm2k/analog/m2kanalogout.hpp>
	#include <libm2k/analog/m2khardwaretrigger.hpp>
	#include <libm2k/analog/m2kpowersupply.hpp>
	#include <libm2k/analog/powersupply.hpp>

	#include <libm2k/digital/genericdigital.hpp>
	#include <libm2k/digital/enums.hpp>
	#include <libm2k/digital/m2kdigital.hpp>

	#include <libm2k/context.hpp>
	#include <libm2k/contextbuilder.hpp>
	#include <libm2k/devices.hpp>
	#include <libm2k/enums.hpp>
	#include <libm2k/fmcomms.hpp>
	#include <libm2k/logger.hpp>
	#include <libm2k/m2kcalibration.hpp>
	#include <libm2k/m2kexceptions.hpp>
	#include <libm2k/m2k.hpp>
%}

%include <std_shared_ptr.i>
%include <libm2k/m2kglobal.hpp>
%include <libm2k/utils/buffer.hpp>
%include <libm2k/utils/channel.hpp>
%include <libm2k/utils/device.hpp>
%include <libm2k/utils/enums.hpp>
%include <libm2k/utils/utils.hpp>

%include <libm2k/analog/dmm.hpp>
%include <libm2k/analog/enums.hpp>
%include <libm2k/analog/genericanalogin.hpp>
%include <libm2k/analog/genericanalogout.hpp>
%include <libm2k/analog/m2kanalogin.hpp>
%include <libm2k/analog/m2kanalogout.hpp>
%include <libm2k/analog/m2khardwaretrigger.hpp>
%include <libm2k/analog/m2kpowersupply.hpp>
%include <libm2k/analog/powersupply.hpp>

%include <libm2k/digital/genericdigital.hpp>
%include <libm2k/digital/enums.hpp>
%include <libm2k/digital/m2kdigital.hpp>

%include <libm2k/context.hpp>
%include <libm2k/contextbuilder.hpp>
%include <libm2k/devices.hpp>
%include <libm2k/enums.hpp>
%include <libm2k/fmcomms.hpp>
%include <libm2k/logger.hpp>
%include <libm2k/m2kcalibration.hpp>
%include <libm2k/m2kexceptions.hpp>
%include <libm2k/m2k.hpp>


