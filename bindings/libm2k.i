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
%include "typemaps.i"
%include "stdint.i"

#ifdef COMMUNICATION
#ifdef SWIGCSHARP
%include "arrays_csharp.i"
%apply uint8_t INPUT[] {uint8_t* data}
#endif
#endif

#ifdef SWIGCSHARP
%apply void *VOID_UINT_PTR { unsigned short * getSamplesP}
#endif

#ifndef DOXYGEN
%feature("autodoc", "3");
#endif

%allowexception;

%ignore pushInterleaved;
%ignore pushRawInterleaved;
%ignore pushBytes;
%ignore getVoltageP;
%ignore getVoltageRawP;
%ignore getSamplesRawInterleaved_matlab;
%ignore getSamplesInterleaved_matlab;
%rename(pushBytes) push(unsigned short*, unsigned int);

%ignore buildLoggingMessage;

#ifdef SWIGPYTHON
%typemap(in) short * {
	if (PyBytes_Check($input))
	{
		$1_ltype data = ($1_ltype)PyBytes_AsString($input);
		$1 = data;
	} else if (PyObject_CheckBuffer($input)) {
		Py_buffer view;
		if (PyObject_GetBuffer($input, &view, PyBUF_SIMPLE | PyBUF_C_CONTIGUOUS) == -1)
		{
			PyErr_SetString(PyExc_ValueError, "Failed to get buffer.");
			return NULL;
		}
		$1 = ($1_ltype)view.buf;

	} else {
		PyErr_SetString(PyExc_ValueError, "Expecting bytearray\n");
	}
}
/* Apply all of the integer typemaps to double* and unsigned short * */
%apply short * { unsigned short * };


%typemap(out) short * getSamplesRawInterleaved {
	auto iio_obj = arg1->getIioObjects();
	auto buf = iio_obj.buffers_rx[0];
	if (buf) {
		char* start = (char*)(iio_buffer_start(buf));
		char* end = (char*)(iio_buffer_end(buf));
		auto size = (end - start);

		// This is used when returning double*
		if (sizeof($*1_ltype) != sizeof(short)) {
			size = size * sizeof(float);
		}
		auto memory_view = PyMemoryView_FromMemory(start, size, PyBUF_WRITE);
		auto res = PyMemoryView_GetContiguous(memory_view, PyBUF_READ, 'F');
		$result = res;
	}
}
%apply short * getSamplesRawInterleaved {short * getSamplesInterleaved};
%apply short * getSamplesRawInterleaved {double * getSamplesInterleaved};
%apply short * getSamplesRawInterleaved {unsigned short * getSamplesP};

#endif


#ifdef SWIGPYTHON
#ifdef COMMUNICATION
%include <pybuffer.i>
%pybuffer_mutable_binary(uint8_t *data, uint8_t bytes_number);
%pybuffer_mutable_binary(uint8_t *data, uint32_t bytes_number);
%pybuffer_mutable_binary(const uint8_t *data, uint32_t bytes_number);
#endif
#endif

namespace std {
	%template(VectorI) vector<int>;
	%template(VectorS) vector<short>;
	%template(VectorUS) vector<unsigned short>;
	%template(VectorD) vector<double>;
	%template(VectorStr) vector<string>;
	%template(VectorVectorD) vector< vector<double> >;
	%template(VectorVectorS) vector< vector<short> >;
	%template(VectorVectorI) vector< vector<int> >;
	%template(VectorVectorUS) vector< vector<unsigned short> >;
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

	#include <libm2k/digital/enums.hpp>
	#include <libm2k/digital/m2kdigital.hpp>

	#include <libm2k/context.hpp>
	#include <libm2k/contextbuilder.hpp>
	#include <libm2k/fmcomms.hpp>
	#include <libm2k/logger.hpp>
	#include <libm2k/m2kcalibration.hpp>
	#include <libm2k/m2kexceptions.hpp>
	#include <libm2k/m2k.hpp>
	#include <libm2k/generic.hpp>
#ifdef COMMUNICATION
	#include <libm2k/tools/spi.hpp>
	#include <libm2k/tools/spi_extra.hpp>
	#include <libm2k/tools/i2c.hpp>
	#include <libm2k/tools/i2c_extra.hpp>
	#include <libm2k/tools/uart.hpp>
	#include <libm2k/tools/uart_extra.hpp>
#endif
	typedef std::vector<libm2k::analog::DMM_READING> DMMReading;
	typedef std::vector<libm2k::analog::DMM*> DMMs;
	typedef std::vector<libm2k::analog::M2kAnalogIn*> M2kAnalogIns;
	typedef std::vector<libm2k::analog::M2kAnalogOut*> M2kAnalogOuts;
	typedef std::vector<libm2k::M2K_TRIGGER_CONDITION_ANALOG> M2kConditionAnalog;
	typedef std::vector<libm2k::M2K_TRIGGER_CONDITION_DIGITAL> M2kConditionDigital;
	typedef std::vector<libm2k::M2K_TRIGGER_MODE> M2kModes;
	typedef std::vector<libm2k::CONTEXT_INFO*> VectorCtxInfo;
%}

#ifdef COMMUNICATION
#ifdef SWIGCSHARP
	typedef struct spi_init_param {
		uint32_t 	max_speed_hz;
		uint8_t 	chip_select;
		enum 		spi_mode mode;
		m2k_spi_init 	*extra;
	}spi_init_param;

	typedef struct i2c_init_param {
		uint32_t 	max_speed_hz;
		uint16_t 	slave_address;
		m2k_i2c_init 	*extra;
	} i2c_init_param;

	typedef struct uart_init_param {
		uint8_t		device_id;
		uint32_t 	baud_rate;
		m2k_uart_init 	*extra;
	}uart_init_param;
#endif

%ignore spi_init(struct spi_desc**, const struct spi_init_param *);
%ignore i2c_init(struct i2c_desc**, const struct i2c_init_param *);
%ignore uart_init(struct uart_desc**, const struct uart_init_param *);

%inline %{
	spi_desc *spi_init(const struct spi_init_param *param)
	{
		spi_desc *temp;
		spi_init(&temp, param);
		return temp;
	}

	i2c_desc *i2c_init(const struct i2c_init_param *param)
	{
		i2c_desc *temp;
		i2c_init(&temp, param);
		return temp;
	}

	uart_desc *uart_init(const struct uart_init_param *param)
	{
		uart_desc *temp;
		uart_init(&temp, param);
		return temp;
	}
%}
#endif


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

%include <libm2k/digital/enums.hpp>
%include <libm2k/digital/m2kdigital.hpp>

%include <libm2k/context.hpp>
%include <libm2k/contextbuilder.hpp>
%include <libm2k/fmcomms.hpp>
%include <libm2k/logger.hpp>
%include <libm2k/m2kcalibration.hpp>
%include <libm2k/m2kexceptions.hpp>
%include <libm2k/m2k.hpp>
%include <libm2k/generic.hpp>

#ifdef COMMUNICATION
%include <libm2k/tools/spi.hpp>
%include <libm2k/tools/spi_extra.hpp>
%include <libm2k/tools/i2c.hpp>
%include <libm2k/tools/i2c_extra.hpp>
%include <libm2k/tools/uart.hpp>
%include <libm2k/tools/uart_extra.hpp>
#endif

%template(DMMReading) std::vector<libm2k::analog::DMM_READING>;
%template(DMMs) std::vector<libm2k::analog::DMM*>;
%template(M2kAnalogIns) std::vector<libm2k::analog::M2kAnalogIn*>;
%template(M2kAnalogOuts) std::vector<libm2k::analog::M2kAnalogOut*>;
%template(M2kConditionAnalog) std::vector<libm2k::M2K_TRIGGER_CONDITION_ANALOG>;
%template(M2kConditionDigital) std::vector<libm2k::M2K_TRIGGER_CONDITION_DIGITAL>;
%template(M2kModes) std::vector<libm2k::M2K_TRIGGER_MODE>;
%template(VectorCtxInfo) std::vector<libm2k::CONTEXT_INFO*>;

#ifdef SWIGPYTHON
	%template(IioBuffers) std::vector<iio_buffer*>;
	%template(IioChannels) std::vector<iio_channel*>;
	%template(IioDevices) std::vector<iio_device*>;
#endif
