%% About definelibm2k.mlx
% This file defines the MATLAB interface to the library |libm2k|.
%
% Commented sections represent C++ functionality that MATLAB cannot automatically define. To include
% functionality, uncomment a section and provide values for &lt;SHAPE&gt;, &lt;DIRECTION&gt;, etc. For more
% information, see <matlab:helpview(fullfile(docroot,'matlab','helptargets.map'),'cpp_define_interface') Define MATLAB Interface for C++ Library>.



%% Setup. Do not edit this section.
function libDef = definelibm2k()
libDef = clibgen.LibraryDefinition("libm2kData.xml");
%% OutputFolder and Libraries 
libDef.OutputFolder = "/tmp/libm2k/bindings/matlab";
libDef.Libraries = "/usr/local/lib/libm2k.so";

%% C++ class |iio_context| with MATLAB name |clib.libm2k.iio_context| 
% iio_contextDefinition = addClass(libDef, "iio_context", "MATLABName", "clib.libm2k.iio_context", ...
%     "Description", "clib.libm2k.iio_context    Representation of C++ class iio_context"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |iio_channel| with MATLAB name |clib.libm2k.iio_channel| 
% iio_channelDefinition = addClass(libDef, "iio_channel", "MATLABName", "clib.libm2k.iio_channel", ...
%     "Description", "clib.libm2k.iio_channel    Representation of C++ class iio_channel"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |iio_device| with MATLAB name |clib.libm2k.iio_device| 
% iio_deviceDefinition = addClass(libDef, "iio_device", "MATLABName", "clib.libm2k.iio_device", ...
%     "Description", "clib.libm2k.iio_device    Representation of C++ class iio_device"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |DeviceTypes| with MATLAB name |clib.libm2k.DeviceTypes| 
addEnumeration(libDef, "DeviceTypes", "int32",...
    [...
      "DevFMCOMMS",...  % 0
      "DevM2K",...  % 1
      "Other",...  % 2
    ],...
    "MATLABName", "clib.libm2k.DeviceTypes", ...
    "Description", "clib.libm2k.DeviceTypes    Representation of C++ enumeration DeviceTypes"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |iio_buffer| with MATLAB name |clib.libm2k.iio_buffer| 
% iio_bufferDefinition = addClass(libDef, "iio_buffer", "MATLABName", "clib.libm2k.iio_buffer", ...
%     "Description", "clib.libm2k.iio_buffer    Representation of C++ class iio_buffer"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |iio_context_info| with MATLAB name |clib.libm2k.iio_context_info| 
% iio_context_infoDefinition = addClass(libDef, "iio_context_info", "MATLABName", "clib.libm2k.iio_context_info", ...
%     "Description", "clib.libm2k.iio_context_info    Representation of C++ class iio_context_info"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |iio_scan_context| with MATLAB name |clib.libm2k.iio_scan_context| 
% iio_scan_contextDefinition = addClass(libDef, "iio_scan_context", "MATLABName", "clib.libm2k.iio_scan_context", ...
%     "Description", "clib.libm2k.iio_scan_context    Representation of C++ class iio_scan_context"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |iio_chan_type| with MATLAB name |clib.libm2k.iio_chan_type| 
addEnumeration(libDef, "iio_chan_type", "int32",...
    [...
      "IIO_VOLTAGE",...  % 0
      "IIO_CURRENT",...  % 1
      "IIO_POWER",...  % 2
      "IIO_ACCEL",...  % 3
      "IIO_ANGL_VEL",...  % 4
      "IIO_MAGN",...  % 5
      "IIO_LIGHT",...  % 6
      "IIO_INTENSITY",...  % 7
      "IIO_PROXIMITY",...  % 8
      "IIO_TEMP",...  % 9
      "IIO_INCLI",...  % 10
      "IIO_ROT",...  % 11
      "IIO_ANGL",...  % 12
      "IIO_TIMESTAMP",...  % 13
      "IIO_CAPACITANCE",...  % 14
      "IIO_ALTVOLTAGE",...  % 15
      "IIO_CCT",...  % 16
      "IIO_PRESSURE",...  % 17
      "IIO_HUMIDITYRELATIVE",...  % 18
      "IIO_ACTIVITY",...  % 19
      "IIO_STEPS",...  % 20
      "IIO_ENERGY",...  % 21
      "IIO_DISTANCE",...  % 22
      "IIO_VELOCITY",...  % 23
      "IIO_CONCENTRATION",...  % 24
      "IIO_RESISTANCE",...  % 25
      "IIO_PH",...  % 26
      "IIO_UVINDEX",...  % 27
      "IIO_ELECTRICALCONDUCTIVITY",...  % 28
      "IIO_COUNT",...  % 29
      "IIO_INDEX",...  % 30
      "IIO_GRAVITY",...  % 31
      "IIO_CHAN_TYPE_UNKNOWN",...  % 2147483647
    ],...
    "MATLABName", "clib.libm2k.iio_chan_type", ...
    "Description", "clib.libm2k.iio_chan_type    Representation of C++ enumeration iio_chan_type"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |iio_modifier| with MATLAB name |clib.libm2k.iio_modifier| 
addEnumeration(libDef, "iio_modifier", "int32",...
    [...
      "IIO_NO_MOD",...  % 0
      "IIO_MOD_X",...  % 1
      "IIO_MOD_Y",...  % 2
      "IIO_MOD_Z",...  % 3
      "IIO_MOD_X_AND_Y",...  % 4
      "IIO_MOD_X_AND_Z",...  % 5
      "IIO_MOD_Y_AND_Z",...  % 6
      "IIO_MOD_X_AND_Y_AND_Z",...  % 7
      "IIO_MOD_X_OR_Y",...  % 8
      "IIO_MOD_X_OR_Z",...  % 9
      "IIO_MOD_Y_OR_Z",...  % 10
      "IIO_MOD_X_OR_Y_OR_Z",...  % 11
      "IIO_MOD_LIGHT_BOTH",...  % 12
      "IIO_MOD_LIGHT_IR",...  % 13
      "IIO_MOD_ROOT_SUM_SQUARED_X_Y",...  % 14
      "IIO_MOD_SUM_SQUARED_X_Y_Z",...  % 15
      "IIO_MOD_LIGHT_CLEAR",...  % 16
      "IIO_MOD_LIGHT_RED",...  % 17
      "IIO_MOD_LIGHT_GREEN",...  % 18
      "IIO_MOD_LIGHT_BLUE",...  % 19
      "IIO_MOD_QUATERNION",...  % 20
      "IIO_MOD_TEMP_AMBIENT",...  % 21
      "IIO_MOD_TEMP_OBJECT",...  % 22
      "IIO_MOD_NORTH_MAGN",...  % 23
      "IIO_MOD_NORTH_TRUE",...  % 24
      "IIO_MOD_NORTH_MAGN_TILT_COMP",...  % 25
      "IIO_MOD_NORTH_TRUE_TILT_COMP",...  % 26
      "IIO_MOD_RUNNING",...  % 27
      "IIO_MOD_JOGGING",...  % 28
      "IIO_MOD_WALKING",...  % 29
      "IIO_MOD_STILL",...  % 30
      "IIO_MOD_ROOT_SUM_SQUARED_X_Y_Z",...  % 31
      "IIO_MOD_I",...  % 32
      "IIO_MOD_Q",...  % 33
      "IIO_MOD_CO2",...  % 34
      "IIO_MOD_VOC",...  % 35
      "IIO_MOD_LIGHT_UV",...  % 36
    ],...
    "MATLABName", "clib.libm2k.iio_modifier", ...
    "Description", "clib.libm2k.iio_modifier    Representation of C++ enumeration iio_modifier"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |iio_data_format| with MATLAB name |clib.libm2k.iio_data_format| 
iio_data_formatDefinition = addClass(libDef, "iio_data_format", "MATLABName", "clib.libm2k.iio_data_format", ...
    "Description", "clib.libm2k.iio_data_format    Representation of C++ class iio_data_format"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |iio_data_format| 
% C++ Signature: iio_data_format::iio_data_format()
iio_data_formatConstructor1Definition = addConstructor(iio_data_formatDefinition, ...
    "iio_data_format::iio_data_format()", ...
    "Description", "clib.libm2k.iio_data_format    Constructor of C++ class iio_data_format"); % This description is shown as help to user. Modify it to appropriate description.
validate(iio_data_formatConstructor1Definition);

%% C++ class constructor for C++ class |iio_data_format| 
% C++ Signature: iio_data_format::iio_data_format(iio_data_format const & input1)
iio_data_formatConstructor2Definition = addConstructor(iio_data_formatDefinition, ...
    "iio_data_format::iio_data_format(iio_data_format const & input1)", ...
    "Description", "clib.libm2k.iio_data_format    Constructor of C++ class iio_data_format"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(iio_data_formatConstructor2Definition, "input1", "clib.libm2k.iio_data_format", "input");
validate(iio_data_formatConstructor2Definition);

%% C++ class public data member |length| for C++ class |iio_data_format| 
% C++ Signature: unsigned int iio_data_format::length
addProperty(iio_data_formatDefinition, "length", "uint32", ...
    "Description", "uint32    Data Member of C++ class iio_data_format::length"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class public data member |bits| for C++ class |iio_data_format| 
% C++ Signature: unsigned int iio_data_format::bits
addProperty(iio_data_formatDefinition, "bits", "uint32", ...
    "Description", "uint32    Data Member of C++ class iio_data_format::bits"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class public data member |shift| for C++ class |iio_data_format| 
% C++ Signature: unsigned int iio_data_format::shift
addProperty(iio_data_formatDefinition, "shift", "uint32", ...
    "Description", "uint32    Data Member of C++ class iio_data_format::shift"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class public data member |is_signed| for C++ class |iio_data_format| 
% C++ Signature: bool iio_data_format::is_signed
addProperty(iio_data_formatDefinition, "is_signed", "logical", ...
    "Description", "logical    Data Member of C++ class iio_data_format::is_signed"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class public data member |is_fully_defined| for C++ class |iio_data_format| 
% C++ Signature: bool iio_data_format::is_fully_defined
addProperty(iio_data_formatDefinition, "is_fully_defined", "logical", ...
    "Description", "logical    Data Member of C++ class iio_data_format::is_fully_defined"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class public data member |is_be| for C++ class |iio_data_format| 
% C++ Signature: bool iio_data_format::is_be
addProperty(iio_data_formatDefinition, "is_be", "logical", ...
    "Description", "logical    Data Member of C++ class iio_data_format::is_be"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class public data member |with_scale| for C++ class |iio_data_format| 
% C++ Signature: bool iio_data_format::with_scale
addProperty(iio_data_formatDefinition, "with_scale", "logical", ...
    "Description", "logical    Data Member of C++ class iio_data_format::with_scale"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class public data member |scale| for C++ class |iio_data_format| 
% C++ Signature: double iio_data_format::scale
addProperty(iio_data_formatDefinition, "scale", "double", ...
    "Description", "double    Data Member of C++ class iio_data_format::scale"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class public data member |repeat| for C++ class |iio_data_format| 
% C++ Signature: unsigned int iio_data_format::repeat
addProperty(iio_data_formatDefinition, "repeat", "uint32", ...
    "Description", "uint32    Data Member of C++ class iio_data_format::repeat"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |no_device_exception| with MATLAB name |clib.libm2k.no_device_exception| 
no_device_exceptionDefinition = addClass(libDef, "no_device_exception", "MATLABName", "clib.libm2k.no_device_exception", ...
    "Description", "clib.libm2k.no_device_exception    Representation of C++ class no_device_exception"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |no_device_exception| 
% C++ Signature: no_device_exception::no_device_exception(std::string const & what)
no_device_exceptionConstructor1Definition = addConstructor(no_device_exceptionDefinition, ...
    "no_device_exception::no_device_exception(std::string const & what)", ...
    "Description", "clib.libm2k.no_device_exception    Constructor of C++ class no_device_exception"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(no_device_exceptionConstructor1Definition, "what", "string", "input");
validate(no_device_exceptionConstructor1Definition);

%% C++ class constructor for C++ class |no_device_exception| 
% C++ Signature: no_device_exception::no_device_exception(char const * what)
%no_device_exceptionConstructor2Definition = addConstructor(no_device_exceptionDefinition, ...
%    "no_device_exception::no_device_exception(char const * what)", ...
%    "Description", "clib.libm2k.no_device_exception    Constructor of C++ class no_device_exception"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(no_device_exceptionConstructor2Definition, "what", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(no_device_exceptionConstructor2Definition);

%% C++ class constructor for C++ class |no_device_exception| 
% C++ Signature: no_device_exception::no_device_exception(no_device_exception const & input1)
no_device_exceptionConstructor3Definition = addConstructor(no_device_exceptionDefinition, ...
    "no_device_exception::no_device_exception(no_device_exception const & input1)", ...
    "Description", "clib.libm2k.no_device_exception    Constructor of C++ class no_device_exception"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(no_device_exceptionConstructor3Definition, "input1", "clib.libm2k.no_device_exception", "input");
validate(no_device_exceptionConstructor3Definition);

%% C++ class |instrument_already_in_use_exception| with MATLAB name |clib.libm2k.instrument_already_in_use_exception| 
instrument_already_in_use_exceptionDefinition = addClass(libDef, "instrument_already_in_use_exception", "MATLABName", "clib.libm2k.instrument_already_in_use_exception", ...
    "Description", "clib.libm2k.instrument_already_in_use_exception    Representation of C++ class instrument_already_in_use_exception"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |instrument_already_in_use_exception| 
% C++ Signature: instrument_already_in_use_exception::instrument_already_in_use_exception(std::string const & what)
instrument_already_in_use_exceptConstructor1Definition = addConstructor(instrument_already_in_use_exceptionDefinition, ...
    "instrument_already_in_use_exception::instrument_already_in_use_exception(std::string const & what)", ...
    "Description", "clib.libm2k.instrument_already_in_use_exception    Constructor of C++ class instrument_already_in_use_exception"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(instrument_already_in_use_exceptConstructor1Definition, "what", "string", "input");
validate(instrument_already_in_use_exceptConstructor1Definition);

%% C++ class constructor for C++ class |instrument_already_in_use_exception| 
% C++ Signature: instrument_already_in_use_exception::instrument_already_in_use_exception(char const * what)
%instrument_already_in_use_exceptConstructor2Definition = addConstructor(instrument_already_in_use_exceptionDefinition, ...
%    "instrument_already_in_use_exception::instrument_already_in_use_exception(char const * what)", ...
%    "Description", "clib.libm2k.instrument_already_in_use_exception    Constructor of C++ class instrument_already_in_use_exception"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(instrument_already_in_use_exceptConstructor2Definition, "what", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(instrument_already_in_use_exceptConstructor2Definition);

%% C++ class constructor for C++ class |instrument_already_in_use_exception| 
% C++ Signature: instrument_already_in_use_exception::instrument_already_in_use_exception(instrument_already_in_use_exception const & input1)
instrument_already_in_use_exceptConstructor3Definition = addConstructor(instrument_already_in_use_exceptionDefinition, ...
    "instrument_already_in_use_exception::instrument_already_in_use_exception(instrument_already_in_use_exception const & input1)", ...
    "Description", "clib.libm2k.instrument_already_in_use_exception    Constructor of C++ class instrument_already_in_use_exception"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(instrument_already_in_use_exceptConstructor3Definition, "input1", "clib.libm2k.instrument_already_in_use_exception", "input");
validate(instrument_already_in_use_exceptConstructor3Definition);

%% C++ class |invalid_parameter_exception| with MATLAB name |clib.libm2k.invalid_parameter_exception| 
invalid_parameter_exceptionDefinition = addClass(libDef, "invalid_parameter_exception", "MATLABName", "clib.libm2k.invalid_parameter_exception", ...
    "Description", "clib.libm2k.invalid_parameter_exception    Representation of C++ class invalid_parameter_exception"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |invalid_parameter_exception| 
% C++ Signature: invalid_parameter_exception::invalid_parameter_exception(std::string const & what)
invalid_parameter_exceptionConstructor1Definition = addConstructor(invalid_parameter_exceptionDefinition, ...
    "invalid_parameter_exception::invalid_parameter_exception(std::string const & what)", ...
    "Description", "clib.libm2k.invalid_parameter_exception    Constructor of C++ class invalid_parameter_exception"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(invalid_parameter_exceptionConstructor1Definition, "what", "string", "input");
validate(invalid_parameter_exceptionConstructor1Definition);

%% C++ class constructor for C++ class |invalid_parameter_exception| 
% C++ Signature: invalid_parameter_exception::invalid_parameter_exception(char const * what)
%invalid_parameter_exceptionConstructor2Definition = addConstructor(invalid_parameter_exceptionDefinition, ...
%    "invalid_parameter_exception::invalid_parameter_exception(char const * what)", ...
%    "Description", "clib.libm2k.invalid_parameter_exception    Constructor of C++ class invalid_parameter_exception"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(invalid_parameter_exceptionConstructor2Definition, "what", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(invalid_parameter_exceptionConstructor2Definition);

%% C++ class constructor for C++ class |invalid_parameter_exception| 
% C++ Signature: invalid_parameter_exception::invalid_parameter_exception(invalid_parameter_exception const & input1)
invalid_parameter_exceptionConstructor3Definition = addConstructor(invalid_parameter_exceptionDefinition, ...
    "invalid_parameter_exception::invalid_parameter_exception(invalid_parameter_exception const & input1)", ...
    "Description", "clib.libm2k.invalid_parameter_exception    Constructor of C++ class invalid_parameter_exception"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(invalid_parameter_exceptionConstructor3Definition, "input1", "clib.libm2k.invalid_parameter_exception", "input");
validate(invalid_parameter_exceptionConstructor3Definition);

%% C++ class |timeout_exception| with MATLAB name |clib.libm2k.timeout_exception| 
timeout_exceptionDefinition = addClass(libDef, "timeout_exception", "MATLABName", "clib.libm2k.timeout_exception", ...
    "Description", "clib.libm2k.timeout_exception    Representation of C++ class timeout_exception"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |timeout_exception| 
% C++ Signature: timeout_exception::timeout_exception(std::string const & what)
timeout_exceptionConstructor1Definition = addConstructor(timeout_exceptionDefinition, ...
    "timeout_exception::timeout_exception(std::string const & what)", ...
    "Description", "clib.libm2k.timeout_exception    Constructor of C++ class timeout_exception"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(timeout_exceptionConstructor1Definition, "what", "string", "input");
validate(timeout_exceptionConstructor1Definition);

%% C++ class constructor for C++ class |timeout_exception| 
% C++ Signature: timeout_exception::timeout_exception(char const * what)
%timeout_exceptionConstructor2Definition = addConstructor(timeout_exceptionDefinition, ...
%    "timeout_exception::timeout_exception(char const * what)", ...
%    "Description", "clib.libm2k.timeout_exception    Constructor of C++ class timeout_exception"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(timeout_exceptionConstructor2Definition, "what", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(timeout_exceptionConstructor2Definition);

%% C++ class constructor for C++ class |timeout_exception| 
% C++ Signature: timeout_exception::timeout_exception(timeout_exception const & input1)
timeout_exceptionConstructor3Definition = addConstructor(timeout_exceptionDefinition, ...
    "timeout_exception::timeout_exception(timeout_exception const & input1)", ...
    "Description", "clib.libm2k.timeout_exception    Constructor of C++ class timeout_exception"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(timeout_exceptionConstructor3Definition, "input1", "clib.libm2k.timeout_exception", "input");
validate(timeout_exceptionConstructor3Definition);

%% C++ function |iio_create_scan_context| with MATLAB name |clib.libm2k.iio_create_scan_context|
% C++ Signature: iio_scan_context * iio_create_scan_context(char const * backend,unsigned int flags)
%iio_create_scan_contextDefinition = addFunction(libDef, ...
%    "iio_scan_context * iio_create_scan_context(char const * backend,unsigned int flags)", ...
%    "MATLABName", "clib.libm2k.iio_create_scan_context", ...
%    "Description", "clib.libm2k.iio_create_scan_context    Representation of C++ function iio_create_scan_context"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_create_scan_contextDefinition, "backend", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_create_scan_contextDefinition, "flags", "uint32");
%defineOutput(iio_create_scan_contextDefinition, "RetVal", "clib.libm2k.iio_scan_context", <SHAPE>);
%validate(iio_create_scan_contextDefinition);

%% C++ function |iio_scan_context_destroy| with MATLAB name |clib.libm2k.iio_scan_context_destroy|
% C++ Signature: void iio_scan_context_destroy(iio_scan_context * ctx)
%iio_scan_context_destroyDefinition = addFunction(libDef, ...
%    "void iio_scan_context_destroy(iio_scan_context * ctx)", ...
%    "MATLABName", "clib.libm2k.iio_scan_context_destroy", ...
%    "Description", "clib.libm2k.iio_scan_context_destroy    Representation of C++ function iio_scan_context_destroy"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_scan_context_destroyDefinition, "ctx", "clib.libm2k.iio_scan_context", "input", <SHAPE>);
%validate(iio_scan_context_destroyDefinition);

%% C++ function |iio_context_info_get_description| with MATLAB name |clib.libm2k.iio_context_info_get_description|
% C++ Signature: char const * iio_context_info_get_description(iio_context_info const * info)
%iio_context_info_get_descriptionDefinition = addFunction(libDef, ...
%    "char const * iio_context_info_get_description(iio_context_info const * info)", ...
%    "MATLABName", "clib.libm2k.iio_context_info_get_description", ...
%    "Description", "clib.libm2k.iio_context_info_get_description    Representation of C++ function iio_context_info_get_description"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_info_get_descriptionDefinition, "info", "clib.libm2k.iio_context_info", "input", <SHAPE>);
%defineOutput(iio_context_info_get_descriptionDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_context_info_get_descriptionDefinition);

%% C++ function |iio_context_info_get_uri| with MATLAB name |clib.libm2k.iio_context_info_get_uri|
% C++ Signature: char const * iio_context_info_get_uri(iio_context_info const * info)
%iio_context_info_get_uriDefinition = addFunction(libDef, ...
%    "char const * iio_context_info_get_uri(iio_context_info const * info)", ...
%    "MATLABName", "clib.libm2k.iio_context_info_get_uri", ...
%    "Description", "clib.libm2k.iio_context_info_get_uri    Representation of C++ function iio_context_info_get_uri"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_info_get_uriDefinition, "info", "clib.libm2k.iio_context_info", "input", <SHAPE>);
%defineOutput(iio_context_info_get_uriDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_context_info_get_uriDefinition);

%% C++ function |iio_library_get_version| with MATLAB name |clib.libm2k.iio_library_get_version|
% C++ Signature: void iio_library_get_version(unsigned int * major,unsigned int * minor,char [8] git_tag)
%iio_library_get_versionDefinition = addFunction(libDef, ...
%    "void iio_library_get_version(unsigned int * major,unsigned int * minor,char [8] git_tag)", ...
%    "MATLABName", "clib.libm2k.iio_library_get_version", ...
%    "Description", "clib.libm2k.iio_library_get_version    Representation of C++ function iio_library_get_version"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_library_get_versionDefinition, "major", "uint32", <DIRECTION>, <SHAPE>);
%defineArgument(iio_library_get_versionDefinition, "minor", "uint32", <DIRECTION>, <SHAPE>);
%defineArgument(iio_library_get_versionDefinition, "git_tag", "int8", <DIRECTION>, [8]);
%validate(iio_library_get_versionDefinition);

%% C++ function |iio_strerror| with MATLAB name |clib.libm2k.iio_strerror|
% C++ Signature: void iio_strerror(int err,char * dst,size_t len)
%iio_strerrorDefinition = addFunction(libDef, ...
%    "void iio_strerror(int err,char * dst,size_t len)", ...
%    "MATLABName", "clib.libm2k.iio_strerror", ...
%    "Description", "clib.libm2k.iio_strerror    Representation of C++ function iio_strerror"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_strerrorDefinition, "err", "int32");
%defineArgument(iio_strerrorDefinition, "dst", "int8", <DIRECTION>, <SHAPE>);
%defineArgument(iio_strerrorDefinition, "len", "uint64");
%validate(iio_strerrorDefinition);

%% C++ function |iio_has_backend| with MATLAB name |clib.libm2k.iio_has_backend|
% C++ Signature: bool iio_has_backend(char const * backend)
%iio_has_backendDefinition = addFunction(libDef, ...
%    "bool iio_has_backend(char const * backend)", ...
%    "MATLABName", "clib.libm2k.iio_has_backend", ...
%    "Description", "clib.libm2k.iio_has_backend    Representation of C++ function iio_has_backend"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_has_backendDefinition, "backend", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_has_backendDefinition, "RetVal", "logical");
%validate(iio_has_backendDefinition);

%% C++ function |iio_get_backends_count| with MATLAB name |clib.libm2k.iio_get_backends_count|
% C++ Signature: unsigned int iio_get_backends_count()
% iio_get_backends_countDefinition = addFunction(libDef, ...
%     "unsigned int iio_get_backends_count()", ...
%     "MATLABName", "clib.libm2k.iio_get_backends_count", ...
%     "Description", "clib.libm2k.iio_get_backends_count    Representation of C++ function iio_get_backends_count"); % This description is shown as help to user. Modify it to appropriate description.
% defineOutput(iio_get_backends_countDefinition, "RetVal", "uint32");
% validate(iio_get_backends_countDefinition);

%% C++ function |iio_get_backend| with MATLAB name |clib.libm2k.iio_get_backend|
% C++ Signature: char const * iio_get_backend(unsigned int index)
%iio_get_backendDefinition = addFunction(libDef, ...
%    "char const * iio_get_backend(unsigned int index)", ...
%    "MATLABName", "clib.libm2k.iio_get_backend", ...
%    "Description", "clib.libm2k.iio_get_backend    Representation of C++ function iio_get_backend"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_get_backendDefinition, "index", "uint32");
%defineOutput(iio_get_backendDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_get_backendDefinition);

%% C++ function |iio_create_default_context| with MATLAB name |clib.libm2k.iio_create_default_context|
% C++ Signature: iio_context * iio_create_default_context()
%iio_create_default_contextDefinition = addFunction(libDef, ...
%    "iio_context * iio_create_default_context()", ...
%    "MATLABName", "clib.libm2k.iio_create_default_context", ...
%    "Description", "clib.libm2k.iio_create_default_context    Representation of C++ function iio_create_default_context"); % This description is shown as help to user. Modify it to appropriate description.
%defineOutput(iio_create_default_contextDefinition, "RetVal", "clib.libm2k.iio_context", <SHAPE>);
%validate(iio_create_default_contextDefinition);

%% C++ function |iio_create_local_context| with MATLAB name |clib.libm2k.iio_create_local_context|
% C++ Signature: iio_context * iio_create_local_context()
%iio_create_local_contextDefinition = addFunction(libDef, ...
%    "iio_context * iio_create_local_context()", ...
%    "MATLABName", "clib.libm2k.iio_create_local_context", ...
%    "Description", "clib.libm2k.iio_create_local_context    Representation of C++ function iio_create_local_context"); % This description is shown as help to user. Modify it to appropriate description.
%defineOutput(iio_create_local_contextDefinition, "RetVal", "clib.libm2k.iio_context", <SHAPE>);
%validate(iio_create_local_contextDefinition);

%% C++ function |iio_create_xml_context| with MATLAB name |clib.libm2k.iio_create_xml_context|
% C++ Signature: iio_context * iio_create_xml_context(char const * xml_file)
%iio_create_xml_contextDefinition = addFunction(libDef, ...
%    "iio_context * iio_create_xml_context(char const * xml_file)", ...
%    "MATLABName", "clib.libm2k.iio_create_xml_context", ...
%    "Description", "clib.libm2k.iio_create_xml_context    Representation of C++ function iio_create_xml_context"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_create_xml_contextDefinition, "xml_file", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_create_xml_contextDefinition, "RetVal", "clib.libm2k.iio_context", <SHAPE>);
%validate(iio_create_xml_contextDefinition);

%% C++ function |iio_create_xml_context_mem| with MATLAB name |clib.libm2k.iio_create_xml_context_mem|
% C++ Signature: iio_context * iio_create_xml_context_mem(char const * xml,size_t len)
%iio_create_xml_context_memDefinition = addFunction(libDef, ...
%    "iio_context * iio_create_xml_context_mem(char const * xml,size_t len)", ...
%    "MATLABName", "clib.libm2k.iio_create_xml_context_mem", ...
%    "Description", "clib.libm2k.iio_create_xml_context_mem    Representation of C++ function iio_create_xml_context_mem"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_create_xml_context_memDefinition, "xml", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_create_xml_context_memDefinition, "len", "uint64");
%defineOutput(iio_create_xml_context_memDefinition, "RetVal", "clib.libm2k.iio_context", <SHAPE>);
%validate(iio_create_xml_context_memDefinition);

%% C++ function |iio_create_network_context| with MATLAB name |clib.libm2k.iio_create_network_context|
% C++ Signature: iio_context * iio_create_network_context(char const * host)
%iio_create_network_contextDefinition = addFunction(libDef, ...
%    "iio_context * iio_create_network_context(char const * host)", ...
%    "MATLABName", "clib.libm2k.iio_create_network_context", ...
%    "Description", "clib.libm2k.iio_create_network_context    Representation of C++ function iio_create_network_context"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_create_network_contextDefinition, "host", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_create_network_contextDefinition, "RetVal", "clib.libm2k.iio_context", <SHAPE>);
%validate(iio_create_network_contextDefinition);

%% C++ function |iio_create_context_from_uri| with MATLAB name |clib.libm2k.iio_create_context_from_uri|
% C++ Signature: iio_context * iio_create_context_from_uri(char const * uri)
%iio_create_context_from_uriDefinition = addFunction(libDef, ...
%    "iio_context * iio_create_context_from_uri(char const * uri)", ...
%    "MATLABName", "clib.libm2k.iio_create_context_from_uri", ...
%    "Description", "clib.libm2k.iio_create_context_from_uri    Representation of C++ function iio_create_context_from_uri"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_create_context_from_uriDefinition, "uri", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_create_context_from_uriDefinition, "RetVal", "clib.libm2k.iio_context", <SHAPE>);
%validate(iio_create_context_from_uriDefinition);

%% C++ function |iio_context_clone| with MATLAB name |clib.libm2k.iio_context_clone|
% C++ Signature: iio_context * iio_context_clone(iio_context const * ctx)
%iio_context_cloneDefinition = addFunction(libDef, ...
%    "iio_context * iio_context_clone(iio_context const * ctx)", ...
%    "MATLABName", "clib.libm2k.iio_context_clone", ...
%    "Description", "clib.libm2k.iio_context_clone    Representation of C++ function iio_context_clone"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_cloneDefinition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineOutput(iio_context_cloneDefinition, "RetVal", "clib.libm2k.iio_context", <SHAPE>);
%validate(iio_context_cloneDefinition);

%% C++ function |iio_context_destroy| with MATLAB name |clib.libm2k.iio_context_destroy|
% C++ Signature: void iio_context_destroy(iio_context * ctx)
%iio_context_destroyDefinition = addFunction(libDef, ...
%    "void iio_context_destroy(iio_context * ctx)", ...
%    "MATLABName", "clib.libm2k.iio_context_destroy", ...
%    "Description", "clib.libm2k.iio_context_destroy    Representation of C++ function iio_context_destroy"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_destroyDefinition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%validate(iio_context_destroyDefinition);

%% C++ function |iio_context_get_version| with MATLAB name |clib.libm2k.iio_context_get_version|
% C++ Signature: int iio_context_get_version(iio_context const * ctx,unsigned int * major,unsigned int * minor,char [8] git_tag)
%iio_context_get_versionDefinition = addFunction(libDef, ...
%    "int iio_context_get_version(iio_context const * ctx,unsigned int * major,unsigned int * minor,char [8] git_tag)", ...
%    "MATLABName", "clib.libm2k.iio_context_get_version", ...
%    "Description", "clib.libm2k.iio_context_get_version    Representation of C++ function iio_context_get_version"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_get_versionDefinition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineArgument(iio_context_get_versionDefinition, "major", "uint32", <DIRECTION>, <SHAPE>);
%defineArgument(iio_context_get_versionDefinition, "minor", "uint32", <DIRECTION>, <SHAPE>);
%defineArgument(iio_context_get_versionDefinition, "git_tag", "int8", <DIRECTION>, [8]);
%defineOutput(iio_context_get_versionDefinition, "RetVal", "int32");
%validate(iio_context_get_versionDefinition);

%% C++ function |iio_context_get_xml| with MATLAB name |clib.libm2k.iio_context_get_xml|
% C++ Signature: char const * iio_context_get_xml(iio_context const * ctx)
%iio_context_get_xmlDefinition = addFunction(libDef, ...
%    "char const * iio_context_get_xml(iio_context const * ctx)", ...
%    "MATLABName", "clib.libm2k.iio_context_get_xml", ...
%    "Description", "clib.libm2k.iio_context_get_xml    Representation of C++ function iio_context_get_xml"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_get_xmlDefinition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineOutput(iio_context_get_xmlDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_context_get_xmlDefinition);

%% C++ function |iio_context_get_name| with MATLAB name |clib.libm2k.iio_context_get_name|
% C++ Signature: char const * iio_context_get_name(iio_context const * ctx)
%iio_context_get_nameDefinition = addFunction(libDef, ...
%    "char const * iio_context_get_name(iio_context const * ctx)", ...
%    "MATLABName", "clib.libm2k.iio_context_get_name", ...
%    "Description", "clib.libm2k.iio_context_get_name    Representation of C++ function iio_context_get_name"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_get_nameDefinition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineOutput(iio_context_get_nameDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_context_get_nameDefinition);

%% C++ function |iio_context_get_description| with MATLAB name |clib.libm2k.iio_context_get_description|
% C++ Signature: char const * iio_context_get_description(iio_context const * ctx)
%iio_context_get_descriptionDefinition = addFunction(libDef, ...
%    "char const * iio_context_get_description(iio_context const * ctx)", ...
%    "MATLABName", "clib.libm2k.iio_context_get_description", ...
%    "Description", "clib.libm2k.iio_context_get_description    Representation of C++ function iio_context_get_description"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_get_descriptionDefinition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineOutput(iio_context_get_descriptionDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_context_get_descriptionDefinition);

%% C++ function |iio_context_get_attrs_count| with MATLAB name |clib.libm2k.iio_context_get_attrs_count|
% C++ Signature: unsigned int iio_context_get_attrs_count(iio_context const * ctx)
%iio_context_get_attrs_countDefinition = addFunction(libDef, ...
%    "unsigned int iio_context_get_attrs_count(iio_context const * ctx)", ...
%    "MATLABName", "clib.libm2k.iio_context_get_attrs_count", ...
%    "Description", "clib.libm2k.iio_context_get_attrs_count    Representation of C++ function iio_context_get_attrs_count"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_get_attrs_countDefinition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineOutput(iio_context_get_attrs_countDefinition, "RetVal", "uint32");
%validate(iio_context_get_attrs_countDefinition);

%% C++ function |iio_context_get_attr_value| with MATLAB name |clib.libm2k.iio_context_get_attr_value|
% C++ Signature: char const * iio_context_get_attr_value(iio_context const * ctx,char const * name)
%iio_context_get_attr_valueDefinition = addFunction(libDef, ...
%    "char const * iio_context_get_attr_value(iio_context const * ctx,char const * name)", ...
%    "MATLABName", "clib.libm2k.iio_context_get_attr_value", ...
%    "Description", "clib.libm2k.iio_context_get_attr_value    Representation of C++ function iio_context_get_attr_value"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_get_attr_valueDefinition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineArgument(iio_context_get_attr_valueDefinition, "name", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_context_get_attr_valueDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_context_get_attr_valueDefinition);

%% C++ function |iio_context_get_devices_count| with MATLAB name |clib.libm2k.iio_context_get_devices_count|
% C++ Signature: unsigned int iio_context_get_devices_count(iio_context const * ctx)
%iio_context_get_devices_countDefinition = addFunction(libDef, ...
%    "unsigned int iio_context_get_devices_count(iio_context const * ctx)", ...
%    "MATLABName", "clib.libm2k.iio_context_get_devices_count", ...
%    "Description", "clib.libm2k.iio_context_get_devices_count    Representation of C++ function iio_context_get_devices_count"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_get_devices_countDefinition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineOutput(iio_context_get_devices_countDefinition, "RetVal", "uint32");
%validate(iio_context_get_devices_countDefinition);

%% C++ function |iio_context_get_device| with MATLAB name |clib.libm2k.iio_context_get_device|
% C++ Signature: iio_device * iio_context_get_device(iio_context const * ctx,unsigned int index)
%iio_context_get_deviceDefinition = addFunction(libDef, ...
%    "iio_device * iio_context_get_device(iio_context const * ctx,unsigned int index)", ...
%    "MATLABName", "clib.libm2k.iio_context_get_device", ...
%    "Description", "clib.libm2k.iio_context_get_device    Representation of C++ function iio_context_get_device"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_get_deviceDefinition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineArgument(iio_context_get_deviceDefinition, "index", "uint32");
%defineOutput(iio_context_get_deviceDefinition, "RetVal", "clib.libm2k.iio_device", <SHAPE>);
%validate(iio_context_get_deviceDefinition);

%% C++ function |iio_context_find_device| with MATLAB name |clib.libm2k.iio_context_find_device|
% C++ Signature: iio_device * iio_context_find_device(iio_context const * ctx,char const * name)
%iio_context_find_deviceDefinition = addFunction(libDef, ...
%    "iio_device * iio_context_find_device(iio_context const * ctx,char const * name)", ...
%    "MATLABName", "clib.libm2k.iio_context_find_device", ...
%    "Description", "clib.libm2k.iio_context_find_device    Representation of C++ function iio_context_find_device"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_find_deviceDefinition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineArgument(iio_context_find_deviceDefinition, "name", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_context_find_deviceDefinition, "RetVal", "clib.libm2k.iio_device", <SHAPE>);
%validate(iio_context_find_deviceDefinition);

%% C++ function |iio_context_set_timeout| with MATLAB name |clib.libm2k.iio_context_set_timeout|
% C++ Signature: int iio_context_set_timeout(iio_context * ctx,unsigned int timeout_ms)
%iio_context_set_timeoutDefinition = addFunction(libDef, ...
%    "int iio_context_set_timeout(iio_context * ctx,unsigned int timeout_ms)", ...
%    "MATLABName", "clib.libm2k.iio_context_set_timeout", ...
%    "Description", "clib.libm2k.iio_context_set_timeout    Representation of C++ function iio_context_set_timeout"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_context_set_timeoutDefinition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineArgument(iio_context_set_timeoutDefinition, "timeout_ms", "uint32");
%defineOutput(iio_context_set_timeoutDefinition, "RetVal", "int32");
%validate(iio_context_set_timeoutDefinition);

%% C++ function |iio_device_get_context| with MATLAB name |clib.libm2k.iio_device_get_context|
% C++ Signature: iio_context const * iio_device_get_context(iio_device const * dev)
%iio_device_get_contextDefinition = addFunction(libDef, ...
%    "iio_context const * iio_device_get_context(iio_device const * dev)", ...
%    "MATLABName", "clib.libm2k.iio_device_get_context", ...
%    "Description", "clib.libm2k.iio_device_get_context    Representation of C++ function iio_device_get_context"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_get_contextDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineOutput(iio_device_get_contextDefinition, "RetVal", "clib.libm2k.iio_context", <SHAPE>);
%validate(iio_device_get_contextDefinition);

%% C++ function |iio_device_get_id| with MATLAB name |clib.libm2k.iio_device_get_id|
% C++ Signature: char const * iio_device_get_id(iio_device const * dev)
%iio_device_get_idDefinition = addFunction(libDef, ...
%    "char const * iio_device_get_id(iio_device const * dev)", ...
%    "MATLABName", "clib.libm2k.iio_device_get_id", ...
%    "Description", "clib.libm2k.iio_device_get_id    Representation of C++ function iio_device_get_id"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_get_idDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineOutput(iio_device_get_idDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_device_get_idDefinition);

%% C++ function |iio_device_get_name| with MATLAB name |clib.libm2k.iio_device_get_name|
% C++ Signature: char const * iio_device_get_name(iio_device const * dev)
%iio_device_get_nameDefinition = addFunction(libDef, ...
%    "char const * iio_device_get_name(iio_device const * dev)", ...
%    "MATLABName", "clib.libm2k.iio_device_get_name", ...
%    "Description", "clib.libm2k.iio_device_get_name    Representation of C++ function iio_device_get_name"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_get_nameDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineOutput(iio_device_get_nameDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_device_get_nameDefinition);

%% C++ function |iio_device_get_channels_count| with MATLAB name |clib.libm2k.iio_device_get_channels_count|
% C++ Signature: unsigned int iio_device_get_channels_count(iio_device const * dev)
%iio_device_get_channels_countDefinition = addFunction(libDef, ...
%    "unsigned int iio_device_get_channels_count(iio_device const * dev)", ...
%    "MATLABName", "clib.libm2k.iio_device_get_channels_count", ...
%    "Description", "clib.libm2k.iio_device_get_channels_count    Representation of C++ function iio_device_get_channels_count"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_get_channels_countDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineOutput(iio_device_get_channels_countDefinition, "RetVal", "uint32");
%validate(iio_device_get_channels_countDefinition);

%% C++ function |iio_device_get_attrs_count| with MATLAB name |clib.libm2k.iio_device_get_attrs_count|
% C++ Signature: unsigned int iio_device_get_attrs_count(iio_device const * dev)
%iio_device_get_attrs_countDefinition = addFunction(libDef, ...
%    "unsigned int iio_device_get_attrs_count(iio_device const * dev)", ...
%    "MATLABName", "clib.libm2k.iio_device_get_attrs_count", ...
%    "Description", "clib.libm2k.iio_device_get_attrs_count    Representation of C++ function iio_device_get_attrs_count"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_get_attrs_countDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineOutput(iio_device_get_attrs_countDefinition, "RetVal", "uint32");
%validate(iio_device_get_attrs_countDefinition);

%% C++ function |iio_device_get_buffer_attrs_count| with MATLAB name |clib.libm2k.iio_device_get_buffer_attrs_count|
% C++ Signature: unsigned int iio_device_get_buffer_attrs_count(iio_device const * dev)
%iio_device_get_buffer_attrs_countDefinition = addFunction(libDef, ...
%    "unsigned int iio_device_get_buffer_attrs_count(iio_device const * dev)", ...
%    "MATLABName", "clib.libm2k.iio_device_get_buffer_attrs_count", ...
%    "Description", "clib.libm2k.iio_device_get_buffer_attrs_count    Representation of C++ function iio_device_get_buffer_attrs_count"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_get_buffer_attrs_countDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineOutput(iio_device_get_buffer_attrs_countDefinition, "RetVal", "uint32");
%validate(iio_device_get_buffer_attrs_countDefinition);

%% C++ function |iio_device_get_channel| with MATLAB name |clib.libm2k.iio_device_get_channel|
% C++ Signature: iio_channel * iio_device_get_channel(iio_device const * dev,unsigned int index)
%iio_device_get_channelDefinition = addFunction(libDef, ...
%    "iio_channel * iio_device_get_channel(iio_device const * dev,unsigned int index)", ...
%    "MATLABName", "clib.libm2k.iio_device_get_channel", ...
%    "Description", "clib.libm2k.iio_device_get_channel    Representation of C++ function iio_device_get_channel"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_get_channelDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_get_channelDefinition, "index", "uint32");
%defineOutput(iio_device_get_channelDefinition, "RetVal", "clib.libm2k.iio_channel", <SHAPE>);
%validate(iio_device_get_channelDefinition);

%% C++ function |iio_device_get_attr| with MATLAB name |clib.libm2k.iio_device_get_attr|
% C++ Signature: char const * iio_device_get_attr(iio_device const * dev,unsigned int index)
%iio_device_get_attrDefinition = addFunction(libDef, ...
%    "char const * iio_device_get_attr(iio_device const * dev,unsigned int index)", ...
%    "MATLABName", "clib.libm2k.iio_device_get_attr", ...
%    "Description", "clib.libm2k.iio_device_get_attr    Representation of C++ function iio_device_get_attr"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_get_attrDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_get_attrDefinition, "index", "uint32");
%defineOutput(iio_device_get_attrDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_device_get_attrDefinition);

%% C++ function |iio_device_get_buffer_attr| with MATLAB name |clib.libm2k.iio_device_get_buffer_attr|
% C++ Signature: char const * iio_device_get_buffer_attr(iio_device const * dev,unsigned int index)
%iio_device_get_buffer_attrDefinition = addFunction(libDef, ...
%    "char const * iio_device_get_buffer_attr(iio_device const * dev,unsigned int index)", ...
%    "MATLABName", "clib.libm2k.iio_device_get_buffer_attr", ...
%    "Description", "clib.libm2k.iio_device_get_buffer_attr    Representation of C++ function iio_device_get_buffer_attr"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_get_buffer_attrDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_get_buffer_attrDefinition, "index", "uint32");
%defineOutput(iio_device_get_buffer_attrDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_device_get_buffer_attrDefinition);

%% C++ function |iio_device_find_channel| with MATLAB name |clib.libm2k.iio_device_find_channel|
% C++ Signature: iio_channel * iio_device_find_channel(iio_device const * dev,char const * name,bool output)
%iio_device_find_channelDefinition = addFunction(libDef, ...
%    "iio_channel * iio_device_find_channel(iio_device const * dev,char const * name,bool output)", ...
%    "MATLABName", "clib.libm2k.iio_device_find_channel", ...
%    "Description", "clib.libm2k.iio_device_find_channel    Representation of C++ function iio_device_find_channel"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_find_channelDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_find_channelDefinition, "name", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_find_channelDefinition, "output", "logical");
%defineOutput(iio_device_find_channelDefinition, "RetVal", "clib.libm2k.iio_channel", <SHAPE>);
%validate(iio_device_find_channelDefinition);

%% C++ function |iio_device_find_attr| with MATLAB name |clib.libm2k.iio_device_find_attr|
% C++ Signature: char const * iio_device_find_attr(iio_device const * dev,char const * name)
%iio_device_find_attrDefinition = addFunction(libDef, ...
%    "char const * iio_device_find_attr(iio_device const * dev,char const * name)", ...
%    "MATLABName", "clib.libm2k.iio_device_find_attr", ...
%    "Description", "clib.libm2k.iio_device_find_attr    Representation of C++ function iio_device_find_attr"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_find_attrDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_find_attrDefinition, "name", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_device_find_attrDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_device_find_attrDefinition);

%% C++ function |iio_device_find_buffer_attr| with MATLAB name |clib.libm2k.iio_device_find_buffer_attr|
% C++ Signature: char const * iio_device_find_buffer_attr(iio_device const * dev,char const * name)
%iio_device_find_buffer_attrDefinition = addFunction(libDef, ...
%    "char const * iio_device_find_buffer_attr(iio_device const * dev,char const * name)", ...
%    "MATLABName", "clib.libm2k.iio_device_find_buffer_attr", ...
%    "Description", "clib.libm2k.iio_device_find_buffer_attr    Representation of C++ function iio_device_find_buffer_attr"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_find_buffer_attrDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_find_buffer_attrDefinition, "name", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_device_find_buffer_attrDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_device_find_buffer_attrDefinition);

%% C++ function |iio_device_attr_read| with MATLAB name |clib.libm2k.iio_device_attr_read|
% C++ Signature: ssize_t iio_device_attr_read(iio_device const * dev,char const * attr,char * dst,size_t len)
%iio_device_attr_readDefinition = addFunction(libDef, ...
%    "ssize_t iio_device_attr_read(iio_device const * dev,char const * attr,char * dst,size_t len)", ...
%    "MATLABName", "clib.libm2k.iio_device_attr_read", ...
%    "Description", "clib.libm2k.iio_device_attr_read    Representation of C++ function iio_device_attr_read"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_attr_readDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_attr_readDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_attr_readDefinition, "dst", "int8", <DIRECTION>, <SHAPE>);
%defineArgument(iio_device_attr_readDefinition, "len", "uint64");
%defineOutput(iio_device_attr_readDefinition, "RetVal", "int64");
%validate(iio_device_attr_readDefinition);

%% C++ function |iio_device_attr_read_bool| with MATLAB name |clib.libm2k.iio_device_attr_read_bool|
% C++ Signature: int iio_device_attr_read_bool(iio_device const * dev,char const * attr,bool * val)
%iio_device_attr_read_boolDefinition = addFunction(libDef, ...
%    "int iio_device_attr_read_bool(iio_device const * dev,char const * attr,bool * val)", ...
%    "MATLABName", "clib.libm2k.iio_device_attr_read_bool", ...
%    "Description", "clib.libm2k.iio_device_attr_read_bool    Representation of C++ function iio_device_attr_read_bool"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_attr_read_boolDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_attr_read_boolDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_attr_read_boolDefinition, "val", "logical", <DIRECTION>, <SHAPE>);
%defineOutput(iio_device_attr_read_boolDefinition, "RetVal", "int32");
%validate(iio_device_attr_read_boolDefinition);

%% C++ function |iio_device_attr_read_longlong| with MATLAB name |clib.libm2k.iio_device_attr_read_longlong|
% C++ Signature: int iio_device_attr_read_longlong(iio_device const * dev,char const * attr,long long * val)
%iio_device_attr_read_longlongDefinition = addFunction(libDef, ...
%    "int iio_device_attr_read_longlong(iio_device const * dev,char const * attr,long long * val)", ...
%    "MATLABName", "clib.libm2k.iio_device_attr_read_longlong", ...
%    "Description", "clib.libm2k.iio_device_attr_read_longlong    Representation of C++ function iio_device_attr_read_longlong"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_attr_read_longlongDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_attr_read_longlongDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_attr_read_longlongDefinition, "val", "int64", <DIRECTION>, <SHAPE>);
%defineOutput(iio_device_attr_read_longlongDefinition, "RetVal", "int32");
%validate(iio_device_attr_read_longlongDefinition);

%% C++ function |iio_device_attr_read_double| with MATLAB name |clib.libm2k.iio_device_attr_read_double|
% C++ Signature: int iio_device_attr_read_double(iio_device const * dev,char const * attr,double * val)
%iio_device_attr_read_doubleDefinition = addFunction(libDef, ...
%    "int iio_device_attr_read_double(iio_device const * dev,char const * attr,double * val)", ...
%    "MATLABName", "clib.libm2k.iio_device_attr_read_double", ...
%    "Description", "clib.libm2k.iio_device_attr_read_double    Representation of C++ function iio_device_attr_read_double"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_attr_read_doubleDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_attr_read_doubleDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_attr_read_doubleDefinition, "val", "double", <DIRECTION>, <SHAPE>);
%defineOutput(iio_device_attr_read_doubleDefinition, "RetVal", "int32");
%validate(iio_device_attr_read_doubleDefinition);

%% C++ function |iio_device_attr_write| with MATLAB name |clib.libm2k.iio_device_attr_write|
% C++ Signature: ssize_t iio_device_attr_write(iio_device const * dev,char const * attr,char const * src)
%iio_device_attr_writeDefinition = addFunction(libDef, ...
%    "ssize_t iio_device_attr_write(iio_device const * dev,char const * attr,char const * src)", ...
%    "MATLABName", "clib.libm2k.iio_device_attr_write", ...
%    "Description", "clib.libm2k.iio_device_attr_write    Representation of C++ function iio_device_attr_write"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_attr_writeDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_attr_writeDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_attr_writeDefinition, "src", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_device_attr_writeDefinition, "RetVal", "int64");
%validate(iio_device_attr_writeDefinition);

%% C++ function |iio_device_attr_write_bool| with MATLAB name |clib.libm2k.iio_device_attr_write_bool|
% C++ Signature: int iio_device_attr_write_bool(iio_device const * dev,char const * attr,bool val)
%iio_device_attr_write_boolDefinition = addFunction(libDef, ...
%    "int iio_device_attr_write_bool(iio_device const * dev,char const * attr,bool val)", ...
%    "MATLABName", "clib.libm2k.iio_device_attr_write_bool", ...
%    "Description", "clib.libm2k.iio_device_attr_write_bool    Representation of C++ function iio_device_attr_write_bool"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_attr_write_boolDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_attr_write_boolDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_attr_write_boolDefinition, "val", "logical");
%defineOutput(iio_device_attr_write_boolDefinition, "RetVal", "int32");
%validate(iio_device_attr_write_boolDefinition);

%% C++ function |iio_device_attr_write_longlong| with MATLAB name |clib.libm2k.iio_device_attr_write_longlong|
% C++ Signature: int iio_device_attr_write_longlong(iio_device const * dev,char const * attr,long long val)
%iio_device_attr_write_longlongDefinition = addFunction(libDef, ...
%    "int iio_device_attr_write_longlong(iio_device const * dev,char const * attr,long long val)", ...
%    "MATLABName", "clib.libm2k.iio_device_attr_write_longlong", ...
%    "Description", "clib.libm2k.iio_device_attr_write_longlong    Representation of C++ function iio_device_attr_write_longlong"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_attr_write_longlongDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_attr_write_longlongDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_attr_write_longlongDefinition, "val", "int64");
%defineOutput(iio_device_attr_write_longlongDefinition, "RetVal", "int32");
%validate(iio_device_attr_write_longlongDefinition);

%% C++ function |iio_device_attr_write_double| with MATLAB name |clib.libm2k.iio_device_attr_write_double|
% C++ Signature: int iio_device_attr_write_double(iio_device const * dev,char const * attr,double val)
%iio_device_attr_write_doubleDefinition = addFunction(libDef, ...
%    "int iio_device_attr_write_double(iio_device const * dev,char const * attr,double val)", ...
%    "MATLABName", "clib.libm2k.iio_device_attr_write_double", ...
%    "Description", "clib.libm2k.iio_device_attr_write_double    Representation of C++ function iio_device_attr_write_double"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_attr_write_doubleDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_attr_write_doubleDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_attr_write_doubleDefinition, "val", "double");
%defineOutput(iio_device_attr_write_doubleDefinition, "RetVal", "int32");
%validate(iio_device_attr_write_doubleDefinition);

%% C++ function |iio_device_buffer_attr_read| with MATLAB name |clib.libm2k.iio_device_buffer_attr_read|
% C++ Signature: ssize_t iio_device_buffer_attr_read(iio_device const * dev,char const * attr,char * dst,size_t len)
%iio_device_buffer_attr_readDefinition = addFunction(libDef, ...
%    "ssize_t iio_device_buffer_attr_read(iio_device const * dev,char const * attr,char * dst,size_t len)", ...
%    "MATLABName", "clib.libm2k.iio_device_buffer_attr_read", ...
%    "Description", "clib.libm2k.iio_device_buffer_attr_read    Representation of C++ function iio_device_buffer_attr_read"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_buffer_attr_readDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_buffer_attr_readDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_buffer_attr_readDefinition, "dst", "int8", <DIRECTION>, <SHAPE>);
%defineArgument(iio_device_buffer_attr_readDefinition, "len", "uint64");
%defineOutput(iio_device_buffer_attr_readDefinition, "RetVal", "int64");
%validate(iio_device_buffer_attr_readDefinition);

%% C++ function |iio_device_buffer_attr_read_bool| with MATLAB name |clib.libm2k.iio_device_buffer_attr_read_bool|
% C++ Signature: int iio_device_buffer_attr_read_bool(iio_device const * dev,char const * attr,bool * val)
%iio_device_buffer_attr_read_boolDefinition = addFunction(libDef, ...
%    "int iio_device_buffer_attr_read_bool(iio_device const * dev,char const * attr,bool * val)", ...
%    "MATLABName", "clib.libm2k.iio_device_buffer_attr_read_bool", ...
%    "Description", "clib.libm2k.iio_device_buffer_attr_read_bool    Representation of C++ function iio_device_buffer_attr_read_bool"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_buffer_attr_read_boolDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_buffer_attr_read_boolDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_buffer_attr_read_boolDefinition, "val", "logical", <DIRECTION>, <SHAPE>);
%defineOutput(iio_device_buffer_attr_read_boolDefinition, "RetVal", "int32");
%validate(iio_device_buffer_attr_read_boolDefinition);

%% C++ function |iio_device_buffer_attr_read_longlong| with MATLAB name |clib.libm2k.iio_device_buffer_attr_read_longlong|
% C++ Signature: int iio_device_buffer_attr_read_longlong(iio_device const * dev,char const * attr,long long * val)
%iio_device_buffer_attr_read_longlongDefinition = addFunction(libDef, ...
%    "int iio_device_buffer_attr_read_longlong(iio_device const * dev,char const * attr,long long * val)", ...
%    "MATLABName", "clib.libm2k.iio_device_buffer_attr_read_longlong", ...
%    "Description", "clib.libm2k.iio_device_buffer_attr_read_longlong    Representation of C++ function iio_device_buffer_attr_read_longlong"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_buffer_attr_read_longlongDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_buffer_attr_read_longlongDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_buffer_attr_read_longlongDefinition, "val", "int64", <DIRECTION>, <SHAPE>);
%defineOutput(iio_device_buffer_attr_read_longlongDefinition, "RetVal", "int32");
%validate(iio_device_buffer_attr_read_longlongDefinition);

%% C++ function |iio_device_buffer_attr_read_double| with MATLAB name |clib.libm2k.iio_device_buffer_attr_read_double|
% C++ Signature: int iio_device_buffer_attr_read_double(iio_device const * dev,char const * attr,double * val)
%iio_device_buffer_attr_read_doubleDefinition = addFunction(libDef, ...
%    "int iio_device_buffer_attr_read_double(iio_device const * dev,char const * attr,double * val)", ...
%    "MATLABName", "clib.libm2k.iio_device_buffer_attr_read_double", ...
%    "Description", "clib.libm2k.iio_device_buffer_attr_read_double    Representation of C++ function iio_device_buffer_attr_read_double"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_buffer_attr_read_doubleDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_buffer_attr_read_doubleDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_buffer_attr_read_doubleDefinition, "val", "double", <DIRECTION>, <SHAPE>);
%defineOutput(iio_device_buffer_attr_read_doubleDefinition, "RetVal", "int32");
%validate(iio_device_buffer_attr_read_doubleDefinition);

%% C++ function |iio_device_buffer_attr_write| with MATLAB name |clib.libm2k.iio_device_buffer_attr_write|
% C++ Signature: ssize_t iio_device_buffer_attr_write(iio_device const * dev,char const * attr,char const * src)
%iio_device_buffer_attr_writeDefinition = addFunction(libDef, ...
%    "ssize_t iio_device_buffer_attr_write(iio_device const * dev,char const * attr,char const * src)", ...
%    "MATLABName", "clib.libm2k.iio_device_buffer_attr_write", ...
%    "Description", "clib.libm2k.iio_device_buffer_attr_write    Representation of C++ function iio_device_buffer_attr_write"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_buffer_attr_writeDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_buffer_attr_writeDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_buffer_attr_writeDefinition, "src", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_device_buffer_attr_writeDefinition, "RetVal", "int64");
%validate(iio_device_buffer_attr_writeDefinition);

%% C++ function |iio_device_buffer_attr_write_bool| with MATLAB name |clib.libm2k.iio_device_buffer_attr_write_bool|
% C++ Signature: int iio_device_buffer_attr_write_bool(iio_device const * dev,char const * attr,bool val)
%iio_device_buffer_attr_write_boolDefinition = addFunction(libDef, ...
%    "int iio_device_buffer_attr_write_bool(iio_device const * dev,char const * attr,bool val)", ...
%    "MATLABName", "clib.libm2k.iio_device_buffer_attr_write_bool", ...
%    "Description", "clib.libm2k.iio_device_buffer_attr_write_bool    Representation of C++ function iio_device_buffer_attr_write_bool"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_buffer_attr_write_boolDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_buffer_attr_write_boolDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_buffer_attr_write_boolDefinition, "val", "logical");
%defineOutput(iio_device_buffer_attr_write_boolDefinition, "RetVal", "int32");
%validate(iio_device_buffer_attr_write_boolDefinition);

%% C++ function |iio_device_buffer_attr_write_longlong| with MATLAB name |clib.libm2k.iio_device_buffer_attr_write_longlong|
% C++ Signature: int iio_device_buffer_attr_write_longlong(iio_device const * dev,char const * attr,long long val)
%iio_device_buffer_attr_write_longlongDefinition = addFunction(libDef, ...
%    "int iio_device_buffer_attr_write_longlong(iio_device const * dev,char const * attr,long long val)", ...
%    "MATLABName", "clib.libm2k.iio_device_buffer_attr_write_longlong", ...
%    "Description", "clib.libm2k.iio_device_buffer_attr_write_longlong    Representation of C++ function iio_device_buffer_attr_write_longlong"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_buffer_attr_write_longlongDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_buffer_attr_write_longlongDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_buffer_attr_write_longlongDefinition, "val", "int64");
%defineOutput(iio_device_buffer_attr_write_longlongDefinition, "RetVal", "int32");
%validate(iio_device_buffer_attr_write_longlongDefinition);

%% C++ function |iio_device_buffer_attr_write_double| with MATLAB name |clib.libm2k.iio_device_buffer_attr_write_double|
% C++ Signature: int iio_device_buffer_attr_write_double(iio_device const * dev,char const * attr,double val)
%iio_device_buffer_attr_write_doubleDefinition = addFunction(libDef, ...
%    "int iio_device_buffer_attr_write_double(iio_device const * dev,char const * attr,double val)", ...
%    "MATLABName", "clib.libm2k.iio_device_buffer_attr_write_double", ...
%    "Description", "clib.libm2k.iio_device_buffer_attr_write_double    Representation of C++ function iio_device_buffer_attr_write_double"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_buffer_attr_write_doubleDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_buffer_attr_write_doubleDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_buffer_attr_write_doubleDefinition, "val", "double");
%defineOutput(iio_device_buffer_attr_write_doubleDefinition, "RetVal", "int32");
%validate(iio_device_buffer_attr_write_doubleDefinition);

%% C++ function |iio_device_set_trigger| with MATLAB name |clib.libm2k.iio_device_set_trigger|
% C++ Signature: int iio_device_set_trigger(iio_device const * dev,iio_device const * trigger)
%iio_device_set_triggerDefinition = addFunction(libDef, ...
%    "int iio_device_set_trigger(iio_device const * dev,iio_device const * trigger)", ...
%    "MATLABName", "clib.libm2k.iio_device_set_trigger", ...
%    "Description", "clib.libm2k.iio_device_set_trigger    Representation of C++ function iio_device_set_trigger"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_set_triggerDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_set_triggerDefinition, "trigger", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineOutput(iio_device_set_triggerDefinition, "RetVal", "int32");
%validate(iio_device_set_triggerDefinition);

%% C++ function |iio_device_is_trigger| with MATLAB name |clib.libm2k.iio_device_is_trigger|
% C++ Signature: bool iio_device_is_trigger(iio_device const * dev)
%iio_device_is_triggerDefinition = addFunction(libDef, ...
%    "bool iio_device_is_trigger(iio_device const * dev)", ...
%    "MATLABName", "clib.libm2k.iio_device_is_trigger", ...
%    "Description", "clib.libm2k.iio_device_is_trigger    Representation of C++ function iio_device_is_trigger"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_is_triggerDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineOutput(iio_device_is_triggerDefinition, "RetVal", "logical");
%validate(iio_device_is_triggerDefinition);

%% C++ function |iio_device_set_kernel_buffers_count| with MATLAB name |clib.libm2k.iio_device_set_kernel_buffers_count|
% C++ Signature: int iio_device_set_kernel_buffers_count(iio_device const * dev,unsigned int nb_buffers)
%iio_device_set_kernel_buffers_countDefinition = addFunction(libDef, ...
%    "int iio_device_set_kernel_buffers_count(iio_device const * dev,unsigned int nb_buffers)", ...
%    "MATLABName", "clib.libm2k.iio_device_set_kernel_buffers_count", ...
%    "Description", "clib.libm2k.iio_device_set_kernel_buffers_count    Representation of C++ function iio_device_set_kernel_buffers_count"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_set_kernel_buffers_countDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_set_kernel_buffers_countDefinition, "nb_buffers", "uint32");
%defineOutput(iio_device_set_kernel_buffers_countDefinition, "RetVal", "int32");
%validate(iio_device_set_kernel_buffers_countDefinition);

%% C++ function |iio_channel_get_device| with MATLAB name |clib.libm2k.iio_channel_get_device|
% C++ Signature: iio_device const * iio_channel_get_device(iio_channel const * chn)
%iio_channel_get_deviceDefinition = addFunction(libDef, ...
%    "iio_device const * iio_channel_get_device(iio_channel const * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_get_device", ...
%    "Description", "clib.libm2k.iio_channel_get_device    Representation of C++ function iio_channel_get_device"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_get_deviceDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineOutput(iio_channel_get_deviceDefinition, "RetVal", "clib.libm2k.iio_device", <SHAPE>);
%validate(iio_channel_get_deviceDefinition);

%% C++ function |iio_channel_get_id| with MATLAB name |clib.libm2k.iio_channel_get_id|
% C++ Signature: char const * iio_channel_get_id(iio_channel const * chn)
%iio_channel_get_idDefinition = addFunction(libDef, ...
%    "char const * iio_channel_get_id(iio_channel const * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_get_id", ...
%    "Description", "clib.libm2k.iio_channel_get_id    Representation of C++ function iio_channel_get_id"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_get_idDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineOutput(iio_channel_get_idDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_channel_get_idDefinition);

%% C++ function |iio_channel_get_name| with MATLAB name |clib.libm2k.iio_channel_get_name|
% C++ Signature: char const * iio_channel_get_name(iio_channel const * chn)
%iio_channel_get_nameDefinition = addFunction(libDef, ...
%    "char const * iio_channel_get_name(iio_channel const * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_get_name", ...
%    "Description", "clib.libm2k.iio_channel_get_name    Representation of C++ function iio_channel_get_name"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_get_nameDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineOutput(iio_channel_get_nameDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_channel_get_nameDefinition);

%% C++ function |iio_channel_is_output| with MATLAB name |clib.libm2k.iio_channel_is_output|
% C++ Signature: bool iio_channel_is_output(iio_channel const * chn)
%iio_channel_is_outputDefinition = addFunction(libDef, ...
%    "bool iio_channel_is_output(iio_channel const * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_is_output", ...
%    "Description", "clib.libm2k.iio_channel_is_output    Representation of C++ function iio_channel_is_output"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_is_outputDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineOutput(iio_channel_is_outputDefinition, "RetVal", "logical");
%validate(iio_channel_is_outputDefinition);

%% C++ function |iio_channel_is_scan_element| with MATLAB name |clib.libm2k.iio_channel_is_scan_element|
% C++ Signature: bool iio_channel_is_scan_element(iio_channel const * chn)
%iio_channel_is_scan_elementDefinition = addFunction(libDef, ...
%    "bool iio_channel_is_scan_element(iio_channel const * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_is_scan_element", ...
%    "Description", "clib.libm2k.iio_channel_is_scan_element    Representation of C++ function iio_channel_is_scan_element"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_is_scan_elementDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineOutput(iio_channel_is_scan_elementDefinition, "RetVal", "logical");
%validate(iio_channel_is_scan_elementDefinition);

%% C++ function |iio_channel_get_attrs_count| with MATLAB name |clib.libm2k.iio_channel_get_attrs_count|
% C++ Signature: unsigned int iio_channel_get_attrs_count(iio_channel const * chn)
%iio_channel_get_attrs_countDefinition = addFunction(libDef, ...
%    "unsigned int iio_channel_get_attrs_count(iio_channel const * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_get_attrs_count", ...
%    "Description", "clib.libm2k.iio_channel_get_attrs_count    Representation of C++ function iio_channel_get_attrs_count"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_get_attrs_countDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineOutput(iio_channel_get_attrs_countDefinition, "RetVal", "uint32");
%validate(iio_channel_get_attrs_countDefinition);

%% C++ function |iio_channel_get_attr| with MATLAB name |clib.libm2k.iio_channel_get_attr|
% C++ Signature: char const * iio_channel_get_attr(iio_channel const * chn,unsigned int index)
%iio_channel_get_attrDefinition = addFunction(libDef, ...
%    "char const * iio_channel_get_attr(iio_channel const * chn,unsigned int index)", ...
%    "MATLABName", "clib.libm2k.iio_channel_get_attr", ...
%    "Description", "clib.libm2k.iio_channel_get_attr    Representation of C++ function iio_channel_get_attr"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_get_attrDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineArgument(iio_channel_get_attrDefinition, "index", "uint32");
%defineOutput(iio_channel_get_attrDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_channel_get_attrDefinition);

%% C++ function |iio_channel_find_attr| with MATLAB name |clib.libm2k.iio_channel_find_attr|
% C++ Signature: char const * iio_channel_find_attr(iio_channel const * chn,char const * name)
%iio_channel_find_attrDefinition = addFunction(libDef, ...
%    "char const * iio_channel_find_attr(iio_channel const * chn,char const * name)", ...
%    "MATLABName", "clib.libm2k.iio_channel_find_attr", ...
%    "Description", "clib.libm2k.iio_channel_find_attr    Representation of C++ function iio_channel_find_attr"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_find_attrDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineArgument(iio_channel_find_attrDefinition, "name", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_channel_find_attrDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_channel_find_attrDefinition);

%% C++ function |iio_channel_attr_get_filename| with MATLAB name |clib.libm2k.iio_channel_attr_get_filename|
% C++ Signature: char const * iio_channel_attr_get_filename(iio_channel const * chn,char const * attr)
%iio_channel_attr_get_filenameDefinition = addFunction(libDef, ...
%    "char const * iio_channel_attr_get_filename(iio_channel const * chn,char const * attr)", ...
%    "MATLABName", "clib.libm2k.iio_channel_attr_get_filename", ...
%    "Description", "clib.libm2k.iio_channel_attr_get_filename    Representation of C++ function iio_channel_attr_get_filename"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_attr_get_filenameDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineArgument(iio_channel_attr_get_filenameDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_channel_attr_get_filenameDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_channel_attr_get_filenameDefinition);

%% C++ function |iio_channel_attr_read| with MATLAB name |clib.libm2k.iio_channel_attr_read|
% C++ Signature: ssize_t iio_channel_attr_read(iio_channel const * chn,char const * attr,char * dst,size_t len)
%iio_channel_attr_readDefinition = addFunction(libDef, ...
%    "ssize_t iio_channel_attr_read(iio_channel const * chn,char const * attr,char * dst,size_t len)", ...
%    "MATLABName", "clib.libm2k.iio_channel_attr_read", ...
%    "Description", "clib.libm2k.iio_channel_attr_read    Representation of C++ function iio_channel_attr_read"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_attr_readDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineArgument(iio_channel_attr_readDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_channel_attr_readDefinition, "dst", "int8", <DIRECTION>, <SHAPE>);
%defineArgument(iio_channel_attr_readDefinition, "len", "uint64");
%defineOutput(iio_channel_attr_readDefinition, "RetVal", "int64");
%validate(iio_channel_attr_readDefinition);

%% C++ function |iio_channel_attr_read_bool| with MATLAB name |clib.libm2k.iio_channel_attr_read_bool|
% C++ Signature: int iio_channel_attr_read_bool(iio_channel const * chn,char const * attr,bool * val)
%iio_channel_attr_read_boolDefinition = addFunction(libDef, ...
%    "int iio_channel_attr_read_bool(iio_channel const * chn,char const * attr,bool * val)", ...
%    "MATLABName", "clib.libm2k.iio_channel_attr_read_bool", ...
%    "Description", "clib.libm2k.iio_channel_attr_read_bool    Representation of C++ function iio_channel_attr_read_bool"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_attr_read_boolDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineArgument(iio_channel_attr_read_boolDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_channel_attr_read_boolDefinition, "val", "logical", <DIRECTION>, <SHAPE>);
%defineOutput(iio_channel_attr_read_boolDefinition, "RetVal", "int32");
%validate(iio_channel_attr_read_boolDefinition);

%% C++ function |iio_channel_attr_read_longlong| with MATLAB name |clib.libm2k.iio_channel_attr_read_longlong|
% C++ Signature: int iio_channel_attr_read_longlong(iio_channel const * chn,char const * attr,long long * val)
%iio_channel_attr_read_longlongDefinition = addFunction(libDef, ...
%    "int iio_channel_attr_read_longlong(iio_channel const * chn,char const * attr,long long * val)", ...
%    "MATLABName", "clib.libm2k.iio_channel_attr_read_longlong", ...
%    "Description", "clib.libm2k.iio_channel_attr_read_longlong    Representation of C++ function iio_channel_attr_read_longlong"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_attr_read_longlongDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineArgument(iio_channel_attr_read_longlongDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_channel_attr_read_longlongDefinition, "val", "int64", <DIRECTION>, <SHAPE>);
%defineOutput(iio_channel_attr_read_longlongDefinition, "RetVal", "int32");
%validate(iio_channel_attr_read_longlongDefinition);

%% C++ function |iio_channel_attr_read_double| with MATLAB name |clib.libm2k.iio_channel_attr_read_double|
% C++ Signature: int iio_channel_attr_read_double(iio_channel const * chn,char const * attr,double * val)
%iio_channel_attr_read_doubleDefinition = addFunction(libDef, ...
%    "int iio_channel_attr_read_double(iio_channel const * chn,char const * attr,double * val)", ...
%    "MATLABName", "clib.libm2k.iio_channel_attr_read_double", ...
%    "Description", "clib.libm2k.iio_channel_attr_read_double    Representation of C++ function iio_channel_attr_read_double"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_attr_read_doubleDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineArgument(iio_channel_attr_read_doubleDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_channel_attr_read_doubleDefinition, "val", "double", <DIRECTION>, <SHAPE>);
%defineOutput(iio_channel_attr_read_doubleDefinition, "RetVal", "int32");
%validate(iio_channel_attr_read_doubleDefinition);

%% C++ function |iio_channel_attr_write| with MATLAB name |clib.libm2k.iio_channel_attr_write|
% C++ Signature: ssize_t iio_channel_attr_write(iio_channel const * chn,char const * attr,char const * src)
%iio_channel_attr_writeDefinition = addFunction(libDef, ...
%    "ssize_t iio_channel_attr_write(iio_channel const * chn,char const * attr,char const * src)", ...
%    "MATLABName", "clib.libm2k.iio_channel_attr_write", ...
%    "Description", "clib.libm2k.iio_channel_attr_write    Representation of C++ function iio_channel_attr_write"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_attr_writeDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineArgument(iio_channel_attr_writeDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_channel_attr_writeDefinition, "src", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_channel_attr_writeDefinition, "RetVal", "int64");
%validate(iio_channel_attr_writeDefinition);

%% C++ function |iio_channel_attr_write_bool| with MATLAB name |clib.libm2k.iio_channel_attr_write_bool|
% C++ Signature: int iio_channel_attr_write_bool(iio_channel const * chn,char const * attr,bool val)
%iio_channel_attr_write_boolDefinition = addFunction(libDef, ...
%    "int iio_channel_attr_write_bool(iio_channel const * chn,char const * attr,bool val)", ...
%    "MATLABName", "clib.libm2k.iio_channel_attr_write_bool", ...
%    "Description", "clib.libm2k.iio_channel_attr_write_bool    Representation of C++ function iio_channel_attr_write_bool"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_attr_write_boolDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineArgument(iio_channel_attr_write_boolDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_channel_attr_write_boolDefinition, "val", "logical");
%defineOutput(iio_channel_attr_write_boolDefinition, "RetVal", "int32");
%validate(iio_channel_attr_write_boolDefinition);

%% C++ function |iio_channel_attr_write_longlong| with MATLAB name |clib.libm2k.iio_channel_attr_write_longlong|
% C++ Signature: int iio_channel_attr_write_longlong(iio_channel const * chn,char const * attr,long long val)
%iio_channel_attr_write_longlongDefinition = addFunction(libDef, ...
%    "int iio_channel_attr_write_longlong(iio_channel const * chn,char const * attr,long long val)", ...
%    "MATLABName", "clib.libm2k.iio_channel_attr_write_longlong", ...
%    "Description", "clib.libm2k.iio_channel_attr_write_longlong    Representation of C++ function iio_channel_attr_write_longlong"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_attr_write_longlongDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineArgument(iio_channel_attr_write_longlongDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_channel_attr_write_longlongDefinition, "val", "int64");
%defineOutput(iio_channel_attr_write_longlongDefinition, "RetVal", "int32");
%validate(iio_channel_attr_write_longlongDefinition);

%% C++ function |iio_channel_attr_write_double| with MATLAB name |clib.libm2k.iio_channel_attr_write_double|
% C++ Signature: int iio_channel_attr_write_double(iio_channel const * chn,char const * attr,double val)
%iio_channel_attr_write_doubleDefinition = addFunction(libDef, ...
%    "int iio_channel_attr_write_double(iio_channel const * chn,char const * attr,double val)", ...
%    "MATLABName", "clib.libm2k.iio_channel_attr_write_double", ...
%    "Description", "clib.libm2k.iio_channel_attr_write_double    Representation of C++ function iio_channel_attr_write_double"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_attr_write_doubleDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineArgument(iio_channel_attr_write_doubleDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_channel_attr_write_doubleDefinition, "val", "double");
%defineOutput(iio_channel_attr_write_doubleDefinition, "RetVal", "int32");
%validate(iio_channel_attr_write_doubleDefinition);

%% C++ function |iio_channel_enable| with MATLAB name |clib.libm2k.iio_channel_enable|
% C++ Signature: void iio_channel_enable(iio_channel * chn)
%iio_channel_enableDefinition = addFunction(libDef, ...
%    "void iio_channel_enable(iio_channel * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_enable", ...
%    "Description", "clib.libm2k.iio_channel_enable    Representation of C++ function iio_channel_enable"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_enableDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%validate(iio_channel_enableDefinition);

%% C++ function |iio_channel_disable| with MATLAB name |clib.libm2k.iio_channel_disable|
% C++ Signature: void iio_channel_disable(iio_channel * chn)
%iio_channel_disableDefinition = addFunction(libDef, ...
%    "void iio_channel_disable(iio_channel * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_disable", ...
%    "Description", "clib.libm2k.iio_channel_disable    Representation of C++ function iio_channel_disable"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_disableDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%validate(iio_channel_disableDefinition);

%% C++ function |iio_channel_is_enabled| with MATLAB name |clib.libm2k.iio_channel_is_enabled|
% C++ Signature: bool iio_channel_is_enabled(iio_channel const * chn)
%iio_channel_is_enabledDefinition = addFunction(libDef, ...
%    "bool iio_channel_is_enabled(iio_channel const * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_is_enabled", ...
%    "Description", "clib.libm2k.iio_channel_is_enabled    Representation of C++ function iio_channel_is_enabled"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_is_enabledDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineOutput(iio_channel_is_enabledDefinition, "RetVal", "logical");
%validate(iio_channel_is_enabledDefinition);

%% C++ function |iio_channel_get_type| with MATLAB name |clib.libm2k.iio_channel_get_type|
% C++ Signature: iio_chan_type iio_channel_get_type(iio_channel const * chn)
%iio_channel_get_typeDefinition = addFunction(libDef, ...
%    "iio_chan_type iio_channel_get_type(iio_channel const * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_get_type", ...
%    "Description", "clib.libm2k.iio_channel_get_type    Representation of C++ function iio_channel_get_type"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_get_typeDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineOutput(iio_channel_get_typeDefinition, "RetVal", "clib.libm2k.iio_chan_type");
%validate(iio_channel_get_typeDefinition);

%% C++ function |iio_channel_get_modifier| with MATLAB name |clib.libm2k.iio_channel_get_modifier|
% C++ Signature: iio_modifier iio_channel_get_modifier(iio_channel const * chn)
%iio_channel_get_modifierDefinition = addFunction(libDef, ...
%    "iio_modifier iio_channel_get_modifier(iio_channel const * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_get_modifier", ...
%    "Description", "clib.libm2k.iio_channel_get_modifier    Representation of C++ function iio_channel_get_modifier"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_get_modifierDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineOutput(iio_channel_get_modifierDefinition, "RetVal", "clib.libm2k.iio_modifier");
%validate(iio_channel_get_modifierDefinition);

%% C++ function |iio_buffer_get_device| with MATLAB name |clib.libm2k.iio_buffer_get_device|
% C++ Signature: iio_device const * iio_buffer_get_device(iio_buffer const * buf)
%iio_buffer_get_deviceDefinition = addFunction(libDef, ...
%    "iio_device const * iio_buffer_get_device(iio_buffer const * buf)", ...
%    "MATLABName", "clib.libm2k.iio_buffer_get_device", ...
%    "Description", "clib.libm2k.iio_buffer_get_device    Representation of C++ function iio_buffer_get_device"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_buffer_get_deviceDefinition, "buf", "clib.libm2k.iio_buffer", "input", <SHAPE>);
%defineOutput(iio_buffer_get_deviceDefinition, "RetVal", "clib.libm2k.iio_device", <SHAPE>);
%validate(iio_buffer_get_deviceDefinition);

%% C++ function |iio_device_create_buffer| with MATLAB name |clib.libm2k.iio_device_create_buffer|
% C++ Signature: iio_buffer * iio_device_create_buffer(iio_device const * dev,size_t samples_count,bool cyclic)
%iio_device_create_bufferDefinition = addFunction(libDef, ...
%    "iio_buffer * iio_device_create_buffer(iio_device const * dev,size_t samples_count,bool cyclic)", ...
%    "MATLABName", "clib.libm2k.iio_device_create_buffer", ...
%    "Description", "clib.libm2k.iio_device_create_buffer    Representation of C++ function iio_device_create_buffer"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_create_bufferDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_create_bufferDefinition, "samples_count", "uint64");
%defineArgument(iio_device_create_bufferDefinition, "cyclic", "logical");
%defineOutput(iio_device_create_bufferDefinition, "RetVal", "clib.libm2k.iio_buffer", <SHAPE>);
%validate(iio_device_create_bufferDefinition);

%% C++ function |iio_buffer_destroy| with MATLAB name |clib.libm2k.iio_buffer_destroy|
% C++ Signature: void iio_buffer_destroy(iio_buffer * buf)
%iio_buffer_destroyDefinition = addFunction(libDef, ...
%    "void iio_buffer_destroy(iio_buffer * buf)", ...
%    "MATLABName", "clib.libm2k.iio_buffer_destroy", ...
%    "Description", "clib.libm2k.iio_buffer_destroy    Representation of C++ function iio_buffer_destroy"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_buffer_destroyDefinition, "buf", "clib.libm2k.iio_buffer", "input", <SHAPE>);
%validate(iio_buffer_destroyDefinition);

%% C++ function |iio_buffer_get_poll_fd| with MATLAB name |clib.libm2k.iio_buffer_get_poll_fd|
% C++ Signature: int iio_buffer_get_poll_fd(iio_buffer * buf)
%iio_buffer_get_poll_fdDefinition = addFunction(libDef, ...
%    "int iio_buffer_get_poll_fd(iio_buffer * buf)", ...
%    "MATLABName", "clib.libm2k.iio_buffer_get_poll_fd", ...
%    "Description", "clib.libm2k.iio_buffer_get_poll_fd    Representation of C++ function iio_buffer_get_poll_fd"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_buffer_get_poll_fdDefinition, "buf", "clib.libm2k.iio_buffer", "input", <SHAPE>);
%defineOutput(iio_buffer_get_poll_fdDefinition, "RetVal", "int32");
%validate(iio_buffer_get_poll_fdDefinition);

%% C++ function |iio_buffer_set_blocking_mode| with MATLAB name |clib.libm2k.iio_buffer_set_blocking_mode|
% C++ Signature: int iio_buffer_set_blocking_mode(iio_buffer * buf,bool blocking)
%iio_buffer_set_blocking_modeDefinition = addFunction(libDef, ...
%    "int iio_buffer_set_blocking_mode(iio_buffer * buf,bool blocking)", ...
%    "MATLABName", "clib.libm2k.iio_buffer_set_blocking_mode", ...
%    "Description", "clib.libm2k.iio_buffer_set_blocking_mode    Representation of C++ function iio_buffer_set_blocking_mode"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_buffer_set_blocking_modeDefinition, "buf", "clib.libm2k.iio_buffer", "input", <SHAPE>);
%defineArgument(iio_buffer_set_blocking_modeDefinition, "blocking", "logical");
%defineOutput(iio_buffer_set_blocking_modeDefinition, "RetVal", "int32");
%validate(iio_buffer_set_blocking_modeDefinition);

%% C++ function |iio_buffer_refill| with MATLAB name |clib.libm2k.iio_buffer_refill|
% C++ Signature: ssize_t iio_buffer_refill(iio_buffer * buf)
%iio_buffer_refillDefinition = addFunction(libDef, ...
%    "ssize_t iio_buffer_refill(iio_buffer * buf)", ...
%    "MATLABName", "clib.libm2k.iio_buffer_refill", ...
%    "Description", "clib.libm2k.iio_buffer_refill    Representation of C++ function iio_buffer_refill"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_buffer_refillDefinition, "buf", "clib.libm2k.iio_buffer", "input", <SHAPE>);
%defineOutput(iio_buffer_refillDefinition, "RetVal", "int64");
%validate(iio_buffer_refillDefinition);

%% C++ function |iio_buffer_push| with MATLAB name |clib.libm2k.iio_buffer_push|
% C++ Signature: ssize_t iio_buffer_push(iio_buffer * buf)
%iio_buffer_pushDefinition = addFunction(libDef, ...
%    "ssize_t iio_buffer_push(iio_buffer * buf)", ...
%    "MATLABName", "clib.libm2k.iio_buffer_push", ...
%    "Description", "clib.libm2k.iio_buffer_push    Representation of C++ function iio_buffer_push"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_buffer_pushDefinition, "buf", "clib.libm2k.iio_buffer", "input", <SHAPE>);
%defineOutput(iio_buffer_pushDefinition, "RetVal", "int64");
%validate(iio_buffer_pushDefinition);

%% C++ function |iio_buffer_push_partial| with MATLAB name |clib.libm2k.iio_buffer_push_partial|
% C++ Signature: ssize_t iio_buffer_push_partial(iio_buffer * buf,size_t samples_count)
%iio_buffer_push_partialDefinition = addFunction(libDef, ...
%    "ssize_t iio_buffer_push_partial(iio_buffer * buf,size_t samples_count)", ...
%    "MATLABName", "clib.libm2k.iio_buffer_push_partial", ...
%    "Description", "clib.libm2k.iio_buffer_push_partial    Representation of C++ function iio_buffer_push_partial"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_buffer_push_partialDefinition, "buf", "clib.libm2k.iio_buffer", "input", <SHAPE>);
%defineArgument(iio_buffer_push_partialDefinition, "samples_count", "uint64");
%defineOutput(iio_buffer_push_partialDefinition, "RetVal", "int64");
%validate(iio_buffer_push_partialDefinition);

%% C++ function |iio_buffer_cancel| with MATLAB name |clib.libm2k.iio_buffer_cancel|
% C++ Signature: void iio_buffer_cancel(iio_buffer * buf)
%iio_buffer_cancelDefinition = addFunction(libDef, ...
%    "void iio_buffer_cancel(iio_buffer * buf)", ...
%    "MATLABName", "clib.libm2k.iio_buffer_cancel", ...
%    "Description", "clib.libm2k.iio_buffer_cancel    Representation of C++ function iio_buffer_cancel"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_buffer_cancelDefinition, "buf", "clib.libm2k.iio_buffer", "input", <SHAPE>);
%validate(iio_buffer_cancelDefinition);

%% C++ function |iio_buffer_step| with MATLAB name |clib.libm2k.iio_buffer_step|
% C++ Signature: ptrdiff_t iio_buffer_step(iio_buffer const * buf)
%iio_buffer_stepDefinition = addFunction(libDef, ...
%    "ptrdiff_t iio_buffer_step(iio_buffer const * buf)", ...
%    "MATLABName", "clib.libm2k.iio_buffer_step", ...
%    "Description", "clib.libm2k.iio_buffer_step    Representation of C++ function iio_buffer_step"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_buffer_stepDefinition, "buf", "clib.libm2k.iio_buffer", "input", <SHAPE>);
%defineOutput(iio_buffer_stepDefinition, "RetVal", "int64");
%validate(iio_buffer_stepDefinition);

%% C++ function |iio_device_get_sample_size| with MATLAB name |clib.libm2k.iio_device_get_sample_size|
% C++ Signature: ssize_t iio_device_get_sample_size(iio_device const * dev)
%iio_device_get_sample_sizeDefinition = addFunction(libDef, ...
%    "ssize_t iio_device_get_sample_size(iio_device const * dev)", ...
%    "MATLABName", "clib.libm2k.iio_device_get_sample_size", ...
%    "Description", "clib.libm2k.iio_device_get_sample_size    Representation of C++ function iio_device_get_sample_size"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_get_sample_sizeDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineOutput(iio_device_get_sample_sizeDefinition, "RetVal", "int64");
%validate(iio_device_get_sample_sizeDefinition);

%% C++ function |iio_channel_get_index| with MATLAB name |clib.libm2k.iio_channel_get_index|
% C++ Signature: long iio_channel_get_index(iio_channel const * chn)
%iio_channel_get_indexDefinition = addFunction(libDef, ...
%    "long iio_channel_get_index(iio_channel const * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_get_index", ...
%    "Description", "clib.libm2k.iio_channel_get_index    Representation of C++ function iio_channel_get_index"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_get_indexDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineOutput(iio_channel_get_indexDefinition, "RetVal", "int64");
%validate(iio_channel_get_indexDefinition);

%% C++ function |iio_channel_get_data_format| with MATLAB name |clib.libm2k.iio_channel_get_data_format|
% C++ Signature: iio_data_format const * iio_channel_get_data_format(iio_channel const * chn)
%iio_channel_get_data_formatDefinition = addFunction(libDef, ...
%    "iio_data_format const * iio_channel_get_data_format(iio_channel const * chn)", ...
%    "MATLABName", "clib.libm2k.iio_channel_get_data_format", ...
%    "Description", "clib.libm2k.iio_channel_get_data_format    Representation of C++ function iio_channel_get_data_format"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_channel_get_data_formatDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineOutput(iio_channel_get_data_formatDefinition, "RetVal", "clib.libm2k.iio_data_format", <SHAPE>);
%validate(iio_channel_get_data_formatDefinition);

%% C++ function |iio_device_get_debug_attrs_count| with MATLAB name |clib.libm2k.iio_device_get_debug_attrs_count|
% C++ Signature: unsigned int iio_device_get_debug_attrs_count(iio_device const * dev)
%iio_device_get_debug_attrs_countDefinition = addFunction(libDef, ...
%    "unsigned int iio_device_get_debug_attrs_count(iio_device const * dev)", ...
%    "MATLABName", "clib.libm2k.iio_device_get_debug_attrs_count", ...
%    "Description", "clib.libm2k.iio_device_get_debug_attrs_count    Representation of C++ function iio_device_get_debug_attrs_count"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_get_debug_attrs_countDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineOutput(iio_device_get_debug_attrs_countDefinition, "RetVal", "uint32");
%validate(iio_device_get_debug_attrs_countDefinition);

%% C++ function |iio_device_get_debug_attr| with MATLAB name |clib.libm2k.iio_device_get_debug_attr|
% C++ Signature: char const * iio_device_get_debug_attr(iio_device const * dev,unsigned int index)
%iio_device_get_debug_attrDefinition = addFunction(libDef, ...
%    "char const * iio_device_get_debug_attr(iio_device const * dev,unsigned int index)", ...
%    "MATLABName", "clib.libm2k.iio_device_get_debug_attr", ...
%    "Description", "clib.libm2k.iio_device_get_debug_attr    Representation of C++ function iio_device_get_debug_attr"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_get_debug_attrDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_get_debug_attrDefinition, "index", "uint32");
%defineOutput(iio_device_get_debug_attrDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_device_get_debug_attrDefinition);

%% C++ function |iio_device_find_debug_attr| with MATLAB name |clib.libm2k.iio_device_find_debug_attr|
% C++ Signature: char const * iio_device_find_debug_attr(iio_device const * dev,char const * name)
%iio_device_find_debug_attrDefinition = addFunction(libDef, ...
%    "char const * iio_device_find_debug_attr(iio_device const * dev,char const * name)", ...
%    "MATLABName", "clib.libm2k.iio_device_find_debug_attr", ...
%    "Description", "clib.libm2k.iio_device_find_debug_attr    Representation of C++ function iio_device_find_debug_attr"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_find_debug_attrDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_find_debug_attrDefinition, "name", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_device_find_debug_attrDefinition, "RetVal", <MLTYPE>, <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%validate(iio_device_find_debug_attrDefinition);

%% C++ function |iio_device_debug_attr_read| with MATLAB name |clib.libm2k.iio_device_debug_attr_read|
% C++ Signature: ssize_t iio_device_debug_attr_read(iio_device const * dev,char const * attr,char * dst,size_t len)
%iio_device_debug_attr_readDefinition = addFunction(libDef, ...
%    "ssize_t iio_device_debug_attr_read(iio_device const * dev,char const * attr,char * dst,size_t len)", ...
%    "MATLABName", "clib.libm2k.iio_device_debug_attr_read", ...
%    "Description", "clib.libm2k.iio_device_debug_attr_read    Representation of C++ function iio_device_debug_attr_read"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_debug_attr_readDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_debug_attr_readDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_debug_attr_readDefinition, "dst", "int8", <DIRECTION>, <SHAPE>);
%defineArgument(iio_device_debug_attr_readDefinition, "len", "uint64");
%defineOutput(iio_device_debug_attr_readDefinition, "RetVal", "int64");
%validate(iio_device_debug_attr_readDefinition);

%% C++ function |iio_device_debug_attr_write| with MATLAB name |clib.libm2k.iio_device_debug_attr_write|
% C++ Signature: ssize_t iio_device_debug_attr_write(iio_device const * dev,char const * attr,char const * src)
%iio_device_debug_attr_writeDefinition = addFunction(libDef, ...
%    "ssize_t iio_device_debug_attr_write(iio_device const * dev,char const * attr,char const * src)", ...
%    "MATLABName", "clib.libm2k.iio_device_debug_attr_write", ...
%    "Description", "clib.libm2k.iio_device_debug_attr_write    Representation of C++ function iio_device_debug_attr_write"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_debug_attr_writeDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_debug_attr_writeDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_debug_attr_writeDefinition, "src", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(iio_device_debug_attr_writeDefinition, "RetVal", "int64");
%validate(iio_device_debug_attr_writeDefinition);

%% C++ function |iio_device_debug_attr_read_bool| with MATLAB name |clib.libm2k.iio_device_debug_attr_read_bool|
% C++ Signature: int iio_device_debug_attr_read_bool(iio_device const * dev,char const * attr,bool * val)
%iio_device_debug_attr_read_boolDefinition = addFunction(libDef, ...
%    "int iio_device_debug_attr_read_bool(iio_device const * dev,char const * attr,bool * val)", ...
%    "MATLABName", "clib.libm2k.iio_device_debug_attr_read_bool", ...
%    "Description", "clib.libm2k.iio_device_debug_attr_read_bool    Representation of C++ function iio_device_debug_attr_read_bool"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_debug_attr_read_boolDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_debug_attr_read_boolDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_debug_attr_read_boolDefinition, "val", "logical", <DIRECTION>, <SHAPE>);
%defineOutput(iio_device_debug_attr_read_boolDefinition, "RetVal", "int32");
%validate(iio_device_debug_attr_read_boolDefinition);

%% C++ function |iio_device_debug_attr_read_longlong| with MATLAB name |clib.libm2k.iio_device_debug_attr_read_longlong|
% C++ Signature: int iio_device_debug_attr_read_longlong(iio_device const * dev,char const * attr,long long * val)
%iio_device_debug_attr_read_longlongDefinition = addFunction(libDef, ...
%    "int iio_device_debug_attr_read_longlong(iio_device const * dev,char const * attr,long long * val)", ...
%    "MATLABName", "clib.libm2k.iio_device_debug_attr_read_longlong", ...
%    "Description", "clib.libm2k.iio_device_debug_attr_read_longlong    Representation of C++ function iio_device_debug_attr_read_longlong"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_debug_attr_read_longlongDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_debug_attr_read_longlongDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_debug_attr_read_longlongDefinition, "val", "int64", <DIRECTION>, <SHAPE>);
%defineOutput(iio_device_debug_attr_read_longlongDefinition, "RetVal", "int32");
%validate(iio_device_debug_attr_read_longlongDefinition);

%% C++ function |iio_device_debug_attr_read_double| with MATLAB name |clib.libm2k.iio_device_debug_attr_read_double|
% C++ Signature: int iio_device_debug_attr_read_double(iio_device const * dev,char const * attr,double * val)
%iio_device_debug_attr_read_doubleDefinition = addFunction(libDef, ...
%    "int iio_device_debug_attr_read_double(iio_device const * dev,char const * attr,double * val)", ...
%    "MATLABName", "clib.libm2k.iio_device_debug_attr_read_double", ...
%    "Description", "clib.libm2k.iio_device_debug_attr_read_double    Representation of C++ function iio_device_debug_attr_read_double"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_debug_attr_read_doubleDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_debug_attr_read_doubleDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_debug_attr_read_doubleDefinition, "val", "double", <DIRECTION>, <SHAPE>);
%defineOutput(iio_device_debug_attr_read_doubleDefinition, "RetVal", "int32");
%validate(iio_device_debug_attr_read_doubleDefinition);

%% C++ function |iio_device_debug_attr_write_bool| with MATLAB name |clib.libm2k.iio_device_debug_attr_write_bool|
% C++ Signature: int iio_device_debug_attr_write_bool(iio_device const * dev,char const * attr,bool val)
%iio_device_debug_attr_write_boolDefinition = addFunction(libDef, ...
%    "int iio_device_debug_attr_write_bool(iio_device const * dev,char const * attr,bool val)", ...
%    "MATLABName", "clib.libm2k.iio_device_debug_attr_write_bool", ...
%    "Description", "clib.libm2k.iio_device_debug_attr_write_bool    Representation of C++ function iio_device_debug_attr_write_bool"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_debug_attr_write_boolDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_debug_attr_write_boolDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_debug_attr_write_boolDefinition, "val", "logical");
%defineOutput(iio_device_debug_attr_write_boolDefinition, "RetVal", "int32");
%validate(iio_device_debug_attr_write_boolDefinition);

%% C++ function |iio_device_debug_attr_write_longlong| with MATLAB name |clib.libm2k.iio_device_debug_attr_write_longlong|
% C++ Signature: int iio_device_debug_attr_write_longlong(iio_device const * dev,char const * attr,long long val)
%iio_device_debug_attr_write_longlongDefinition = addFunction(libDef, ...
%    "int iio_device_debug_attr_write_longlong(iio_device const * dev,char const * attr,long long val)", ...
%    "MATLABName", "clib.libm2k.iio_device_debug_attr_write_longlong", ...
%    "Description", "clib.libm2k.iio_device_debug_attr_write_longlong    Representation of C++ function iio_device_debug_attr_write_longlong"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_debug_attr_write_longlongDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_debug_attr_write_longlongDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_debug_attr_write_longlongDefinition, "val", "int64");
%defineOutput(iio_device_debug_attr_write_longlongDefinition, "RetVal", "int32");
%validate(iio_device_debug_attr_write_longlongDefinition);

%% C++ function |iio_device_debug_attr_write_double| with MATLAB name |clib.libm2k.iio_device_debug_attr_write_double|
% C++ Signature: int iio_device_debug_attr_write_double(iio_device const * dev,char const * attr,double val)
%iio_device_debug_attr_write_doubleDefinition = addFunction(libDef, ...
%    "int iio_device_debug_attr_write_double(iio_device const * dev,char const * attr,double val)", ...
%    "MATLABName", "clib.libm2k.iio_device_debug_attr_write_double", ...
%    "Description", "clib.libm2k.iio_device_debug_attr_write_double    Representation of C++ function iio_device_debug_attr_write_double"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_debug_attr_write_doubleDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_debug_attr_write_doubleDefinition, "attr", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineArgument(iio_device_debug_attr_write_doubleDefinition, "val", "double");
%defineOutput(iio_device_debug_attr_write_doubleDefinition, "RetVal", "int32");
%validate(iio_device_debug_attr_write_doubleDefinition);

%% C++ function |iio_device_reg_write| with MATLAB name |clib.libm2k.iio_device_reg_write|
% C++ Signature: int iio_device_reg_write(iio_device * dev,uint32_t address,uint32_t value)
%iio_device_reg_writeDefinition = addFunction(libDef, ...
%    "int iio_device_reg_write(iio_device * dev,uint32_t address,uint32_t value)", ...
%    "MATLABName", "clib.libm2k.iio_device_reg_write", ...
%    "Description", "clib.libm2k.iio_device_reg_write    Representation of C++ function iio_device_reg_write"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_reg_writeDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_reg_writeDefinition, "address", "uint32");
%defineArgument(iio_device_reg_writeDefinition, "value", "uint32");
%defineOutput(iio_device_reg_writeDefinition, "RetVal", "int32");
%validate(iio_device_reg_writeDefinition);

%% C++ function |iio_device_reg_read| with MATLAB name |clib.libm2k.iio_device_reg_read|
% C++ Signature: int iio_device_reg_read(iio_device * dev,uint32_t address,uint32_t * value)
%iio_device_reg_readDefinition = addFunction(libDef, ...
%    "int iio_device_reg_read(iio_device * dev,uint32_t address,uint32_t * value)", ...
%    "MATLABName", "clib.libm2k.iio_device_reg_read", ...
%    "Description", "clib.libm2k.iio_device_reg_read    Representation of C++ function iio_device_reg_read"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iio_device_reg_readDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iio_device_reg_readDefinition, "address", "uint32");
%defineArgument(iio_device_reg_readDefinition, "value", "uint32", <DIRECTION>, <SHAPE>);
%defineOutput(iio_device_reg_readDefinition, "RetVal", "int32");
%validate(iio_device_reg_readDefinition);

%% C++ function |throw_exception| with MATLAB name |clib.libm2k.throw_exception|
% C++ Signature: void throw_exception(libm2k::M2K_EXCEPTION exc_type,std::string exception)
throw_exceptionDefinition = addFunction(libDef, ...
    "void throw_exception(libm2k::M2K_EXCEPTION exc_type,std::string exception)", ...
    "MATLABName", "clib.libm2k.throw_exception", ...
    "Description", "clib.libm2k.throw_exception    Representation of C++ function throw_exception"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(throw_exceptionDefinition, "exc_type", "clib.libm2k.libm2k.M2K_EXCEPTION");
defineArgument(throw_exceptionDefinition, "exception", "string");
validate(throw_exceptionDefinition);

%% C++ enumeration |libm2k::CALIBRATION_MODE| with MATLAB name |clib.libm2k.libm2k.CALIBRATION_MODE| 
addEnumeration(libDef, "libm2k::CALIBRATION_MODE", "int32",...
    [...
      "ADC_REF1",...  % 0
      "ADC_REF2",...  % 1
      "ADC_GND",...  % 2
      "DAC",...  % 3
      "NONE",...  % 4
    ],...
    "MATLABName", "clib.libm2k.libm2k.CALIBRATION_MODE", ...
    "Description", "clib.libm2k.libm2k.CALIBRATION_MODE    Representation of C++ enumeration libm2k::CALIBRATION_MODE"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |libm2k::GAIN_MODE| with MATLAB name |clib.libm2k.libm2k.GAIN_MODE| 
addEnumeration(libDef, "libm2k::GAIN_MODE", "int32",...
    [...
      "LOW_GAIN",...  % 0
      "HIGH_GAIN",...  % 1
    ],...
    "MATLABName", "clib.libm2k.libm2k.GAIN_MODE", ...
    "Description", "clib.libm2k.libm2k.GAIN_MODE    Representation of C++ enumeration libm2k::GAIN_MODE"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |libm2k::M2K_EXCEPTION| with MATLAB name |clib.libm2k.libm2k.M2K_EXCEPTION| 
addEnumeration(libDef, "libm2k::M2K_EXCEPTION", "int32",...
    [...
      "EXC_OUT_OF_RANGE",...  % 0
      "EXC_RUNTIME_ERROR",...  % 1
      "EXC_INVALID_PARAMETER",...  % 2
      "EXC_TIMEOUT",...  % 3
    ],...
    "MATLABName", "clib.libm2k.libm2k.M2K_EXCEPTION", ...
    "Description", "clib.libm2k.libm2k.M2K_EXCEPTION    Representation of C++ enumeration libm2k::M2K_EXCEPTION"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |libm2k::M2kCalibration| with MATLAB name |clib.libm2k.libm2k.M2kCalibration| 
M2kCalibrationDefinition = addClass(libDef, "libm2k::M2kCalibration", "MATLABName", "clib.libm2k.libm2k.M2kCalibration", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration    Representation of C++ class libm2k::M2kCalibration"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::M2kCalibration| 
% C++ Signature: libm2k::M2kCalibration::M2kCalibration(iio_context * ctx,libm2k::analog::M2kAnalogIn * analogIn,libm2k::analog::M2kAnalogOut * analogOut)
%M2kCalibrationConstructor1Definition = addConstructor(M2kCalibrationDefinition, ...
%    "libm2k::M2kCalibration::M2kCalibration(iio_context * ctx,libm2k::analog::M2kAnalogIn * analogIn,libm2k::analog::M2kAnalogOut * analogOut)", ...
%    "Description", "clib.libm2k.libm2k.M2kCalibration    Constructor of C++ class libm2k::M2kCalibration"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(M2kCalibrationConstructor1Definition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineArgument(M2kCalibrationConstructor1Definition, "analogIn", "clib.libm2k.libm2k.analog.M2kAnalogIn", "input", <SHAPE>);
%defineArgument(M2kCalibrationConstructor1Definition, "analogOut", "clib.libm2k.libm2k.analog.M2kAnalogOut", "input", <SHAPE>);
%validate(M2kCalibrationConstructor1Definition);

%% C++ class method |initialize| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: bool libm2k::M2kCalibration::initialize()
initializeDefinition = addMethod(M2kCalibrationDefinition, ...
    "bool libm2k::M2kCalibration::initialize()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.initialize    Method of C++ class libm2k::M2kCalibration::initialize"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(initializeDefinition, "RetVal", "logical");
validate(initializeDefinition);

%% C++ class method |isInitialized| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: bool libm2k::M2kCalibration::isInitialized() const
isInitializedDefinition = addMethod(M2kCalibrationDefinition, ...
    "bool libm2k::M2kCalibration::isInitialized() const", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.isInitialized    Method of C++ class libm2k::M2kCalibration::isInitialized"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(isInitializedDefinition, "RetVal", "logical");
validate(isInitializedDefinition);

%% C++ class method |setAdcInCalibMode| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: void libm2k::M2kCalibration::setAdcInCalibMode()
setAdcInCalibModeDefinition = addMethod(M2kCalibrationDefinition, ...
    "void libm2k::M2kCalibration::setAdcInCalibMode()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.setAdcInCalibMode    Method of C++ class libm2k::M2kCalibration::setAdcInCalibMode"); % This description is shown as help to user. Modify it to appropriate description.
validate(setAdcInCalibModeDefinition);

%% C++ class method |setDacInCalibMode| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: void libm2k::M2kCalibration::setDacInCalibMode()
setDacInCalibModeDefinition = addMethod(M2kCalibrationDefinition, ...
    "void libm2k::M2kCalibration::setDacInCalibMode()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.setDacInCalibMode    Method of C++ class libm2k::M2kCalibration::setDacInCalibMode"); % This description is shown as help to user. Modify it to appropriate description.
validate(setDacInCalibModeDefinition);

%% C++ class method |restoreAdcFromCalibMode| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: void libm2k::M2kCalibration::restoreAdcFromCalibMode()
restoreAdcFromCalibModeDefinition = addMethod(M2kCalibrationDefinition, ...
    "void libm2k::M2kCalibration::restoreAdcFromCalibMode()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.restoreAdcFromCalibMode    Method of C++ class libm2k::M2kCalibration::restoreAdcFromCalibMode"); % This description is shown as help to user. Modify it to appropriate description.
validate(restoreAdcFromCalibModeDefinition);

%% C++ class method |restoreDacFromCalibMode| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: void libm2k::M2kCalibration::restoreDacFromCalibMode()
restoreDacFromCalibModeDefinition = addMethod(M2kCalibrationDefinition, ...
    "void libm2k::M2kCalibration::restoreDacFromCalibMode()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.restoreDacFromCalibMode    Method of C++ class libm2k::M2kCalibration::restoreDacFromCalibMode"); % This description is shown as help to user. Modify it to appropriate description.
validate(restoreDacFromCalibModeDefinition);

%% C++ class method |calibrateAll| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: bool libm2k::M2kCalibration::calibrateAll()
calibrateAllDefinition = addMethod(M2kCalibrationDefinition, ...
    "bool libm2k::M2kCalibration::calibrateAll()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.calibrateAll    Method of C++ class libm2k::M2kCalibration::calibrateAll"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(calibrateAllDefinition, "RetVal", "logical");
validate(calibrateAllDefinition);

%% C++ class method |calibrateADC| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: bool libm2k::M2kCalibration::calibrateADC()
calibrateADCDefinition = addMethod(M2kCalibrationDefinition, ...
    "bool libm2k::M2kCalibration::calibrateADC()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.calibrateADC    Method of C++ class libm2k::M2kCalibration::calibrateADC"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(calibrateADCDefinition, "RetVal", "logical");
validate(calibrateADCDefinition);

%% C++ class method |calibrateADCoffset| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: bool libm2k::M2kCalibration::calibrateADCoffset()
calibrateADCoffsetDefinition = addMethod(M2kCalibrationDefinition, ...
    "bool libm2k::M2kCalibration::calibrateADCoffset()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.calibrateADCoffset    Method of C++ class libm2k::M2kCalibration::calibrateADCoffset"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(calibrateADCoffsetDefinition, "RetVal", "logical");
validate(calibrateADCoffsetDefinition);

%% C++ class method |calibrateADCgain| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: bool libm2k::M2kCalibration::calibrateADCgain()
calibrateADCgainDefinition = addMethod(M2kCalibrationDefinition, ...
    "bool libm2k::M2kCalibration::calibrateADCgain()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.calibrateADCgain    Method of C++ class libm2k::M2kCalibration::calibrateADCgain"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(calibrateADCgainDefinition, "RetVal", "logical");
validate(calibrateADCgainDefinition);

%% C++ class method |calibrateDAC| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: bool libm2k::M2kCalibration::calibrateDAC()
calibrateDACDefinition = addMethod(M2kCalibrationDefinition, ...
    "bool libm2k::M2kCalibration::calibrateDAC()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.calibrateDAC    Method of C++ class libm2k::M2kCalibration::calibrateDAC"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(calibrateDACDefinition, "RetVal", "logical");
validate(calibrateDACDefinition);

%% C++ class method |calibrateDACoffset| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: bool libm2k::M2kCalibration::calibrateDACoffset()
calibrateDACoffsetDefinition = addMethod(M2kCalibrationDefinition, ...
    "bool libm2k::M2kCalibration::calibrateDACoffset()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.calibrateDACoffset    Method of C++ class libm2k::M2kCalibration::calibrateDACoffset"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(calibrateDACoffsetDefinition, "RetVal", "logical");
validate(calibrateDACoffsetDefinition);

%% C++ class method |calibrateDACgain| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: bool libm2k::M2kCalibration::calibrateDACgain()
calibrateDACgainDefinition = addMethod(M2kCalibrationDefinition, ...
    "bool libm2k::M2kCalibration::calibrateDACgain()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.calibrateDACgain    Method of C++ class libm2k::M2kCalibration::calibrateDACgain"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(calibrateDACgainDefinition, "RetVal", "logical");
validate(calibrateDACgainDefinition);

%% C++ class method |cancelCalibration| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: void libm2k::M2kCalibration::cancelCalibration()
cancelCalibrationDefinition = addMethod(M2kCalibrationDefinition, ...
    "void libm2k::M2kCalibration::cancelCalibration()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.cancelCalibration    Method of C++ class libm2k::M2kCalibration::cancelCalibration"); % This description is shown as help to user. Modify it to appropriate description.
validate(cancelCalibrationDefinition);

%% C++ class method |adcOffsetChannel0| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: int libm2k::M2kCalibration::adcOffsetChannel0() const
adcOffsetChannel0Definition = addMethod(M2kCalibrationDefinition, ...
    "int libm2k::M2kCalibration::adcOffsetChannel0() const", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.adcOffsetChannel0    Method of C++ class libm2k::M2kCalibration::adcOffsetChannel0"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(adcOffsetChannel0Definition, "RetVal", "int32");
validate(adcOffsetChannel0Definition);

%% C++ class method |adcOffsetChannel1| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: int libm2k::M2kCalibration::adcOffsetChannel1() const
adcOffsetChannel1Definition = addMethod(M2kCalibrationDefinition, ...
    "int libm2k::M2kCalibration::adcOffsetChannel1() const", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.adcOffsetChannel1    Method of C++ class libm2k::M2kCalibration::adcOffsetChannel1"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(adcOffsetChannel1Definition, "RetVal", "int32");
validate(adcOffsetChannel1Definition);

%% C++ class method |dacAoffset| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: int libm2k::M2kCalibration::dacAoffset() const
dacAoffsetDefinition = addMethod(M2kCalibrationDefinition, ...
    "int libm2k::M2kCalibration::dacAoffset() const", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.dacAoffset    Method of C++ class libm2k::M2kCalibration::dacAoffset"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(dacAoffsetDefinition, "RetVal", "int32");
validate(dacAoffsetDefinition);

%% C++ class method |dacBoffset| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: int libm2k::M2kCalibration::dacBoffset() const
dacBoffsetDefinition = addMethod(M2kCalibrationDefinition, ...
    "int libm2k::M2kCalibration::dacBoffset() const", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.dacBoffset    Method of C++ class libm2k::M2kCalibration::dacBoffset"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(dacBoffsetDefinition, "RetVal", "int32");
validate(dacBoffsetDefinition);

%% C++ class method |adcGainChannel0| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: double libm2k::M2kCalibration::adcGainChannel0() const
adcGainChannel0Definition = addMethod(M2kCalibrationDefinition, ...
    "double libm2k::M2kCalibration::adcGainChannel0() const", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.adcGainChannel0    Method of C++ class libm2k::M2kCalibration::adcGainChannel0"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(adcGainChannel0Definition, "RetVal", "double");
validate(adcGainChannel0Definition);

%% C++ class method |adcGainChannel1| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: double libm2k::M2kCalibration::adcGainChannel1() const
adcGainChannel1Definition = addMethod(M2kCalibrationDefinition, ...
    "double libm2k::M2kCalibration::adcGainChannel1() const", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.adcGainChannel1    Method of C++ class libm2k::M2kCalibration::adcGainChannel1"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(adcGainChannel1Definition, "RetVal", "double");
validate(adcGainChannel1Definition);

%% C++ class method |dacAvlsb| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: double libm2k::M2kCalibration::dacAvlsb() const
dacAvlsbDefinition = addMethod(M2kCalibrationDefinition, ...
    "double libm2k::M2kCalibration::dacAvlsb() const", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.dacAvlsb    Method of C++ class libm2k::M2kCalibration::dacAvlsb"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(dacAvlsbDefinition, "RetVal", "double");
validate(dacAvlsbDefinition);

%% C++ class method |dacBvlsb| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: double libm2k::M2kCalibration::dacBvlsb() const
dacBvlsbDefinition = addMethod(M2kCalibrationDefinition, ...
    "double libm2k::M2kCalibration::dacBvlsb() const", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.dacBvlsb    Method of C++ class libm2k::M2kCalibration::dacBvlsb"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(dacBvlsbDefinition, "RetVal", "double");
validate(dacBvlsbDefinition);

%% C++ class method |resetCalibration| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: bool libm2k::M2kCalibration::resetCalibration()
resetCalibrationDefinition = addMethod(M2kCalibrationDefinition, ...
    "bool libm2k::M2kCalibration::resetCalibration()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.resetCalibration    Method of C++ class libm2k::M2kCalibration::resetCalibration"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(resetCalibrationDefinition, "RetVal", "logical");
validate(resetCalibrationDefinition);

%% C++ class method |updateAdcCorrections| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: void libm2k::M2kCalibration::updateAdcCorrections()
updateAdcCorrectionsDefinition = addMethod(M2kCalibrationDefinition, ...
    "void libm2k::M2kCalibration::updateAdcCorrections()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.updateAdcCorrections    Method of C++ class libm2k::M2kCalibration::updateAdcCorrections"); % This description is shown as help to user. Modify it to appropriate description.
validate(updateAdcCorrectionsDefinition);

%% C++ class method |updateDacCorrections| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: void libm2k::M2kCalibration::updateDacCorrections()
updateDacCorrectionsDefinition = addMethod(M2kCalibrationDefinition, ...
    "void libm2k::M2kCalibration::updateDacCorrections()", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.updateDacCorrections    Method of C++ class libm2k::M2kCalibration::updateDacCorrections"); % This description is shown as help to user. Modify it to appropriate description.
validate(updateDacCorrectionsDefinition);

%% C++ class method |setCalibrationMode| for C++ class |libm2k::M2kCalibration| 
% C++ Signature: bool libm2k::M2kCalibration::setCalibrationMode(int input1)
setCalibrationModeDefinition = addMethod(M2kCalibrationDefinition, ...
    "bool libm2k::M2kCalibration::setCalibrationMode(int input1)", ...
    "Description", "clib.libm2k.libm2k.M2kCalibration.setCalibrationMode    Method of C++ class libm2k::M2kCalibration::setCalibrationMode"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setCalibrationModeDefinition, "input1", "int32");
defineOutput(setCalibrationModeDefinition, "RetVal", "logical");
validate(setCalibrationModeDefinition);

%% C++ class |libm2k::Logger| with MATLAB name |clib.libm2k.libm2k.Logger| 
LoggerDefinition = addClass(libDef, "libm2k::Logger", "MATLABName", "clib.libm2k.libm2k.Logger", ...
    "Description", "clib.libm2k.libm2k.Logger    Representation of C++ class libm2k::Logger"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class method |getInstance| for C++ class |libm2k::Logger| 
% C++ Signature: libm2k::Logger & libm2k::Logger::getInstance()
getInstanceDefinition = addMethod(LoggerDefinition, ...
    "libm2k::Logger & libm2k::Logger::getInstance()", ...
    "Description", "clib.libm2k.libm2k.Logger.getInstance    Method of C++ class libm2k::Logger::getInstance"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getInstanceDefinition, "RetVal", "clib.libm2k.libm2k.Logger");
validate(getInstanceDefinition);

%% C++ class method |warn| for C++ class |libm2k::Logger| 
% C++ Signature: void libm2k::Logger::warn(std::string message)
warnDefinition = addMethod(LoggerDefinition, ...
    "void libm2k::Logger::warn(std::string message)", ...
    "Description", "clib.libm2k.libm2k.Logger.warn    Method of C++ class libm2k::Logger::warn"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(warnDefinition, "message", "string");
validate(warnDefinition);

%% C++ class constructor for C++ class |libm2k::Logger| 
% C++ Signature: libm2k::Logger::Logger(libm2k::Logger const & input1)
LoggerConstructor1Definition = addConstructor(LoggerDefinition, ...
    "libm2k::Logger::Logger(libm2k::Logger const & input1)", ...
    "Description", "clib.libm2k.libm2k.Logger    Constructor of C++ class libm2k::Logger"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(LoggerConstructor1Definition, "input1", "clib.libm2k.libm2k.Logger", "input");
validate(LoggerConstructor1Definition);

%% C++ enumeration |libm2k::analog::M2K_TRIGGER_CONDITION| with MATLAB name |clib.libm2k.libm2k.analog.M2K_TRIGGER_CONDITION| 
addEnumeration(libDef, "libm2k::analog::M2K_TRIGGER_CONDITION", "int32",...
    [...
      "RISING_EDGE",...  % 0
      "FALLING_EDGE",...  % 1
      "LOW_LEVEL",...  % 2
      "HIGH_LEVEL",...  % 3
      "ANY_EDGE",...  % 4
      "NO_TRIGGER",...  % 5
    ],...
    "MATLABName", "clib.libm2k.libm2k.analog.M2K_TRIGGER_CONDITION", ...
    "Description", "clib.libm2k.libm2k.analog.M2K_TRIGGER_CONDITION    Representation of C++ enumeration libm2k::analog::M2K_TRIGGER_CONDITION"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |libm2k::analog::M2K_TRIGGER_MODE| with MATLAB name |clib.libm2k.libm2k.analog.M2K_TRIGGER_MODE| 
addEnumeration(libDef, "libm2k::analog::M2K_TRIGGER_MODE", "int32",...
    [...
      "ALWAYS",...  % 0
      "ANALOG",...  % 1
      "DIGITAL",...  % 2
      "DIGITAL_OR_ANALOG",...  % 3
      "DIGITAL_AND_ANALOG",...  % 4
      "DIGITAL_XOR_ANALOG",...  % 5
      "N_DIGITAL_OR_ANALOG",...  % 6
      "N_DIGITAL_AND_ANALOG",...  % 7
      "N_DIGITAL_XOR_ANALOG",...  % 8
    ],...
    "MATLABName", "clib.libm2k.libm2k.analog.M2K_TRIGGER_MODE", ...
    "Description", "clib.libm2k.libm2k.analog.M2K_TRIGGER_MODE    Representation of C++ enumeration libm2k::analog::M2K_TRIGGER_MODE"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |libm2k::analog::DMM| with MATLAB name |clib.libm2k.libm2k.analog.DMM| 
DMMDefinition = addClass(libDef, "libm2k::analog::DMM", "MATLABName", "clib.libm2k.libm2k.analog.DMM", ...
    "Description", "clib.libm2k.libm2k.analog.DMM    Representation of C++ class libm2k::analog::DMM"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |libm2k::analog::ANALOG_IN_CHANNEL| with MATLAB name |clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL| 
addEnumeration(libDef, "libm2k::analog::ANALOG_IN_CHANNEL", "int32",...
    [...
      "ANALOG_IN_CHANNEL_1",...  % 0
      "ANALOG_IN_CHANNEL_2",...  % 1
    ],...
    "MATLABName", "clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL", ...
    "Description", "clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL    Representation of C++ enumeration libm2k::analog::ANALOG_IN_CHANNEL"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |libm2k::analog::M2K_RANGE| with MATLAB name |clib.libm2k.libm2k.analog.M2K_RANGE| 
addEnumeration(libDef, "libm2k::analog::M2K_RANGE", "int32",...
    [...
      "PLUS_MINUS_25V",...  % 0
      "PLUS_MINUS_2_5V",...  % 1
    ],...
    "MATLABName", "clib.libm2k.libm2k.analog.M2K_RANGE", ...
    "Description", "clib.libm2k.libm2k.analog.M2K_RANGE    Representation of C++ enumeration libm2k::analog::M2K_RANGE"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |libm2k::analog::M2K_TRIGGER_SOURCE| with MATLAB name |clib.libm2k.libm2k.analog.M2K_TRIGGER_SOURCE| 
addEnumeration(libDef, "libm2k::analog::M2K_TRIGGER_SOURCE", "int32",...
    [...
      "CHANNEL_1",...  % 0
      "CHANNEL_2",...  % 1
      "CHANNEL_1_OR_CHANNEL_2",...  % 2
      "CHANNEL_1_AND_CHANNEL_2",...  % 3
      "CHANNEL_1_XOR_CHANNEL_2",...  % 4
    ],...
    "MATLABName", "clib.libm2k.libm2k.analog.M2K_TRIGGER_SOURCE", ...
    "Description", "clib.libm2k.libm2k.analog.M2K_TRIGGER_SOURCE    Representation of C++ enumeration libm2k::analog::M2K_TRIGGER_SOURCE"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |libm2k::analog::SETTINGS| with MATLAB name |clib.libm2k.libm2k.analog.SETTINGS| 
SETTINGSDefinition = addClass(libDef, "libm2k::analog::SETTINGS", "MATLABName", "clib.libm2k.libm2k.analog.SETTINGS", ...
    "Description", "clib.libm2k.libm2k.analog.SETTINGS    Representation of C++ class libm2k::analog::SETTINGS"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::analog::SETTINGS| 
% C++ Signature: libm2k::analog::SETTINGS::SETTINGS()
SETTINGSConstructor1Definition = addConstructor(SETTINGSDefinition, ...
    "libm2k::analog::SETTINGS::SETTINGS()", ...
    "Description", "clib.libm2k.libm2k.analog.SETTINGS    Constructor of C++ class libm2k::analog::SETTINGS"); % This description is shown as help to user. Modify it to appropriate description.
validate(SETTINGSConstructor1Definition);

%% C++ class constructor for C++ class |libm2k::analog::SETTINGS| 
% C++ Signature: libm2k::analog::SETTINGS::SETTINGS(libm2k::analog::SETTINGS const & input1)
SETTINGSConstructor2Definition = addConstructor(SETTINGSDefinition, ...
    "libm2k::analog::SETTINGS::SETTINGS(libm2k::analog::SETTINGS const & input1)", ...
    "Description", "clib.libm2k.libm2k.analog.SETTINGS    Constructor of C++ class libm2k::analog::SETTINGS"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(SETTINGSConstructor2Definition, "input1", "clib.libm2k.libm2k.analog.SETTINGS", "input");
validate(SETTINGSConstructor2Definition);

%% C++ class public data member |trigger_source| for C++ class |libm2k::analog::SETTINGS| 
% C++ Signature: libm2k::analog::M2K_TRIGGER_SOURCE libm2k::analog::SETTINGS::trigger_source
addProperty(SETTINGSDefinition, "trigger_source", "clib.libm2k.libm2k.analog.M2K_TRIGGER_SOURCE", ...
    "Description", "clib.libm2k.libm2k.analog.M2K_TRIGGER_SOURCE    Data Member of C++ class libm2k::analog::SETTINGS::trigger_source"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class public data member |delay| for C++ class |libm2k::analog::SETTINGS| 
% C++ Signature: int libm2k::analog::SETTINGS::delay
addProperty(SETTINGSDefinition, "delay", "int32", ...
    "Description", "int32    Data Member of C++ class libm2k::analog::SETTINGS::delay"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |libm2k::analog::M2kHardwareTrigger| with MATLAB name |clib.libm2k.libm2k.analog.M2kHardwareTrigger| 
M2kHardwareTriggerDefinition = addClass(libDef, "libm2k::analog::M2kHardwareTrigger", "MATLABName", "clib.libm2k.libm2k.analog.M2kHardwareTrigger", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger    Representation of C++ class libm2k::analog::M2kHardwareTrigger"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: libm2k::analog::M2kHardwareTrigger::M2kHardwareTrigger(iio_context * ctx)
%M2kHardwareTriggerConstructor1Definition = addConstructor(M2kHardwareTriggerDefinition, ...
%    "libm2k::analog::M2kHardwareTrigger::M2kHardwareTrigger(iio_context * ctx)", ...
%    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger    Constructor of C++ class libm2k::analog::M2kHardwareTrigger"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(M2kHardwareTriggerConstructor1Definition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%validate(M2kHardwareTriggerConstructor1Definition);

%% C++ class method |getAnalogLevelRaw| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: int libm2k::analog::M2kHardwareTrigger::getAnalogLevelRaw(unsigned int chnIdx)
getAnalogLevelRawDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "int libm2k::analog::M2kHardwareTrigger::getAnalogLevelRaw(unsigned int chnIdx)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getAnalogLevelRaw    Method of C++ class libm2k::analog::M2kHardwareTrigger::getAnalogLevelRaw"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getAnalogLevelRawDefinition, "chnIdx", "uint32");
defineOutput(getAnalogLevelRawDefinition, "RetVal", "int32");
validate(getAnalogLevelRawDefinition);

%% C++ class method |setAnalogLevelRaw| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setAnalogLevelRaw(unsigned int chnIdx,int level)
setAnalogLevelRawDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setAnalogLevelRaw(unsigned int chnIdx,int level)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setAnalogLevelRaw    Method of C++ class libm2k::analog::M2kHardwareTrigger::setAnalogLevelRaw"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setAnalogLevelRawDefinition, "chnIdx", "uint32");
defineArgument(setAnalogLevelRawDefinition, "level", "int32");
validate(setAnalogLevelRawDefinition);

%% C++ class method |setAnalogLevel| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setAnalogLevel(unsigned int chnIdx,double v_level)
setAnalogLevelDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setAnalogLevel(unsigned int chnIdx,double v_level)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setAnalogLevel    Method of C++ class libm2k::analog::M2kHardwareTrigger::setAnalogLevel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setAnalogLevelDefinition, "chnIdx", "uint32");
defineArgument(setAnalogLevelDefinition, "v_level", "double");
validate(setAnalogLevelDefinition);

%% C++ class method |getAnalogLevel| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: double libm2k::analog::M2kHardwareTrigger::getAnalogLevel(unsigned int chnIdx)
getAnalogLevelDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "double libm2k::analog::M2kHardwareTrigger::getAnalogLevel(unsigned int chnIdx)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getAnalogLevel    Method of C++ class libm2k::analog::M2kHardwareTrigger::getAnalogLevel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getAnalogLevelDefinition, "chnIdx", "uint32");
defineOutput(getAnalogLevelDefinition, "RetVal", "double");
validate(getAnalogLevelDefinition);

%% C++ class method |getAnalogHysteresis| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: int libm2k::analog::M2kHardwareTrigger::getAnalogHysteresis(unsigned int chnIdx)
getAnalogHysteresisDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "int libm2k::analog::M2kHardwareTrigger::getAnalogHysteresis(unsigned int chnIdx)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getAnalogHysteresis    Method of C++ class libm2k::analog::M2kHardwareTrigger::getAnalogHysteresis"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getAnalogHysteresisDefinition, "chnIdx", "uint32");
defineOutput(getAnalogHysteresisDefinition, "RetVal", "int32");
validate(getAnalogHysteresisDefinition);

%% C++ class method |setAnalogHysteresis| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setAnalogHysteresis(unsigned int chnIdx,int histeresis)
setAnalogHysteresisDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setAnalogHysteresis(unsigned int chnIdx,int histeresis)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setAnalogHysteresis    Method of C++ class libm2k::analog::M2kHardwareTrigger::setAnalogHysteresis"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setAnalogHysteresisDefinition, "chnIdx", "uint32");
defineArgument(setAnalogHysteresisDefinition, "histeresis", "int32");
validate(setAnalogHysteresisDefinition);

%% C++ class method |getAnalogCondition| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: libm2k::analog::M2K_TRIGGER_CONDITION libm2k::analog::M2kHardwareTrigger::getAnalogCondition(unsigned int chnIdx)
getAnalogConditionDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "libm2k::analog::M2K_TRIGGER_CONDITION libm2k::analog::M2kHardwareTrigger::getAnalogCondition(unsigned int chnIdx)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getAnalogCondition    Method of C++ class libm2k::analog::M2kHardwareTrigger::getAnalogCondition"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getAnalogConditionDefinition, "chnIdx", "uint32");
defineOutput(getAnalogConditionDefinition, "RetVal", "clib.libm2k.libm2k.analog.M2K_TRIGGER_CONDITION");
validate(getAnalogConditionDefinition);

%% C++ class method |setAnalogCondition| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setAnalogCondition(unsigned int chnIdx,libm2k::analog::M2K_TRIGGER_CONDITION cond)
setAnalogConditionDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setAnalogCondition(unsigned int chnIdx,libm2k::analog::M2K_TRIGGER_CONDITION cond)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setAnalogCondition    Method of C++ class libm2k::analog::M2kHardwareTrigger::setAnalogCondition"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setAnalogConditionDefinition, "chnIdx", "uint32");
defineArgument(setAnalogConditionDefinition, "cond", "clib.libm2k.libm2k.analog.M2K_TRIGGER_CONDITION");
validate(setAnalogConditionDefinition);

%% C++ class method |getExternalCondition| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: libm2k::analog::M2K_TRIGGER_CONDITION libm2k::analog::M2kHardwareTrigger::getExternalCondition(unsigned int chnIdx)
getExternalConditionDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "libm2k::analog::M2K_TRIGGER_CONDITION libm2k::analog::M2kHardwareTrigger::getExternalCondition(unsigned int chnIdx)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getExternalCondition    Method of C++ class libm2k::analog::M2kHardwareTrigger::getExternalCondition"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getExternalConditionDefinition, "chnIdx", "uint32");
defineOutput(getExternalConditionDefinition, "RetVal", "clib.libm2k.libm2k.analog.M2K_TRIGGER_CONDITION");
validate(getExternalConditionDefinition);

%% C++ class method |setExternalCondition| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setExternalCondition(unsigned int chnIdx,libm2k::analog::M2K_TRIGGER_CONDITION cond)
setExternalConditionDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setExternalCondition(unsigned int chnIdx,libm2k::analog::M2K_TRIGGER_CONDITION cond)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setExternalCondition    Method of C++ class libm2k::analog::M2kHardwareTrigger::setExternalCondition"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setExternalConditionDefinition, "chnIdx", "uint32");
defineArgument(setExternalConditionDefinition, "cond", "clib.libm2k.libm2k.analog.M2K_TRIGGER_CONDITION");
validate(setExternalConditionDefinition);

%% C++ class method |getDigitalCondition| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: libm2k::analog::M2K_TRIGGER_CONDITION libm2k::analog::M2kHardwareTrigger::getDigitalCondition(unsigned int chnIdx)
getDigitalConditionDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "libm2k::analog::M2K_TRIGGER_CONDITION libm2k::analog::M2kHardwareTrigger::getDigitalCondition(unsigned int chnIdx)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getDigitalCondition    Method of C++ class libm2k::analog::M2kHardwareTrigger::getDigitalCondition"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getDigitalConditionDefinition, "chnIdx", "uint32");
defineOutput(getDigitalConditionDefinition, "RetVal", "clib.libm2k.libm2k.analog.M2K_TRIGGER_CONDITION");
validate(getDigitalConditionDefinition);

%% C++ class method |setDigitalCondition| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setDigitalCondition(unsigned int chnIdx,libm2k::analog::M2K_TRIGGER_CONDITION cond)
setDigitalConditionDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setDigitalCondition(unsigned int chnIdx,libm2k::analog::M2K_TRIGGER_CONDITION cond)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setDigitalCondition    Method of C++ class libm2k::analog::M2kHardwareTrigger::setDigitalCondition"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setDigitalConditionDefinition, "chnIdx", "uint32");
defineArgument(setDigitalConditionDefinition, "cond", "clib.libm2k.libm2k.analog.M2K_TRIGGER_CONDITION");
validate(setDigitalConditionDefinition);

%% C++ class method |getAnalogMode| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: libm2k::analog::M2K_TRIGGER_MODE libm2k::analog::M2kHardwareTrigger::getAnalogMode(unsigned int chnIdx)
getAnalogModeDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "libm2k::analog::M2K_TRIGGER_MODE libm2k::analog::M2kHardwareTrigger::getAnalogMode(unsigned int chnIdx)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getAnalogMode    Method of C++ class libm2k::analog::M2kHardwareTrigger::getAnalogMode"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getAnalogModeDefinition, "chnIdx", "uint32");
defineOutput(getAnalogModeDefinition, "RetVal", "clib.libm2k.libm2k.analog.M2K_TRIGGER_MODE");
validate(getAnalogModeDefinition);

%% C++ class method |setAnalogMode| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setAnalogMode(unsigned int chnIdx,libm2k::analog::M2K_TRIGGER_MODE mode)
setAnalogModeDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setAnalogMode(unsigned int chnIdx,libm2k::analog::M2K_TRIGGER_MODE mode)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setAnalogMode    Method of C++ class libm2k::analog::M2kHardwareTrigger::setAnalogMode"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setAnalogModeDefinition, "chnIdx", "uint32");
defineArgument(setAnalogModeDefinition, "mode", "clib.libm2k.libm2k.analog.M2K_TRIGGER_MODE");
validate(setAnalogModeDefinition);

%% C++ class method |getDigitalMode| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: libm2k::digital::DIO_TRIGGER_MODE libm2k::analog::M2kHardwareTrigger::getDigitalMode()
getDigitalModeDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "libm2k::digital::DIO_TRIGGER_MODE libm2k::analog::M2kHardwareTrigger::getDigitalMode()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getDigitalMode    Method of C++ class libm2k::analog::M2kHardwareTrigger::getDigitalMode"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getDigitalModeDefinition, "RetVal", "clib.libm2k.libm2k.digital.DIO_TRIGGER_MODE");
validate(getDigitalModeDefinition);

%% C++ class method |setDigitalMode| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setDigitalMode(libm2k::digital::DIO_TRIGGER_MODE mode)
setDigitalModeDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setDigitalMode(libm2k::digital::DIO_TRIGGER_MODE mode)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setDigitalMode    Method of C++ class libm2k::analog::M2kHardwareTrigger::setDigitalMode"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setDigitalModeDefinition, "mode", "clib.libm2k.libm2k.digital.DIO_TRIGGER_MODE");
validate(setDigitalModeDefinition);

%% C++ class method |getAnalogSource| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: libm2k::analog::M2K_TRIGGER_SOURCE libm2k::analog::M2kHardwareTrigger::getAnalogSource()
getAnalogSourceDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "libm2k::analog::M2K_TRIGGER_SOURCE libm2k::analog::M2kHardwareTrigger::getAnalogSource()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getAnalogSource    Method of C++ class libm2k::analog::M2kHardwareTrigger::getAnalogSource"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getAnalogSourceDefinition, "RetVal", "clib.libm2k.libm2k.analog.M2K_TRIGGER_SOURCE");
validate(getAnalogSourceDefinition);

%% C++ class method |setAnalogSource| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setAnalogSource(libm2k::analog::M2K_TRIGGER_SOURCE src)
setAnalogSourceDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setAnalogSource(libm2k::analog::M2K_TRIGGER_SOURCE src)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setAnalogSource    Method of C++ class libm2k::analog::M2kHardwareTrigger::setAnalogSource"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setAnalogSourceDefinition, "src", "clib.libm2k.libm2k.analog.M2K_TRIGGER_SOURCE");
validate(setAnalogSourceDefinition);

%% C++ class method |getAnalogSourceChannel| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: int libm2k::analog::M2kHardwareTrigger::getAnalogSourceChannel()
getAnalogSourceChannelDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "int libm2k::analog::M2kHardwareTrigger::getAnalogSourceChannel()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getAnalogSourceChannel    Method of C++ class libm2k::analog::M2kHardwareTrigger::getAnalogSourceChannel"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getAnalogSourceChannelDefinition, "RetVal", "int32");
validate(getAnalogSourceChannelDefinition);

%% C++ class method |setAnalogSourceChannel| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setAnalogSourceChannel(unsigned int chnIdx)
setAnalogSourceChannelDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setAnalogSourceChannel(unsigned int chnIdx)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setAnalogSourceChannel    Method of C++ class libm2k::analog::M2kHardwareTrigger::setAnalogSourceChannel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setAnalogSourceChannelDefinition, "chnIdx", "uint32");
validate(setAnalogSourceChannelDefinition);

%% C++ class method |getAnalogDelay| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: int libm2k::analog::M2kHardwareTrigger::getAnalogDelay() const
getAnalogDelayDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "int libm2k::analog::M2kHardwareTrigger::getAnalogDelay() const", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getAnalogDelay    Method of C++ class libm2k::analog::M2kHardwareTrigger::getAnalogDelay"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getAnalogDelayDefinition, "RetVal", "int32");
validate(getAnalogDelayDefinition);

%% C++ class method |setAnalogDelay| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setAnalogDelay(int delay)
setAnalogDelayDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setAnalogDelay(int delay)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setAnalogDelay    Method of C++ class libm2k::analog::M2kHardwareTrigger::setAnalogDelay"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setAnalogDelayDefinition, "delay", "int32");
validate(setAnalogDelayDefinition);

%% C++ class method |getDigitalDelay| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: int libm2k::analog::M2kHardwareTrigger::getDigitalDelay() const
getDigitalDelayDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "int libm2k::analog::M2kHardwareTrigger::getDigitalDelay() const", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getDigitalDelay    Method of C++ class libm2k::analog::M2kHardwareTrigger::getDigitalDelay"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getDigitalDelayDefinition, "RetVal", "int32");
validate(getDigitalDelayDefinition);

%% C++ class method |setDigitalDelay| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setDigitalDelay(int delay)
setDigitalDelayDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setDigitalDelay(int delay)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setDigitalDelay    Method of C++ class libm2k::analog::M2kHardwareTrigger::setDigitalDelay"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setDigitalDelayDefinition, "delay", "int32");
validate(setDigitalDelayDefinition);

%% C++ class method |getCurrentHwSettings| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: libm2k::analog::SETTINGS * libm2k::analog::M2kHardwareTrigger::getCurrentHwSettings()
%getCurrentHwSettingsDefinition = addMethod(M2kHardwareTriggerDefinition, ...
%    "libm2k::analog::SETTINGS * libm2k::analog::M2kHardwareTrigger::getCurrentHwSettings()", ...
%    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getCurrentHwSettings    Method of C++ class libm2k::analog::M2kHardwareTrigger::getCurrentHwSettings"); % This description is shown as help to user. Modify it to appropriate description.
%defineOutput(getCurrentHwSettingsDefinition, "RetVal", "clib.libm2k.libm2k.analog.SETTINGS", <SHAPE>);
%validate(getCurrentHwSettingsDefinition);

%% C++ class method |setHwTriggerSettings| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setHwTriggerSettings(libm2k::analog::SETTINGS * settings)
%setHwTriggerSettingsDefinition = addMethod(M2kHardwareTriggerDefinition, ...
%    "void libm2k::analog::M2kHardwareTrigger::setHwTriggerSettings(libm2k::analog::SETTINGS * settings)", ...
%    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setHwTriggerSettings    Method of C++ class libm2k::analog::M2kHardwareTrigger::setHwTriggerSettings"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(setHwTriggerSettingsDefinition, "settings", "clib.libm2k.libm2k.analog.SETTINGS", "input", <SHAPE>);
%validate(setHwTriggerSettingsDefinition);

%% C++ class method |setAnalogStreamingFlag| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setAnalogStreamingFlag(bool enable)
setAnalogStreamingFlagDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setAnalogStreamingFlag(bool enable)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setAnalogStreamingFlag    Method of C++ class libm2k::analog::M2kHardwareTrigger::setAnalogStreamingFlag"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setAnalogStreamingFlagDefinition, "enable", "logical");
validate(setAnalogStreamingFlagDefinition);

%% C++ class method |getAnalogStreamingFlag| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: bool libm2k::analog::M2kHardwareTrigger::getAnalogStreamingFlag()
getAnalogStreamingFlagDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "bool libm2k::analog::M2kHardwareTrigger::getAnalogStreamingFlag()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getAnalogStreamingFlag    Method of C++ class libm2k::analog::M2kHardwareTrigger::getAnalogStreamingFlag"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getAnalogStreamingFlagDefinition, "RetVal", "logical");
validate(getAnalogStreamingFlagDefinition);

%% C++ class method |setDigitalStreamingFlag| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setDigitalStreamingFlag(bool enable)
setDigitalStreamingFlagDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setDigitalStreamingFlag(bool enable)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setDigitalStreamingFlag    Method of C++ class libm2k::analog::M2kHardwareTrigger::setDigitalStreamingFlag"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setDigitalStreamingFlagDefinition, "enable", "logical");
validate(setDigitalStreamingFlagDefinition);

%% C++ class method |getDigitalStreamingFlag| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: bool libm2k::analog::M2kHardwareTrigger::getDigitalStreamingFlag()
getDigitalStreamingFlagDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "bool libm2k::analog::M2kHardwareTrigger::getDigitalStreamingFlag()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.getDigitalStreamingFlag    Method of C++ class libm2k::analog::M2kHardwareTrigger::getDigitalStreamingFlag"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getDigitalStreamingFlagDefinition, "RetVal", "logical");
validate(getDigitalStreamingFlagDefinition);

%% C++ class method |setCalibParameters| for C++ class |libm2k::analog::M2kHardwareTrigger| 
% C++ Signature: void libm2k::analog::M2kHardwareTrigger::setCalibParameters(unsigned int chnIdx,double scaling,double vert_offset)
setCalibParametersDefinition = addMethod(M2kHardwareTriggerDefinition, ...
    "void libm2k::analog::M2kHardwareTrigger::setCalibParameters(unsigned int chnIdx,double scaling,double vert_offset)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kHardwareTrigger.setCalibParameters    Method of C++ class libm2k::analog::M2kHardwareTrigger::setCalibParameters"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setCalibParametersDefinition, "chnIdx", "uint32");
defineArgument(setCalibParametersDefinition, "scaling", "double");
defineArgument(setCalibParametersDefinition, "vert_offset", "double");
validate(setCalibParametersDefinition);

%% C++ class |libm2k::analog::M2kAnalogIn| with MATLAB name |clib.libm2k.libm2k.analog.M2kAnalogIn| 
M2kAnalogInDefinition = addClass(libDef, "libm2k::analog::M2kAnalogIn", "MATLABName", "clib.libm2k.libm2k.analog.M2kAnalogIn", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn    Representation of C++ class libm2k::analog::M2kAnalogIn"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: libm2k::analog::M2kAnalogIn::M2kAnalogIn(iio_context * input1,std::string adc_dev,bool sync)
%M2kAnalogInConstructor1Definition = addConstructor(M2kAnalogInDefinition, ...
%    "libm2k::analog::M2kAnalogIn::M2kAnalogIn(iio_context * input1,std::string adc_dev,bool sync)", ...
%    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn    Constructor of C++ class libm2k::analog::M2kAnalogIn"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(M2kAnalogInConstructor1Definition, "input1", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineArgument(M2kAnalogInConstructor1Definition, "adc_dev", "string");
%defineArgument(M2kAnalogInConstructor1Definition, "sync", "logical");
%validate(M2kAnalogInConstructor1Definition);

%% C++ class method |init| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: void libm2k::analog::M2kAnalogIn::init()
initDefinition = addMethod(M2kAnalogInDefinition, ...
    "void libm2k::analog::M2kAnalogIn::init()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.init    Method of C++ class libm2k::analog::M2kAnalogIn::init"); % This description is shown as help to user. Modify it to appropriate description.
validate(initDefinition);

%% C++ class method |getSamplesInterleaved| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double const * libm2k::analog::M2kAnalogIn::getSamplesInterleaved(unsigned int nb_samples)
getSamplesInterleavedDefinition = addMethod(M2kAnalogInDefinition, ...
   "double const * libm2k::analog::M2kAnalogIn::getSamplesInterleaved(unsigned int nb_samples)", ...
   "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getSamplesInterleaved    Method of C++ class libm2k::analog::M2kAnalogIn::getSamplesInterleaved"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getSamplesInterleavedDefinition, "nb_samples", "uint32");
defineOutput(getSamplesInterleavedDefinition, "RetVal", "double", "nb_samples");
validate(getSamplesInterleavedDefinition);

%% C++ class method |getSamplesRawInterleaved| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: short const * libm2k::analog::M2kAnalogIn::getSamplesRawInterleaved(unsigned int nb_samples)
getSamplesRawInterleavedDefinition = addMethod(M2kAnalogInDefinition, ...
   "short const * libm2k::analog::M2kAnalogIn::getSamplesRawInterleaved(unsigned int nb_samples)", ...
   "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getSamplesRawInterleaved    Method of C++ class libm2k::analog::M2kAnalogIn::getSamplesRawInterleaved"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getSamplesRawInterleavedDefinition, "nb_samples", "uint32");
defineOutput(getSamplesRawInterleavedDefinition, "RetVal", "int16", "nb_samples");
validate(getSamplesRawInterleavedDefinition);

%% C++ class method |processSample| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::processSample(int16_t sample,unsigned int channel)
processSampleDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::processSample(int16_t sample,unsigned int channel)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.processSample    Method of C++ class libm2k::analog::M2kAnalogIn::processSample"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(processSampleDefinition, "sample", "int16");
defineArgument(processSampleDefinition, "channel", "uint32");
defineOutput(processSampleDefinition, "RetVal", "double");
validate(processSampleDefinition);

%% C++ class method |getVoltageRaw| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: short libm2k::analog::M2kAnalogIn::getVoltageRaw(unsigned int ch)
getVoltageRawDefinition = addMethod(M2kAnalogInDefinition, ...
    "short libm2k::analog::M2kAnalogIn::getVoltageRaw(unsigned int ch)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getVoltageRaw    Method of C++ class libm2k::analog::M2kAnalogIn::getVoltageRaw"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getVoltageRawDefinition, "ch", "uint32");
defineOutput(getVoltageRawDefinition, "RetVal", "int16");
validate(getVoltageRawDefinition);

%% C++ class method |getVoltage| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::getVoltage(unsigned int ch)
getVoltageDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::getVoltage(unsigned int ch)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getVoltage    Method of C++ class libm2k::analog::M2kAnalogIn::getVoltage"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getVoltageDefinition, "ch", "uint32");
defineOutput(getVoltageDefinition, "RetVal", "double");
validate(getVoltageDefinition);

%% C++ class method |getVoltageRaw| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: short libm2k::analog::M2kAnalogIn::getVoltageRaw(libm2k::analog::ANALOG_IN_CHANNEL ch)
getVoltageRawDefinition = addMethod(M2kAnalogInDefinition, ...
    "short libm2k::analog::M2kAnalogIn::getVoltageRaw(libm2k::analog::ANALOG_IN_CHANNEL ch)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getVoltageRaw    Method of C++ class libm2k::analog::M2kAnalogIn::getVoltageRaw"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getVoltageRawDefinition, "ch", "clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL");
defineOutput(getVoltageRawDefinition, "RetVal", "int16");
validate(getVoltageRawDefinition);

%% C++ class method |getVoltage| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::getVoltage(libm2k::analog::ANALOG_IN_CHANNEL ch)
getVoltageDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::getVoltage(libm2k::analog::ANALOG_IN_CHANNEL ch)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getVoltage    Method of C++ class libm2k::analog::M2kAnalogIn::getVoltage"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getVoltageDefinition, "ch", "clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL");
defineOutput(getVoltageDefinition, "RetVal", "double");
validate(getVoltageDefinition);

%% C++ class method |getVoltageRawP| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: short const * libm2k::analog::M2kAnalogIn::getVoltageRawP()
%getVoltageRawPDefinition = addMethod(M2kAnalogInDefinition, ...
%    "short const * libm2k::analog::M2kAnalogIn::getVoltageRawP()", ...
%    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getVoltageRawP    Method of C++ class libm2k::analog::M2kAnalogIn::getVoltageRawP"); % This description is shown as help to user. Modify it to appropriate description.
%defineOutput(getVoltageRawPDefinition, "RetVal", "int16", <SHAPE>);
%validate(getVoltageRawPDefinition);

%% C++ class method |getVoltageP| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double const * libm2k::analog::M2kAnalogIn::getVoltageP()
%getVoltagePDefinition = addMethod(M2kAnalogInDefinition, ...
%    "double const * libm2k::analog::M2kAnalogIn::getVoltageP()", ...
%    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getVoltageP    Method of C++ class libm2k::analog::M2kAnalogIn::getVoltageP"); % This description is shown as help to user. Modify it to appropriate description.
%defineOutput(getVoltagePDefinition, "RetVal", "double", <SHAPE>);
%validate(getVoltagePDefinition);

%% C++ class method |setVerticalOffset| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: void libm2k::analog::M2kAnalogIn::setVerticalOffset(libm2k::analog::ANALOG_IN_CHANNEL channel,double vertOffset)
setVerticalOffsetDefinition = addMethod(M2kAnalogInDefinition, ...
    "void libm2k::analog::M2kAnalogIn::setVerticalOffset(libm2k::analog::ANALOG_IN_CHANNEL channel,double vertOffset)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.setVerticalOffset    Method of C++ class libm2k::analog::M2kAnalogIn::setVerticalOffset"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setVerticalOffsetDefinition, "channel", "clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL");
defineArgument(setVerticalOffsetDefinition, "vertOffset", "double");
validate(setVerticalOffsetDefinition);

%% C++ class method |getVerticalOffset| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::getVerticalOffset(libm2k::analog::ANALOG_IN_CHANNEL channel)
getVerticalOffsetDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::getVerticalOffset(libm2k::analog::ANALOG_IN_CHANNEL channel)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getVerticalOffset    Method of C++ class libm2k::analog::M2kAnalogIn::getVerticalOffset"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getVerticalOffsetDefinition, "channel", "clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL");
defineOutput(getVerticalOffsetDefinition, "RetVal", "double");
validate(getVerticalOffsetDefinition);

%% C++ class method |getScalingFactor| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::getScalingFactor(libm2k::analog::ANALOG_IN_CHANNEL ch)
getScalingFactorDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::getScalingFactor(libm2k::analog::ANALOG_IN_CHANNEL ch)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getScalingFactor    Method of C++ class libm2k::analog::M2kAnalogIn::getScalingFactor"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getScalingFactorDefinition, "ch", "clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL");
defineOutput(getScalingFactorDefinition, "RetVal", "double");
validate(getScalingFactorDefinition);

%% C++ class method |setRange| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: void libm2k::analog::M2kAnalogIn::setRange(libm2k::analog::ANALOG_IN_CHANNEL channel,libm2k::analog::M2K_RANGE range)
% setRangeDefinition = addMethod(M2kAnalogInDefinition, ...
%     "void libm2k::analog::M2kAnalogIn::setRange(libm2k::analog::ANALOG_IN_CHANNEL channel,libm2k::analog::M2K_RANGE range)", ...
%     "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.setRange    Method of C++ class libm2k::analog::M2kAnalogIn::setRange"); % This description is shown as help to user. Modify it to appropriate description.
% defineArgument(setRangeDefinition, "channel", "clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL");
% defineArgument(setRangeDefinition, "range", "clib.libm2k.libm2k.analog.M2K_RANGE");
% validate(setRangeDefinition);

%% C++ class method |setRange| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: void libm2k::analog::M2kAnalogIn::setRange(libm2k::analog::ANALOG_IN_CHANNEL channel,double min,double max)
setRangeDefinition = addMethod(M2kAnalogInDefinition, ...
    "void libm2k::analog::M2kAnalogIn::setRange(libm2k::analog::ANALOG_IN_CHANNEL channel,double min,double max)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.setRange    Method of C++ class libm2k::analog::M2kAnalogIn::setRange"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setRangeDefinition, "channel", "clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL");
defineArgument(setRangeDefinition, "min", "double");
defineArgument(setRangeDefinition, "max", "double");
validate(setRangeDefinition);

%% C++ class method |getRange| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: libm2k::analog::M2K_RANGE libm2k::analog::M2kAnalogIn::getRange(libm2k::analog::ANALOG_IN_CHANNEL channel)
getRangeDefinition = addMethod(M2kAnalogInDefinition, ...
    "libm2k::analog::M2K_RANGE libm2k::analog::M2kAnalogIn::getRange(libm2k::analog::ANALOG_IN_CHANNEL channel)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getRange    Method of C++ class libm2k::analog::M2kAnalogIn::getRange"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getRangeDefinition, "channel", "clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL");
defineOutput(getRangeDefinition, "RetVal", "clib.libm2k.libm2k.analog.M2K_RANGE");
validate(getRangeDefinition);

%% C++ class method |getOversamplingRatio| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::getOversamplingRatio()
getOversamplingRatioDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::getOversamplingRatio()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getOversamplingRatio    Method of C++ class libm2k::analog::M2kAnalogIn::getOversamplingRatio"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getOversamplingRatioDefinition, "RetVal", "double");
validate(getOversamplingRatioDefinition);

%% C++ class method |getOversamplingRatio| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::getOversamplingRatio(unsigned int chn_idx)
getOversamplingRatioDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::getOversamplingRatio(unsigned int chn_idx)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getOversamplingRatio    Method of C++ class libm2k::analog::M2kAnalogIn::getOversamplingRatio"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getOversamplingRatioDefinition, "chn_idx", "uint32");
defineOutput(getOversamplingRatioDefinition, "RetVal", "double");
validate(getOversamplingRatioDefinition);

%% C++ class method |setOversamplingRatio| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::setOversamplingRatio(double oversampling)
setOversamplingRatioDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::setOversamplingRatio(double oversampling)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.setOversamplingRatio    Method of C++ class libm2k::analog::M2kAnalogIn::setOversamplingRatio"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setOversamplingRatioDefinition, "oversampling", "double");
defineOutput(setOversamplingRatioDefinition, "RetVal", "double");
validate(setOversamplingRatioDefinition);

%% C++ class method |setOversamplingRatio| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::setOversamplingRatio(unsigned int chn_idx,double oversampling)
setOversamplingRatioDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::setOversamplingRatio(unsigned int chn_idx,double oversampling)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.setOversamplingRatio    Method of C++ class libm2k::analog::M2kAnalogIn::setOversamplingRatio"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setOversamplingRatioDefinition, "chn_idx", "uint32");
defineArgument(setOversamplingRatioDefinition, "oversampling", "double");
defineOutput(setOversamplingRatioDefinition, "RetVal", "double");
validate(setOversamplingRatioDefinition);

%% C++ class method |getSampleRate| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::getSampleRate()
getSampleRateDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::getSampleRate()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getSampleRate    Method of C++ class libm2k::analog::M2kAnalogIn::getSampleRate"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getSampleRateDefinition, "RetVal", "double");
validate(getSampleRateDefinition);

%% C++ class method |getSampleRate| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::getSampleRate(unsigned int chn_idx)
getSampleRateDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::getSampleRate(unsigned int chn_idx)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getSampleRate    Method of C++ class libm2k::analog::M2kAnalogIn::getSampleRate"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getSampleRateDefinition, "chn_idx", "uint32");
defineOutput(getSampleRateDefinition, "RetVal", "double");
validate(getSampleRateDefinition);

%% C++ class method |setSampleRate| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::setSampleRate(double samplerate)
setSampleRateDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::setSampleRate(double samplerate)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.setSampleRate    Method of C++ class libm2k::analog::M2kAnalogIn::setSampleRate"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setSampleRateDefinition, "samplerate", "double");
defineOutput(setSampleRateDefinition, "RetVal", "double");
validate(setSampleRateDefinition);

%% C++ class method |setSampleRate| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::setSampleRate(unsigned int chn_idx,double samplerate)
setSampleRateDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::setSampleRate(unsigned int chn_idx,double samplerate)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.setSampleRate    Method of C++ class libm2k::analog::M2kAnalogIn::setSampleRate"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setSampleRateDefinition, "chn_idx", "uint32");
defineArgument(setSampleRateDefinition, "samplerate", "double");
defineOutput(setSampleRateDefinition, "RetVal", "double");
validate(setSampleRateDefinition);

%% C++ class method |setAdcCalibOffset| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: void libm2k::analog::M2kAnalogIn::setAdcCalibOffset(libm2k::analog::ANALOG_IN_CHANNEL channel,int calib_offset)
setAdcCalibOffsetDefinition = addMethod(M2kAnalogInDefinition, ...
    "void libm2k::analog::M2kAnalogIn::setAdcCalibOffset(libm2k::analog::ANALOG_IN_CHANNEL channel,int calib_offset)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.setAdcCalibOffset    Method of C++ class libm2k::analog::M2kAnalogIn::setAdcCalibOffset"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setAdcCalibOffsetDefinition, "channel", "clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL");
defineArgument(setAdcCalibOffsetDefinition, "calib_offset", "int32");
validate(setAdcCalibOffsetDefinition);

%% C++ class method |setCalibscale| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::setCalibscale(unsigned int index,double calibscale)
setCalibscaleDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::setCalibscale(unsigned int index,double calibscale)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.setCalibscale    Method of C++ class libm2k::analog::M2kAnalogIn::setCalibscale"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setCalibscaleDefinition, "index", "uint32");
defineArgument(setCalibscaleDefinition, "calibscale", "double");
defineOutput(setCalibscaleDefinition, "RetVal", "double");
validate(setCalibscaleDefinition);

%% C++ class method |getCalibscale| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::getCalibscale(unsigned int index)
getCalibscaleDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::getCalibscale(unsigned int index)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getCalibscale    Method of C++ class libm2k::analog::M2kAnalogIn::getCalibscale"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getCalibscaleDefinition, "index", "uint32");
defineOutput(getCalibscaleDefinition, "RetVal", "double");
validate(getCalibscaleDefinition);

%% C++ class method |setAdcCalibGain| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: void libm2k::analog::M2kAnalogIn::setAdcCalibGain(libm2k::analog::ANALOG_IN_CHANNEL channel,double gain)
setAdcCalibGainDefinition = addMethod(M2kAnalogInDefinition, ...
    "void libm2k::analog::M2kAnalogIn::setAdcCalibGain(libm2k::analog::ANALOG_IN_CHANNEL channel,double gain)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.setAdcCalibGain    Method of C++ class libm2k::analog::M2kAnalogIn::setAdcCalibGain"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setAdcCalibGainDefinition, "channel", "clib.libm2k.libm2k.analog.ANALOG_IN_CHANNEL");
defineArgument(setAdcCalibGainDefinition, "gain", "double");
validate(setAdcCalibGainDefinition);

%% C++ class method |getFilterCompensation| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::getFilterCompensation(double samplerate)
getFilterCompensationDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::getFilterCompensation(double samplerate)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getFilterCompensation    Method of C++ class libm2k::analog::M2kAnalogIn::getFilterCompensation"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getFilterCompensationDefinition, "samplerate", "double");
defineOutput(getFilterCompensationDefinition, "RetVal", "double");
validate(getFilterCompensationDefinition);

%% C++ class method |getValueForRange| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::getValueForRange(libm2k::analog::M2K_RANGE range)
getValueForRangeDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::getValueForRange(libm2k::analog::M2K_RANGE range)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getValueForRange    Method of C++ class libm2k::analog::M2kAnalogIn::getValueForRange"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getValueForRangeDefinition, "range", "clib.libm2k.libm2k.analog.M2K_RANGE");
defineOutput(getValueForRangeDefinition, "RetVal", "double");
validate(getValueForRangeDefinition);

%% C++ class method |convertRawToVolts| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: double libm2k::analog::M2kAnalogIn::convertRawToVolts(int sample,double correctionGain,double hw_gain,double filterCompensation,double offset)
convertRawToVoltsDefinition = addMethod(M2kAnalogInDefinition, ...
    "double libm2k::analog::M2kAnalogIn::convertRawToVolts(int sample,double correctionGain,double hw_gain,double filterCompensation,double offset)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.convertRawToVolts    Method of C++ class libm2k::analog::M2kAnalogIn::convertRawToVolts"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(convertRawToVoltsDefinition, "sample", "int32");
defineArgument(convertRawToVoltsDefinition, "correctionGain", "double");
defineArgument(convertRawToVoltsDefinition, "hw_gain", "double");
defineArgument(convertRawToVoltsDefinition, "filterCompensation", "double");
defineArgument(convertRawToVoltsDefinition, "offset", "double");
defineOutput(convertRawToVoltsDefinition, "RetVal", "double");
validate(convertRawToVoltsDefinition);

%% C++ class method |convertVoltsToRaw| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: int libm2k::analog::M2kAnalogIn::convertVoltsToRaw(double voltage,double correctionGain,double hw_gain,double filterCompensation,double offset)
convertVoltsToRawDefinition = addMethod(M2kAnalogInDefinition, ...
    "int libm2k::analog::M2kAnalogIn::convertVoltsToRaw(double voltage,double correctionGain,double hw_gain,double filterCompensation,double offset)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.convertVoltsToRaw    Method of C++ class libm2k::analog::M2kAnalogIn::convertVoltsToRaw"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(convertVoltsToRawDefinition, "voltage", "double");
defineArgument(convertVoltsToRawDefinition, "correctionGain", "double");
defineArgument(convertVoltsToRawDefinition, "hw_gain", "double");
defineArgument(convertVoltsToRawDefinition, "filterCompensation", "double");
defineArgument(convertVoltsToRawDefinition, "offset", "double");
defineOutput(convertVoltsToRawDefinition, "RetVal", "int32");
validate(convertVoltsToRawDefinition);

%% C++ class method |getNbChannels| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: unsigned int libm2k::analog::M2kAnalogIn::getNbChannels()
getNbChannelsDefinition = addMethod(M2kAnalogInDefinition, ...
    "unsigned int libm2k::analog::M2kAnalogIn::getNbChannels()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getNbChannels    Method of C++ class libm2k::analog::M2kAnalogIn::getNbChannels"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getNbChannelsDefinition, "RetVal", "uint32");
validate(getNbChannelsDefinition);

%% C++ class method |getName| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: std::string libm2k::analog::M2kAnalogIn::getName()
getNameDefinition = addMethod(M2kAnalogInDefinition, ...
    "std::string libm2k::analog::M2kAnalogIn::getName()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getName    Method of C++ class libm2k::analog::M2kAnalogIn::getName"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getNameDefinition, "RetVal", "string");
validate(getNameDefinition);

%% C++ class method |enableChannel| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: void libm2k::analog::M2kAnalogIn::enableChannel(unsigned int chnIdx,bool enable)
enableChannelDefinition = addMethod(M2kAnalogInDefinition, ...
    "void libm2k::analog::M2kAnalogIn::enableChannel(unsigned int chnIdx,bool enable)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.enableChannel    Method of C++ class libm2k::analog::M2kAnalogIn::enableChannel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(enableChannelDefinition, "chnIdx", "uint32");
defineArgument(enableChannelDefinition, "enable", "logical");
validate(enableChannelDefinition);

%% C++ class method |isChannelEnabled| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: bool libm2k::analog::M2kAnalogIn::isChannelEnabled(unsigned int chnIdx)
isChannelEnabledDefinition = addMethod(M2kAnalogInDefinition, ...
    "bool libm2k::analog::M2kAnalogIn::isChannelEnabled(unsigned int chnIdx)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.isChannelEnabled    Method of C++ class libm2k::analog::M2kAnalogIn::isChannelEnabled"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(isChannelEnabledDefinition, "chnIdx", "uint32");
defineOutput(isChannelEnabledDefinition, "RetVal", "logical");
validate(isChannelEnabledDefinition);

%% C++ class method |convertChannelHostFormat| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: void libm2k::analog::M2kAnalogIn::convertChannelHostFormat(unsigned int chn_idx,int16_t * avg,int16_t * src)
%convertChannelHostFormatDefinition = addMethod(M2kAnalogInDefinition, ...
%    "void libm2k::analog::M2kAnalogIn::convertChannelHostFormat(unsigned int chn_idx,int16_t * avg,int16_t * src)", ...
%    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.convertChannelHostFormat    Method of C++ class libm2k::analog::M2kAnalogIn::convertChannelHostFormat"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(convertChannelHostFormatDefinition, "chn_idx", "uint32");
%defineArgument(convertChannelHostFormatDefinition, "avg", "int16", <DIRECTION>, <SHAPE>);
%defineArgument(convertChannelHostFormatDefinition, "src", "int16", <DIRECTION>, <SHAPE>);
%validate(convertChannelHostFormatDefinition);

%% C++ class method |convertChannelHostFormat| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: void libm2k::analog::M2kAnalogIn::convertChannelHostFormat(unsigned int chn_idx,double * avg,int16_t * src)
%convertChannelHostFormatDefinition = addMethod(M2kAnalogInDefinition, ...
%    "void libm2k::analog::M2kAnalogIn::convertChannelHostFormat(unsigned int chn_idx,double * avg,int16_t * src)", ...
%    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.convertChannelHostFormat    Method of C++ class libm2k::analog::M2kAnalogIn::convertChannelHostFormat"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(convertChannelHostFormatDefinition, "chn_idx", "uint32");
%defineArgument(convertChannelHostFormatDefinition, "avg", "double", <DIRECTION>, <SHAPE>);
%defineArgument(convertChannelHostFormatDefinition, "src", "int16", <DIRECTION>, <SHAPE>);
%validate(convertChannelHostFormatDefinition);

%% C++ class method |setKernelBuffersCount| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: void libm2k::analog::M2kAnalogIn::setKernelBuffersCount(unsigned int count)
setKernelBuffersCountDefinition = addMethod(M2kAnalogInDefinition, ...
    "void libm2k::analog::M2kAnalogIn::setKernelBuffersCount(unsigned int count)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.setKernelBuffersCount    Method of C++ class libm2k::analog::M2kAnalogIn::setKernelBuffersCount"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setKernelBuffersCountDefinition, "count", "uint32");
validate(setKernelBuffersCountDefinition);

%% C++ class method |getTrigger| for C++ class |libm2k::analog::M2kAnalogIn| 
% C++ Signature: libm2k::analog::M2kHardwareTrigger * libm2k::analog::M2kAnalogIn::getTrigger()
getTriggerDefinition = addMethod(M2kAnalogInDefinition, ...
   "libm2k::analog::M2kHardwareTrigger * libm2k::analog::M2kAnalogIn::getTrigger()", ...
   "Description", "clib.libm2k.libm2k.analog.M2kAnalogIn.getTrigger    Method of C++ class libm2k::analog::M2kAnalogIn::getTrigger"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getTriggerDefinition, "RetVal", "clib.libm2k.libm2k.analog.M2kHardwareTrigger", 1);
validate(getTriggerDefinition);

%% C++ class |libm2k::analog::M2kAnalogOut| with MATLAB name |clib.libm2k.libm2k.analog.M2kAnalogOut| 
M2kAnalogOutDefinition = addClass(libDef, "libm2k::analog::M2kAnalogOut", "MATLABName", "clib.libm2k.libm2k.analog.M2kAnalogOut", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut    Representation of C++ class libm2k::analog::M2kAnalogOut"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class method |init| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: void libm2k::analog::M2kAnalogOut::init()
initDefinition = addMethod(M2kAnalogOutDefinition, ...
    "void libm2k::analog::M2kAnalogOut::init()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.init    Method of C++ class libm2k::analog::M2kAnalogOut::init"); % This description is shown as help to user. Modify it to appropriate description.
validate(initDefinition);

%% C++ class method |getOversamplingRatio| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: double libm2k::analog::M2kAnalogOut::getOversamplingRatio(unsigned int chn)
getOversamplingRatioDefinition = addMethod(M2kAnalogOutDefinition, ...
    "double libm2k::analog::M2kAnalogOut::getOversamplingRatio(unsigned int chn)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.getOversamplingRatio    Method of C++ class libm2k::analog::M2kAnalogOut::getOversamplingRatio"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getOversamplingRatioDefinition, "chn", "uint32");
defineOutput(getOversamplingRatioDefinition, "RetVal", "double");
validate(getOversamplingRatioDefinition);

%% C++ class method |setOversamplingRatio| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: double libm2k::analog::M2kAnalogOut::setOversamplingRatio(unsigned int chn,double oversampling_ratio)
setOversamplingRatioDefinition = addMethod(M2kAnalogOutDefinition, ...
    "double libm2k::analog::M2kAnalogOut::setOversamplingRatio(unsigned int chn,double oversampling_ratio)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.setOversamplingRatio    Method of C++ class libm2k::analog::M2kAnalogOut::setOversamplingRatio"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setOversamplingRatioDefinition, "chn", "uint32");
defineArgument(setOversamplingRatioDefinition, "oversampling_ratio", "double");
defineOutput(setOversamplingRatioDefinition, "RetVal", "double");
validate(setOversamplingRatioDefinition);

%% C++ class method |getSampleRate| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: double libm2k::analog::M2kAnalogOut::getSampleRate(unsigned int chn)
getSampleRateDefinition = addMethod(M2kAnalogOutDefinition, ...
    "double libm2k::analog::M2kAnalogOut::getSampleRate(unsigned int chn)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.getSampleRate    Method of C++ class libm2k::analog::M2kAnalogOut::getSampleRate"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getSampleRateDefinition, "chn", "uint32");
defineOutput(getSampleRateDefinition, "RetVal", "double");
validate(getSampleRateDefinition);

%% C++ class method |setSampleRate| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: double libm2k::analog::M2kAnalogOut::setSampleRate(unsigned int chn,double samplerate)
setSampleRateDefinition = addMethod(M2kAnalogOutDefinition, ...
    "double libm2k::analog::M2kAnalogOut::setSampleRate(unsigned int chn,double samplerate)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.setSampleRate    Method of C++ class libm2k::analog::M2kAnalogOut::setSampleRate"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setSampleRateDefinition, "chn", "uint32");
defineArgument(setSampleRateDefinition, "samplerate", "double");
defineOutput(setSampleRateDefinition, "RetVal", "double");
validate(setSampleRateDefinition);

%% C++ class method |setSyncedDma| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: void libm2k::analog::M2kAnalogOut::setSyncedDma(bool en,int chn)
setSyncedDmaDefinition = addMethod(M2kAnalogOutDefinition, ...
    "void libm2k::analog::M2kAnalogOut::setSyncedDma(bool en,int chn)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.setSyncedDma    Method of C++ class libm2k::analog::M2kAnalogOut::setSyncedDma"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setSyncedDmaDefinition, "en", "logical");
defineArgument(setSyncedDmaDefinition, "chn", "int32");
validate(setSyncedDmaDefinition);

%% C++ class method |getSyncedDma| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: bool libm2k::analog::M2kAnalogOut::getSyncedDma(int chn)
getSyncedDmaDefinition = addMethod(M2kAnalogOutDefinition, ...
    "bool libm2k::analog::M2kAnalogOut::getSyncedDma(int chn)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.getSyncedDma    Method of C++ class libm2k::analog::M2kAnalogOut::getSyncedDma"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getSyncedDmaDefinition, "chn", "int32");
defineOutput(getSyncedDmaDefinition, "RetVal", "logical");
validate(getSyncedDmaDefinition);

%% C++ class method |setCyclic| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: void libm2k::analog::M2kAnalogOut::setCyclic(bool en)
setCyclicDefinition = addMethod(M2kAnalogOutDefinition, ...
    "void libm2k::analog::M2kAnalogOut::setCyclic(bool en)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.setCyclic    Method of C++ class libm2k::analog::M2kAnalogOut::setCyclic"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setCyclicDefinition, "en", "logical");
validate(setCyclicDefinition);

%% C++ class method |setCyclic| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: void libm2k::analog::M2kAnalogOut::setCyclic(unsigned int chn,bool en)
setCyclicDefinition = addMethod(M2kAnalogOutDefinition, ...
    "void libm2k::analog::M2kAnalogOut::setCyclic(unsigned int chn,bool en)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.setCyclic    Method of C++ class libm2k::analog::M2kAnalogOut::setCyclic"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setCyclicDefinition, "chn", "uint32");
defineArgument(setCyclicDefinition, "en", "logical");
validate(setCyclicDefinition);

%% C++ class method |getCyclic| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: bool libm2k::analog::M2kAnalogOut::getCyclic(unsigned int chn)
getCyclicDefinition = addMethod(M2kAnalogOutDefinition, ...
    "bool libm2k::analog::M2kAnalogOut::getCyclic(unsigned int chn)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.getCyclic    Method of C++ class libm2k::analog::M2kAnalogOut::getCyclic"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getCyclicDefinition, "chn", "uint32");
defineOutput(getCyclicDefinition, "RetVal", "logical");
validate(getCyclicDefinition);

%% C++ class method |setCalibscale| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: double libm2k::analog::M2kAnalogOut::setCalibscale(unsigned int index,double calibscale)
setCalibscaleDefinition = addMethod(M2kAnalogOutDefinition, ...
    "double libm2k::analog::M2kAnalogOut::setCalibscale(unsigned int index,double calibscale)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.setCalibscale    Method of C++ class libm2k::analog::M2kAnalogOut::setCalibscale"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setCalibscaleDefinition, "index", "uint32");
defineArgument(setCalibscaleDefinition, "calibscale", "double");
defineOutput(setCalibscaleDefinition, "RetVal", "double");
validate(setCalibscaleDefinition);

%% C++ class method |getCalibscale| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: double libm2k::analog::M2kAnalogOut::getCalibscale(unsigned int index)
getCalibscaleDefinition = addMethod(M2kAnalogOutDefinition, ...
    "double libm2k::analog::M2kAnalogOut::getCalibscale(unsigned int index)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.getCalibscale    Method of C++ class libm2k::analog::M2kAnalogOut::getCalibscale"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getCalibscaleDefinition, "index", "uint32");
defineOutput(getCalibscaleDefinition, "RetVal", "double");
validate(getCalibscaleDefinition);

%% C++ class method |getScalingFactor| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: double libm2k::analog::M2kAnalogOut::getScalingFactor(unsigned int chn)
getScalingFactorDefinition = addMethod(M2kAnalogOutDefinition, ...
    "double libm2k::analog::M2kAnalogOut::getScalingFactor(unsigned int chn)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.getScalingFactor    Method of C++ class libm2k::analog::M2kAnalogOut::getScalingFactor"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getScalingFactorDefinition, "chn", "uint32");
defineOutput(getScalingFactorDefinition, "RetVal", "double");
validate(getScalingFactorDefinition);

%% C++ class method |getFilterCompensation| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: double libm2k::analog::M2kAnalogOut::getFilterCompensation(double samplerate)
getFilterCompensationDefinition = addMethod(M2kAnalogOutDefinition, ...
    "double libm2k::analog::M2kAnalogOut::getFilterCompensation(double samplerate)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.getFilterCompensation    Method of C++ class libm2k::analog::M2kAnalogOut::getFilterCompensation"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getFilterCompensationDefinition, "samplerate", "double");
defineOutput(getFilterCompensationDefinition, "RetVal", "double");
validate(getFilterCompensationDefinition);

%% C++ class method |convertVoltsToRaw| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: int libm2k::analog::M2kAnalogOut::convertVoltsToRaw(double voltage,double vlsb,double filterCompensation)
convertVoltsToRawDefinition = addMethod(M2kAnalogOutDefinition, ...
    "int libm2k::analog::M2kAnalogOut::convertVoltsToRaw(double voltage,double vlsb,double filterCompensation)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.convertVoltsToRaw    Method of C++ class libm2k::analog::M2kAnalogOut::convertVoltsToRaw"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(convertVoltsToRawDefinition, "voltage", "double");
defineArgument(convertVoltsToRawDefinition, "vlsb", "double");
defineArgument(convertVoltsToRawDefinition, "filterCompensation", "double");
defineOutput(convertVoltsToRawDefinition, "RetVal", "int32");
validate(convertVoltsToRawDefinition);

%% C++ class method |push| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: void libm2k::analog::M2kAnalogOut::push(unsigned int chnIdx,double * data,unsigned int nb_samples)
pushDefinition = addMethod(M2kAnalogOutDefinition, ...
   "void libm2k::analog::M2kAnalogOut::push(unsigned int chnIdx,double * data,unsigned int nb_samples)", ...
   "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.push    Method of C++ class libm2k::analog::M2kAnalogOut::push"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(pushDefinition, "chnIdx", "uint32");
defineArgument(pushDefinition, "data", "double", "input", "nb_samples");
defineArgument(pushDefinition, "nb_samples", "uint32");
validate(pushDefinition);

%% C++ class method |pushRaw| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: void libm2k::analog::M2kAnalogOut::pushRaw(unsigned int chnIdx,short * data,unsigned int nb_samples)
pushRawDefinition = addMethod(M2kAnalogOutDefinition, ...
   "void libm2k::analog::M2kAnalogOut::pushRaw(unsigned int chnIdx,short * data,unsigned int nb_samples)", ...
   "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.pushRaw    Method of C++ class libm2k::analog::M2kAnalogOut::pushRaw"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(pushRawDefinition, "chnIdx", "uint32");
defineArgument(pushRawDefinition, "data", "int16", "input", "nb_samples");
defineArgument(pushRawDefinition, "nb_samples", "uint32");
validate(pushRawDefinition);

%% C++ class method |pushInterleaved| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: void libm2k::analog::M2kAnalogOut::pushInterleaved(double * data,unsigned int nb_channels,unsigned int nb_samples_per_channel)
pushInterleavedDefinition = addMethod(M2kAnalogOutDefinition, ...
   "void libm2k::analog::M2kAnalogOut::pushInterleaved(double * data,unsigned int nb_channels,unsigned int nb_samples_per_channel)", ...
   "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.pushInterleaved    Method of C++ class libm2k::analog::M2kAnalogOut::pushInterleaved"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(pushInterleavedDefinition, "data", "double", "input", "nb_samples_per_channel");
defineArgument(pushInterleavedDefinition, "nb_channels", "uint32");
defineArgument(pushInterleavedDefinition, "nb_samples_per_channel", "uint32");
validate(pushInterleavedDefinition);

%% C++ class method |pushRawInterleaved| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: void libm2k::analog::M2kAnalogOut::pushRawInterleaved(short * data,unsigned int nb_channels,unsigned int nb_samples_per_channel)
pushRawInterleavedDefinition = addMethod(M2kAnalogOutDefinition, ...
   "void libm2k::analog::M2kAnalogOut::pushRawInterleaved(short * data,unsigned int nb_channels,unsigned int nb_samples_per_channel)", ...
   "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.pushRawInterleaved    Method of C++ class libm2k::analog::M2kAnalogOut::pushRawInterleaved"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(pushRawInterleavedDefinition, "data", "int16", "input", "nb_samples_per_channel");
defineArgument(pushRawInterleavedDefinition, "nb_channels", "uint32");
defineArgument(pushRawInterleavedDefinition, "nb_samples_per_channel", "uint32");
validate(pushRawInterleavedDefinition);

%% C++ class method |setDacCalibVlsb| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: void libm2k::analog::M2kAnalogOut::setDacCalibVlsb(unsigned int chn,double vlsb)
setDacCalibVlsbDefinition = addMethod(M2kAnalogOutDefinition, ...
    "void libm2k::analog::M2kAnalogOut::setDacCalibVlsb(unsigned int chn,double vlsb)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.setDacCalibVlsb    Method of C++ class libm2k::analog::M2kAnalogOut::setDacCalibVlsb"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setDacCalibVlsbDefinition, "chn", "uint32");
defineArgument(setDacCalibVlsbDefinition, "vlsb", "double");
validate(setDacCalibVlsbDefinition);

%% C++ class method |stop| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: void libm2k::analog::M2kAnalogOut::stop()
stopDefinition = addMethod(M2kAnalogOutDefinition, ...
    "void libm2k::analog::M2kAnalogOut::stop()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.stop    Method of C++ class libm2k::analog::M2kAnalogOut::stop"); % This description is shown as help to user. Modify it to appropriate description.
validate(stopDefinition);

%% C++ class method |stop| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: void libm2k::analog::M2kAnalogOut::stop(unsigned int chn)
stopDefinition = addMethod(M2kAnalogOutDefinition, ...
    "void libm2k::analog::M2kAnalogOut::stop(unsigned int chn)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.stop    Method of C++ class libm2k::analog::M2kAnalogOut::stop"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(stopDefinition, "chn", "uint32");
validate(stopDefinition);

%% C++ class method |enableChannel| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: void libm2k::analog::M2kAnalogOut::enableChannel(unsigned int chnIdx,bool enable)
enableChannelDefinition = addMethod(M2kAnalogOutDefinition, ...
    "void libm2k::analog::M2kAnalogOut::enableChannel(unsigned int chnIdx,bool enable)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.enableChannel    Method of C++ class libm2k::analog::M2kAnalogOut::enableChannel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(enableChannelDefinition, "chnIdx", "uint32");
defineArgument(enableChannelDefinition, "enable", "logical");
validate(enableChannelDefinition);

%% C++ class method |isChannelEnabled| for C++ class |libm2k::analog::M2kAnalogOut| 
% C++ Signature: bool libm2k::analog::M2kAnalogOut::isChannelEnabled(unsigned int chnIdx)
isChannelEnabledDefinition = addMethod(M2kAnalogOutDefinition, ...
    "bool libm2k::analog::M2kAnalogOut::isChannelEnabled(unsigned int chnIdx)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kAnalogOut.isChannelEnabled    Method of C++ class libm2k::analog::M2kAnalogOut::isChannelEnabled"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(isChannelEnabledDefinition, "chnIdx", "uint32");
defineOutput(isChannelEnabledDefinition, "RetVal", "logical");
validate(isChannelEnabledDefinition);

%% C++ class |libm2k::analog::GenericAnalogIn| with MATLAB name |clib.libm2k.libm2k.analog.GenericAnalogIn| 
GenericAnalogInDefinition = addClass(libDef, "libm2k::analog::GenericAnalogIn", "MATLABName", "clib.libm2k.libm2k.analog.GenericAnalogIn", ...
    "Description", "clib.libm2k.libm2k.analog.GenericAnalogIn    Representation of C++ class libm2k::analog::GenericAnalogIn"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |libm2k::analog::GenericAnalogOut| with MATLAB name |clib.libm2k.libm2k.analog.GenericAnalogOut| 
GenericAnalogOutDefinition = addClass(libDef, "libm2k::analog::GenericAnalogOut", "MATLABName", "clib.libm2k.libm2k.analog.GenericAnalogOut", ...
    "Description", "clib.libm2k.libm2k.analog.GenericAnalogOut    Representation of C++ class libm2k::analog::GenericAnalogOut"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |libm2k::analog::iio_context| with MATLAB name |clib.libm2k.libm2k.analog.iio_context| 
% iio_contextDefinition = addClass(libDef, "libm2k::analog::iio_context", "MATLABName", "clib.libm2k.libm2k.analog.iio_context", ...
%     "Description", "clib.libm2k.libm2k.analog.iio_context    Representation of C++ class libm2k::analog::iio_context"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |libm2k::analog::M2kPowerSupply| with MATLAB name |clib.libm2k.libm2k.analog.M2kPowerSupply| 
M2kPowerSupplyDefinition = addClass(libDef, "libm2k::analog::M2kPowerSupply", "MATLABName", "clib.libm2k.libm2k.analog.M2kPowerSupply", ...
    "Description", "clib.libm2k.libm2k.analog.M2kPowerSupply    Representation of C++ class libm2k::analog::M2kPowerSupply"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::analog::M2kPowerSupply| 
% C++ Signature: libm2k::analog::M2kPowerSupply::M2kPowerSupply(libm2k::analog::iio_context * ctx,std::string write_dev,std::string read_dev,bool sync)
%M2kPowerSupplyConstructor1Definition = addConstructor(M2kPowerSupplyDefinition, ...
%    "libm2k::analog::M2kPowerSupply::M2kPowerSupply(libm2k::analog::iio_context * ctx,std::string write_dev,std::string read_dev,bool sync)", ...
%    "Description", "clib.libm2k.libm2k.analog.M2kPowerSupply    Constructor of C++ class libm2k::analog::M2kPowerSupply"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(M2kPowerSupplyConstructor1Definition, "ctx", "clib.libm2k.libm2k.analog.iio_context", "input", <SHAPE>);
%defineArgument(M2kPowerSupplyConstructor1Definition, "write_dev", "string");
%defineArgument(M2kPowerSupplyConstructor1Definition, "read_dev", "string");
%defineArgument(M2kPowerSupplyConstructor1Definition, "sync", "logical");
%validate(M2kPowerSupplyConstructor1Definition);

%% C++ class method |init| for C++ class |libm2k::analog::M2kPowerSupply| 
% C++ Signature: void libm2k::analog::M2kPowerSupply::init()
initDefinition = addMethod(M2kPowerSupplyDefinition, ...
    "void libm2k::analog::M2kPowerSupply::init()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kPowerSupply.init    Method of C++ class libm2k::analog::M2kPowerSupply::init"); % This description is shown as help to user. Modify it to appropriate description.
validate(initDefinition);

%% C++ class method |enableChannel| for C++ class |libm2k::analog::M2kPowerSupply| 
% C++ Signature: void libm2k::analog::M2kPowerSupply::enableChannel(unsigned int chn,bool en)
enableChannelDefinition = addMethod(M2kPowerSupplyDefinition, ...
    "void libm2k::analog::M2kPowerSupply::enableChannel(unsigned int chn,bool en)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kPowerSupply.enableChannel    Method of C++ class libm2k::analog::M2kPowerSupply::enableChannel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(enableChannelDefinition, "chn", "uint32");
defineArgument(enableChannelDefinition, "en", "logical");
validate(enableChannelDefinition);

%% C++ class method |enableAll| for C++ class |libm2k::analog::M2kPowerSupply| 
% C++ Signature: void libm2k::analog::M2kPowerSupply::enableAll(bool en)
enableAllDefinition = addMethod(M2kPowerSupplyDefinition, ...
    "void libm2k::analog::M2kPowerSupply::enableAll(bool en)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kPowerSupply.enableAll    Method of C++ class libm2k::analog::M2kPowerSupply::enableAll"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(enableAllDefinition, "en", "logical");
validate(enableAllDefinition);

%% C++ class method |readChannel| for C++ class |libm2k::analog::M2kPowerSupply| 
% C++ Signature: double libm2k::analog::M2kPowerSupply::readChannel(unsigned int chn)
readChannelDefinition = addMethod(M2kPowerSupplyDefinition, ...
    "double libm2k::analog::M2kPowerSupply::readChannel(unsigned int chn)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kPowerSupply.readChannel    Method of C++ class libm2k::analog::M2kPowerSupply::readChannel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(readChannelDefinition, "chn", "uint32");
defineOutput(readChannelDefinition, "RetVal", "double");
validate(readChannelDefinition);

%% C++ class method |pushChannel| for C++ class |libm2k::analog::M2kPowerSupply| 
% C++ Signature: void libm2k::analog::M2kPowerSupply::pushChannel(unsigned int chn,double value)
pushChannelDefinition = addMethod(M2kPowerSupplyDefinition, ...
    "void libm2k::analog::M2kPowerSupply::pushChannel(unsigned int chn,double value)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kPowerSupply.pushChannel    Method of C++ class libm2k::analog::M2kPowerSupply::pushChannel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(pushChannelDefinition, "chn", "uint32");
defineArgument(pushChannelDefinition, "value", "double");
validate(pushChannelDefinition);

%% C++ class method |powerDownDacs| for C++ class |libm2k::analog::M2kPowerSupply| 
% C++ Signature: void libm2k::analog::M2kPowerSupply::powerDownDacs(bool powerdown)
powerDownDacsDefinition = addMethod(M2kPowerSupplyDefinition, ...
    "void libm2k::analog::M2kPowerSupply::powerDownDacs(bool powerdown)", ...
    "Description", "clib.libm2k.libm2k.analog.M2kPowerSupply.powerDownDacs    Method of C++ class libm2k::analog::M2kPowerSupply::powerDownDacs"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(powerDownDacsDefinition, "powerdown", "logical");
validate(powerDownDacsDefinition);

%% C++ class method |anyChannelEnabled| for C++ class |libm2k::analog::M2kPowerSupply| 
% C++ Signature: bool libm2k::analog::M2kPowerSupply::anyChannelEnabled()
anyChannelEnabledDefinition = addMethod(M2kPowerSupplyDefinition, ...
    "bool libm2k::analog::M2kPowerSupply::anyChannelEnabled()", ...
    "Description", "clib.libm2k.libm2k.analog.M2kPowerSupply.anyChannelEnabled    Method of C++ class libm2k::analog::M2kPowerSupply::anyChannelEnabled"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(anyChannelEnabledDefinition, "RetVal", "logical");
validate(anyChannelEnabledDefinition);

%% C++ class |libm2k::analog::DMM_READING| with MATLAB name |clib.libm2k.libm2k.analog.DMM_READING| 
DMM_READINGDefinition = addClass(libDef, "libm2k::analog::DMM_READING", "MATLABName", "clib.libm2k.libm2k.analog.DMM_READING", ...
    "Description", "clib.libm2k.libm2k.analog.DMM_READING    Representation of C++ class libm2k::analog::DMM_READING"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::analog::DMM_READING| 
% C++ Signature: libm2k::analog::DMM_READING::DMM_READING()
DMM_READINGConstructor1Definition = addConstructor(DMM_READINGDefinition, ...
    "libm2k::analog::DMM_READING::DMM_READING()", ...
    "Description", "clib.libm2k.libm2k.analog.DMM_READING    Constructor of C++ class libm2k::analog::DMM_READING"); % This description is shown as help to user. Modify it to appropriate description.
validate(DMM_READINGConstructor1Definition);

%% C++ class constructor for C++ class |libm2k::analog::DMM_READING| 
% C++ Signature: libm2k::analog::DMM_READING::DMM_READING(libm2k::analog::DMM_READING const & input1)
DMM_READINGConstructor2Definition = addConstructor(DMM_READINGDefinition, ...
    "libm2k::analog::DMM_READING::DMM_READING(libm2k::analog::DMM_READING const & input1)", ...
    "Description", "clib.libm2k.libm2k.analog.DMM_READING    Constructor of C++ class libm2k::analog::DMM_READING"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(DMM_READINGConstructor2Definition, "input1", "clib.libm2k.libm2k.analog.DMM_READING", "input");
validate(DMM_READINGConstructor2Definition);

%% C++ class public data member |name| for C++ class |libm2k::analog::DMM_READING| 
% C++ Signature: std::string libm2k::analog::DMM_READING::name
addProperty(DMM_READINGDefinition, "name", "string", ...
    "Description", "string    Data Member of C++ class libm2k::analog::DMM_READING::name"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class public data member |id| for C++ class |libm2k::analog::DMM_READING| 
% C++ Signature: std::string libm2k::analog::DMM_READING::id
addProperty(DMM_READINGDefinition, "id", "string", ...
    "Description", "string    Data Member of C++ class libm2k::analog::DMM_READING::id"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class public data member |value| for C++ class |libm2k::analog::DMM_READING| 
% C++ Signature: double libm2k::analog::DMM_READING::value
addProperty(DMM_READINGDefinition, "value", "double", ...
    "Description", "double    Data Member of C++ class libm2k::analog::DMM_READING::value"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class public data member |unit| for C++ class |libm2k::analog::DMM_READING| 
% C++ Signature: std::string libm2k::analog::DMM_READING::unit
addProperty(DMM_READINGDefinition, "unit", "string", ...
    "Description", "string    Data Member of C++ class libm2k::analog::DMM_READING::unit"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |libm2k::analog::PowerSupply| with MATLAB name |clib.libm2k.libm2k.analog.PowerSupply| 
PowerSupplyDefinition = addClass(libDef, "libm2k::analog::PowerSupply", "MATLABName", "clib.libm2k.libm2k.analog.PowerSupply", ...
    "Description", "clib.libm2k.libm2k.analog.PowerSupply    Representation of C++ class libm2k::analog::PowerSupply"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::analog::PowerSupply| 
% C++ Signature: libm2k::analog::PowerSupply::PowerSupply(libm2k::analog::iio_context * ctx,std::string write_dev,std::string read_dev)
%PowerSupplyConstructor1Definition = addConstructor(PowerSupplyDefinition, ...
%    "libm2k::analog::PowerSupply::PowerSupply(libm2k::analog::iio_context * ctx,std::string write_dev,std::string read_dev)", ...
%    "Description", "clib.libm2k.libm2k.analog.PowerSupply    Constructor of C++ class libm2k::analog::PowerSupply"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(PowerSupplyConstructor1Definition, "ctx", "clib.libm2k.libm2k.analog.iio_context", "input", <SHAPE>);
%defineArgument(PowerSupplyConstructor1Definition, "write_dev", "string");
%defineArgument(PowerSupplyConstructor1Definition, "read_dev", "string");
%validate(PowerSupplyConstructor1Definition);

%% C++ class method |enableChannel| for C++ class |libm2k::analog::PowerSupply| 
% C++ Signature: void libm2k::analog::PowerSupply::enableChannel(unsigned int chnidx,bool enable)
enableChannelDefinition = addMethod(PowerSupplyDefinition, ...
    "void libm2k::analog::PowerSupply::enableChannel(unsigned int chnidx,bool enable)", ...
    "Description", "clib.libm2k.libm2k.analog.PowerSupply.enableChannel    Method of C++ class libm2k::analog::PowerSupply::enableChannel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(enableChannelDefinition, "chnidx", "uint32");
defineArgument(enableChannelDefinition, "enable", "logical");
validate(enableChannelDefinition);

%% C++ enumeration |libm2k::digital::DIO_TRIGGER_MODE| with MATLAB name |clib.libm2k.libm2k.digital.DIO_TRIGGER_MODE| 
addEnumeration(libDef, "libm2k::digital::DIO_TRIGGER_MODE", "int32",...
    [...
      "DIO_OR",...  % 0
      "DIO_AND",...  % 1
    ],...
    "MATLABName", "clib.libm2k.libm2k.digital.DIO_TRIGGER_MODE", ...
    "Description", "clib.libm2k.libm2k.digital.DIO_TRIGGER_MODE    Representation of C++ enumeration libm2k::digital::DIO_TRIGGER_MODE"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |libm2k::digital::DIO_DIRECTION| with MATLAB name |clib.libm2k.libm2k.digital.DIO_DIRECTION| 
addEnumeration(libDef, "libm2k::digital::DIO_DIRECTION", "int32",...
    [...
      "DIO_INPUT",...  % 0
      "DIO_OUTPUT",...  % 1
    ],...
    "MATLABName", "clib.libm2k.libm2k.digital.DIO_DIRECTION", ...
    "Description", "clib.libm2k.libm2k.digital.DIO_DIRECTION    Representation of C++ enumeration libm2k::digital::DIO_DIRECTION"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |libm2k::digital::DIO_CHANNEL| with MATLAB name |clib.libm2k.libm2k.digital.DIO_CHANNEL| 
addEnumeration(libDef, "libm2k::digital::DIO_CHANNEL", "int32",...
    [...
      "DIO_CHANNEL_0",...  % 0
      "DIO_CHANNEL_1",...  % 1
      "DIO_CHANNEL_2",...  % 2
      "DIO_CHANNEL_3",...  % 3
      "DIO_CHANNEL_4",...  % 4
      "DIO_CHANNEL_5",...  % 5
      "DIO_CHANNEL_6",...  % 6
      "DIO_CHANNEL_7",...  % 7
      "DIO_CHANNEL_8",...  % 8
      "DIO_CHANNEL_9",...  % 9
      "DIO_CHANNEL_10",...  % 10
      "DIO_CHANNEL_11",...  % 11
      "DIO_CHANNEL_12",...  % 12
      "DIO_CHANNEL_13",...  % 13
      "DIO_CHANNEL_14",...  % 14
      "DIO_CHANNEL_15",...  % 15
    ],...
    "MATLABName", "clib.libm2k.libm2k.digital.DIO_CHANNEL", ...
    "Description", "clib.libm2k.libm2k.digital.DIO_CHANNEL    Representation of C++ enumeration libm2k::digital::DIO_CHANNEL"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |libm2k::digital::DIO_LEVEL| with MATLAB name |clib.libm2k.libm2k.digital.DIO_LEVEL| 
addEnumeration(libDef, "libm2k::digital::DIO_LEVEL", "int32",...
    [...
      "LOW",...  % 0
      "HIGH",...  % 1
    ],...
    "MATLABName", "clib.libm2k.libm2k.digital.DIO_LEVEL", ...
    "Description", "clib.libm2k.libm2k.digital.DIO_LEVEL    Representation of C++ enumeration libm2k::digital::DIO_LEVEL"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ enumeration |libm2k::digital::DIO_MODE| with MATLAB name |clib.libm2k.libm2k.digital.DIO_MODE| 
addEnumeration(libDef, "libm2k::digital::DIO_MODE", "int32",...
    [...
      "DIO_OPENDRAIN",...  % 0
      "DIO_PUSHPULL",...  % 1
    ],...
    "MATLABName", "clib.libm2k.libm2k.digital.DIO_MODE", ...
    "Description", "clib.libm2k.libm2k.digital.DIO_MODE    Representation of C++ enumeration libm2k::digital::DIO_MODE"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |libm2k::digital::M2kDigital| with MATLAB name |clib.libm2k.libm2k.digital.M2kDigital| 
M2kDigitalDefinition = addClass(libDef, "libm2k::digital::M2kDigital", "MATLABName", "clib.libm2k.libm2k.digital.M2kDigital", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital    Representation of C++ class libm2k::digital::M2kDigital"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: libm2k::digital::M2kDigital::M2kDigital(iio_context * ctx,std::string logic_dev,bool sync)
%M2kDigitalConstructor1Definition = addConstructor(M2kDigitalDefinition, ...
%    "libm2k::digital::M2kDigital::M2kDigital(iio_context * ctx,std::string logic_dev,bool sync)", ...
%    "Description", "clib.libm2k.libm2k.digital.M2kDigital    Constructor of C++ class libm2k::digital::M2kDigital"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(M2kDigitalConstructor1Definition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineArgument(M2kDigitalConstructor1Definition, "logic_dev", "string");
%defineArgument(M2kDigitalConstructor1Definition, "sync", "logical");
%validate(M2kDigitalConstructor1Definition);

%% C++ class method |init| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::init()
initDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::init()", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.init    Method of C++ class libm2k::digital::M2kDigital::init"); % This description is shown as help to user. Modify it to appropriate description.
validate(initDefinition);

%% C++ class method |setDirection| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::setDirection(unsigned short mask)
setDirectionDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::setDirection(unsigned short mask)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setDirection    Method of C++ class libm2k::digital::M2kDigital::setDirection"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setDirectionDefinition, "mask", "uint16");
validate(setDirectionDefinition);

%% C++ class method |setDirection| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::setDirection(unsigned int index,libm2k::digital::DIO_DIRECTION dir)
setDirectionDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::setDirection(unsigned int index,libm2k::digital::DIO_DIRECTION dir)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setDirection    Method of C++ class libm2k::digital::M2kDigital::setDirection"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setDirectionDefinition, "index", "uint32");
defineArgument(setDirectionDefinition, "dir", "clib.libm2k.libm2k.digital.DIO_DIRECTION");
validate(setDirectionDefinition);

%% C++ class method |setDirection| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::setDirection(unsigned int index,bool dir)
setDirectionDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::setDirection(unsigned int index,bool dir)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setDirection    Method of C++ class libm2k::digital::M2kDigital::setDirection"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setDirectionDefinition, "index", "uint32");
defineArgument(setDirectionDefinition, "dir", "logical");
validate(setDirectionDefinition);

%% C++ class method |setDirection| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::setDirection(libm2k::digital::DIO_CHANNEL index,bool dir)
setDirectionDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::setDirection(libm2k::digital::DIO_CHANNEL index,bool dir)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setDirection    Method of C++ class libm2k::digital::M2kDigital::setDirection"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setDirectionDefinition, "index", "clib.libm2k.libm2k.digital.DIO_CHANNEL");
defineArgument(setDirectionDefinition, "dir", "logical");
validate(setDirectionDefinition);

%% C++ class method |setDirection| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::setDirection(libm2k::digital::DIO_CHANNEL index,libm2k::digital::DIO_DIRECTION dir)
setDirectionDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::setDirection(libm2k::digital::DIO_CHANNEL index,libm2k::digital::DIO_DIRECTION dir)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setDirection    Method of C++ class libm2k::digital::M2kDigital::setDirection"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setDirectionDefinition, "index", "clib.libm2k.libm2k.digital.DIO_CHANNEL");
defineArgument(setDirectionDefinition, "dir", "clib.libm2k.libm2k.digital.DIO_DIRECTION");
validate(setDirectionDefinition);

%% C++ class method |getDirection| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: libm2k::digital::DIO_DIRECTION libm2k::digital::M2kDigital::getDirection(libm2k::digital::DIO_CHANNEL index)
getDirectionDefinition = addMethod(M2kDigitalDefinition, ...
    "libm2k::digital::DIO_DIRECTION libm2k::digital::M2kDigital::getDirection(libm2k::digital::DIO_CHANNEL index)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.getDirection    Method of C++ class libm2k::digital::M2kDigital::getDirection"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getDirectionDefinition, "index", "clib.libm2k.libm2k.digital.DIO_CHANNEL");
defineOutput(getDirectionDefinition, "RetVal", "clib.libm2k.libm2k.digital.DIO_DIRECTION");
validate(getDirectionDefinition);

%% C++ class method |setValueRaw| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::setValueRaw(libm2k::digital::DIO_CHANNEL index,libm2k::digital::DIO_LEVEL level)
setValueRawDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::setValueRaw(libm2k::digital::DIO_CHANNEL index,libm2k::digital::DIO_LEVEL level)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setValueRaw    Method of C++ class libm2k::digital::M2kDigital::setValueRaw"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setValueRawDefinition, "index", "clib.libm2k.libm2k.digital.DIO_CHANNEL");
defineArgument(setValueRawDefinition, "level", "clib.libm2k.libm2k.digital.DIO_LEVEL");
validate(setValueRawDefinition);

%% C++ class method |push| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::push(unsigned short * data,unsigned int nb_samples)
pushDefinition = addMethod(M2kDigitalDefinition, ...
   "void libm2k::digital::M2kDigital::push(unsigned short * data,unsigned int nb_samples)", ...
   "Description", "clib.libm2k.libm2k.digital.M2kDigital.push    Method of C++ class libm2k::digital::M2kDigital::push"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(pushDefinition, "data", "uint16", "input", "nb_samples");
defineArgument(pushDefinition, "nb_samples", "uint32");
validate(pushDefinition);

%% C++ class method |setValueRaw| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::setValueRaw(unsigned int index,libm2k::digital::DIO_LEVEL level)
setValueRawDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::setValueRaw(unsigned int index,libm2k::digital::DIO_LEVEL level)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setValueRaw    Method of C++ class libm2k::digital::M2kDigital::setValueRaw"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setValueRawDefinition, "index", "uint32");
defineArgument(setValueRawDefinition, "level", "clib.libm2k.libm2k.digital.DIO_LEVEL");
validate(setValueRawDefinition);

%% C++ class method |setValueRaw| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::setValueRaw(libm2k::digital::DIO_CHANNEL index,bool level)
setValueRawDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::setValueRaw(libm2k::digital::DIO_CHANNEL index,bool level)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setValueRaw    Method of C++ class libm2k::digital::M2kDigital::setValueRaw"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setValueRawDefinition, "index", "clib.libm2k.libm2k.digital.DIO_CHANNEL");
defineArgument(setValueRawDefinition, "level", "logical");
validate(setValueRawDefinition);

%% C++ class method |getValueRaw| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: libm2k::digital::DIO_LEVEL libm2k::digital::M2kDigital::getValueRaw(libm2k::digital::DIO_CHANNEL index)
getValueRawDefinition = addMethod(M2kDigitalDefinition, ...
    "libm2k::digital::DIO_LEVEL libm2k::digital::M2kDigital::getValueRaw(libm2k::digital::DIO_CHANNEL index)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.getValueRaw    Method of C++ class libm2k::digital::M2kDigital::getValueRaw"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getValueRawDefinition, "index", "clib.libm2k.libm2k.digital.DIO_CHANNEL");
defineOutput(getValueRawDefinition, "RetVal", "clib.libm2k.libm2k.digital.DIO_LEVEL");
validate(getValueRawDefinition);

%% C++ class method |getValueRaw| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: libm2k::digital::DIO_LEVEL libm2k::digital::M2kDigital::getValueRaw(unsigned int index)
getValueRawDefinition = addMethod(M2kDigitalDefinition, ...
    "libm2k::digital::DIO_LEVEL libm2k::digital::M2kDigital::getValueRaw(unsigned int index)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.getValueRaw    Method of C++ class libm2k::digital::M2kDigital::getValueRaw"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getValueRawDefinition, "index", "uint32");
defineOutput(getValueRawDefinition, "RetVal", "clib.libm2k.libm2k.digital.DIO_LEVEL");
validate(getValueRawDefinition);

%% C++ class method |stop| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::stop()
stopDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::stop()", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.stop    Method of C++ class libm2k::digital::M2kDigital::stop"); % This description is shown as help to user. Modify it to appropriate description.
validate(stopDefinition);

%% C++ class method |getSamplesP| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: unsigned short const * libm2k::digital::M2kDigital::getSamplesP(unsigned int nb_samples)
getSamplesPDefinition = addMethod(M2kDigitalDefinition, ...
   "unsigned short const * libm2k::digital::M2kDigital::getSamplesP(unsigned int nb_samples)", ...
   "Description", "clib.libm2k.libm2k.digital.M2kDigital.getSamplesP    Method of C++ class libm2k::digital::M2kDigital::getSamplesP"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getSamplesPDefinition, "nb_samples", "uint32");
defineOutput(getSamplesPDefinition, "RetVal", "uint16", "nb_samples");
validate(getSamplesPDefinition);

%% C++ class method |enableChannel| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::enableChannel(unsigned int index,bool enable)
enableChannelDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::enableChannel(unsigned int index,bool enable)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.enableChannel    Method of C++ class libm2k::digital::M2kDigital::enableChannel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(enableChannelDefinition, "index", "uint32");
defineArgument(enableChannelDefinition, "enable", "logical");
validate(enableChannelDefinition);

%% C++ class method |enableChannel| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::enableChannel(libm2k::digital::DIO_CHANNEL index,bool enable)
enableChannelDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::enableChannel(libm2k::digital::DIO_CHANNEL index,bool enable)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.enableChannel    Method of C++ class libm2k::digital::M2kDigital::enableChannel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(enableChannelDefinition, "index", "clib.libm2k.libm2k.digital.DIO_CHANNEL");
defineArgument(enableChannelDefinition, "enable", "logical");
validate(enableChannelDefinition);

%% C++ class method |enableAllOut| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::enableAllOut(bool enable)
enableAllOutDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::enableAllOut(bool enable)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.enableAllOut    Method of C++ class libm2k::digital::M2kDigital::enableAllOut"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(enableAllOutDefinition, "enable", "logical");
validate(enableAllOutDefinition);

%% C++ class method |anyChannelEnabled| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: bool libm2k::digital::M2kDigital::anyChannelEnabled(libm2k::digital::DIO_DIRECTION dir)
anyChannelEnabledDefinition = addMethod(M2kDigitalDefinition, ...
    "bool libm2k::digital::M2kDigital::anyChannelEnabled(libm2k::digital::DIO_DIRECTION dir)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.anyChannelEnabled    Method of C++ class libm2k::digital::M2kDigital::anyChannelEnabled"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(anyChannelEnabledDefinition, "dir", "clib.libm2k.libm2k.digital.DIO_DIRECTION");
defineOutput(anyChannelEnabledDefinition, "RetVal", "logical");
validate(anyChannelEnabledDefinition);

%% C++ class method |setOutputMode| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::setOutputMode(libm2k::digital::DIO_CHANNEL chn,libm2k::digital::DIO_MODE mode)
setOutputModeDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::setOutputMode(libm2k::digital::DIO_CHANNEL chn,libm2k::digital::DIO_MODE mode)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setOutputMode    Method of C++ class libm2k::digital::M2kDigital::setOutputMode"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setOutputModeDefinition, "chn", "clib.libm2k.libm2k.digital.DIO_CHANNEL");
defineArgument(setOutputModeDefinition, "mode", "clib.libm2k.libm2k.digital.DIO_MODE");
validate(setOutputModeDefinition);

%% C++ class method |setOutputMode| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::setOutputMode(unsigned int chn,libm2k::digital::DIO_MODE mode)
setOutputModeDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::setOutputMode(unsigned int chn,libm2k::digital::DIO_MODE mode)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setOutputMode    Method of C++ class libm2k::digital::M2kDigital::setOutputMode"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setOutputModeDefinition, "chn", "uint32");
defineArgument(setOutputModeDefinition, "mode", "clib.libm2k.libm2k.digital.DIO_MODE");
validate(setOutputModeDefinition);

%% C++ class method |getOutputMode| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: libm2k::digital::DIO_MODE libm2k::digital::M2kDigital::getOutputMode(libm2k::digital::DIO_CHANNEL chn)
getOutputModeDefinition = addMethod(M2kDigitalDefinition, ...
    "libm2k::digital::DIO_MODE libm2k::digital::M2kDigital::getOutputMode(libm2k::digital::DIO_CHANNEL chn)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.getOutputMode    Method of C++ class libm2k::digital::M2kDigital::getOutputMode"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getOutputModeDefinition, "chn", "clib.libm2k.libm2k.digital.DIO_CHANNEL");
defineOutput(getOutputModeDefinition, "RetVal", "clib.libm2k.libm2k.digital.DIO_MODE");
validate(getOutputModeDefinition);

%% C++ class method |getOutputMode| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: libm2k::digital::DIO_MODE libm2k::digital::M2kDigital::getOutputMode(unsigned int chn)
getOutputModeDefinition = addMethod(M2kDigitalDefinition, ...
    "libm2k::digital::DIO_MODE libm2k::digital::M2kDigital::getOutputMode(unsigned int chn)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.getOutputMode    Method of C++ class libm2k::digital::M2kDigital::getOutputMode"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getOutputModeDefinition, "chn", "uint32");
defineOutput(getOutputModeDefinition, "RetVal", "clib.libm2k.libm2k.digital.DIO_MODE");
validate(getOutputModeDefinition);

%% C++ class method |setSampleRateIn| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: double libm2k::digital::M2kDigital::setSampleRateIn(double samplerate)
setSampleRateInDefinition = addMethod(M2kDigitalDefinition, ...
    "double libm2k::digital::M2kDigital::setSampleRateIn(double samplerate)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setSampleRateIn    Method of C++ class libm2k::digital::M2kDigital::setSampleRateIn"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setSampleRateInDefinition, "samplerate", "double");
defineOutput(setSampleRateInDefinition, "RetVal", "double");
validate(setSampleRateInDefinition);

%% C++ class method |setSampleRateOut| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: double libm2k::digital::M2kDigital::setSampleRateOut(double samplerate)
setSampleRateOutDefinition = addMethod(M2kDigitalDefinition, ...
    "double libm2k::digital::M2kDigital::setSampleRateOut(double samplerate)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setSampleRateOut    Method of C++ class libm2k::digital::M2kDigital::setSampleRateOut"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setSampleRateOutDefinition, "samplerate", "double");
defineOutput(setSampleRateOutDefinition, "RetVal", "double");
validate(setSampleRateOutDefinition);

%% C++ class method |getSampleRateIn| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: double libm2k::digital::M2kDigital::getSampleRateIn()
getSampleRateInDefinition = addMethod(M2kDigitalDefinition, ...
    "double libm2k::digital::M2kDigital::getSampleRateIn()", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.getSampleRateIn    Method of C++ class libm2k::digital::M2kDigital::getSampleRateIn"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getSampleRateInDefinition, "RetVal", "double");
validate(getSampleRateInDefinition);

%% C++ class method |getSampleRateOut| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: double libm2k::digital::M2kDigital::getSampleRateOut()
getSampleRateOutDefinition = addMethod(M2kDigitalDefinition, ...
    "double libm2k::digital::M2kDigital::getSampleRateOut()", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.getSampleRateOut    Method of C++ class libm2k::digital::M2kDigital::getSampleRateOut"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getSampleRateOutDefinition, "RetVal", "double");
validate(getSampleRateOutDefinition);

%% C++ class method |getCyclic| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: bool libm2k::digital::M2kDigital::getCyclic()
getCyclicDefinition = addMethod(M2kDigitalDefinition, ...
    "bool libm2k::digital::M2kDigital::getCyclic()", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.getCyclic    Method of C++ class libm2k::digital::M2kDigital::getCyclic"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getCyclicDefinition, "RetVal", "logical");
validate(getCyclicDefinition);

%% C++ class method |setCyclic| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: void libm2k::digital::M2kDigital::setCyclic(bool cyclic)
setCyclicDefinition = addMethod(M2kDigitalDefinition, ...
    "void libm2k::digital::M2kDigital::setCyclic(bool cyclic)", ...
    "Description", "clib.libm2k.libm2k.digital.M2kDigital.setCyclic    Method of C++ class libm2k::digital::M2kDigital::setCyclic"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setCyclicDefinition, "cyclic", "logical");
validate(setCyclicDefinition);

%% C++ class method |getTrigger| for C++ class |libm2k::digital::M2kDigital| 
% C++ Signature: libm2k::analog::M2kHardwareTrigger * libm2k::digital::M2kDigital::getTrigger()
getTriggerDefinition = addMethod(M2kDigitalDefinition, ...
   "libm2k::analog::M2kHardwareTrigger * libm2k::digital::M2kDigital::getTrigger()", ...
   "Description", "clib.libm2k.libm2k.digital.M2kDigital.getTrigger    Method of C++ class libm2k::digital::M2kDigital::getTrigger"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getTriggerDefinition, "RetVal", "clib.libm2k.libm2k.analog.M2kHardwareTrigger", 1);
validate(getTriggerDefinition);

%% C++ class |libm2k::digital::channel| with MATLAB name |clib.libm2k.libm2k.digital.channel| 
channelDefinition = addClass(libDef, "libm2k::digital::channel", "MATLABName", "clib.libm2k.libm2k.digital.channel", ...
    "Description", "clib.libm2k.libm2k.digital.channel    Representation of C++ class libm2k::digital::channel"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::digital::channel| 
% C++ Signature: libm2k::digital::channel::channel()
channelConstructor1Definition = addConstructor(channelDefinition, ...
    "libm2k::digital::channel::channel()", ...
    "Description", "clib.libm2k.libm2k.digital.channel    Constructor of C++ class libm2k::digital::channel"); % This description is shown as help to user. Modify it to appropriate description.
validate(channelConstructor1Definition);

%% C++ class constructor for C++ class |libm2k::digital::channel| 
% C++ Signature: libm2k::digital::channel::channel(libm2k::digital::channel const & input1)
channelConstructor2Definition = addConstructor(channelDefinition, ...
    "libm2k::digital::channel::channel(libm2k::digital::channel const & input1)", ...
    "Description", "clib.libm2k.libm2k.digital.channel    Constructor of C++ class libm2k::digital::channel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(channelConstructor2Definition, "input1", "clib.libm2k.libm2k.digital.channel", "input");
validate(channelConstructor2Definition);

%% C++ class public data member |m_direction| for C++ class |libm2k::digital::channel| 
% C++ Signature: libm2k::digital::DIO_DIRECTION libm2k::digital::channel::m_direction
addProperty(channelDefinition, "m_direction", "clib.libm2k.libm2k.digital.DIO_DIRECTION", ...
    "Description", "clib.libm2k.libm2k.digital.DIO_DIRECTION    Data Member of C++ class libm2k::digital::channel::m_direction"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class |libm2k::digital::GenericDigital| with MATLAB name |clib.libm2k.libm2k.digital.GenericDigital| 
GenericDigitalDefinition = addClass(libDef, "libm2k::digital::GenericDigital", "MATLABName", "clib.libm2k.libm2k.digital.GenericDigital", ...
    "Description", "clib.libm2k.libm2k.digital.GenericDigital    Representation of C++ class libm2k::digital::GenericDigital"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::digital::GenericDigital| 
% C++ Signature: libm2k::digital::GenericDigital::GenericDigital(iio_context * ctx,std::string logic_dev)
%GenericDigitalConstructor1Definition = addConstructor(GenericDigitalDefinition, ...
%    "libm2k::digital::GenericDigital::GenericDigital(iio_context * ctx,std::string logic_dev)", ...
%    "Description", "clib.libm2k.libm2k.digital.GenericDigital    Constructor of C++ class libm2k::digital::GenericDigital"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(GenericDigitalConstructor1Definition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineArgument(GenericDigitalConstructor1Definition, "logic_dev", "string");
%validate(GenericDigitalConstructor1Definition);

%% C++ class method |getSampleRate| for C++ class |libm2k::digital::GenericDigital| 
% C++ Signature: double libm2k::digital::GenericDigital::getSampleRate()
getSampleRateDefinition = addMethod(GenericDigitalDefinition, ...
    "double libm2k::digital::GenericDigital::getSampleRate()", ...
    "Description", "clib.libm2k.libm2k.digital.GenericDigital.getSampleRate    Method of C++ class libm2k::digital::GenericDigital::getSampleRate"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getSampleRateDefinition, "RetVal", "double");
validate(getSampleRateDefinition);

%% C++ class method |setSampleRate| for C++ class |libm2k::digital::GenericDigital| 
% C++ Signature: double libm2k::digital::GenericDigital::setSampleRate(double sampleRate)
setSampleRateDefinition = addMethod(GenericDigitalDefinition, ...
    "double libm2k::digital::GenericDigital::setSampleRate(double sampleRate)", ...
    "Description", "clib.libm2k.libm2k.digital.GenericDigital.setSampleRate    Method of C++ class libm2k::digital::GenericDigital::setSampleRate"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setSampleRateDefinition, "sampleRate", "double");
defineOutput(setSampleRateDefinition, "RetVal", "double");
validate(setSampleRateDefinition);

%% C++ class method |setCyclic| for C++ class |libm2k::digital::GenericDigital| 
% C++ Signature: void libm2k::digital::GenericDigital::setCyclic(bool cyclic)
setCyclicDefinition = addMethod(GenericDigitalDefinition, ...
    "void libm2k::digital::GenericDigital::setCyclic(bool cyclic)", ...
    "Description", "clib.libm2k.libm2k.digital.GenericDigital.setCyclic    Method of C++ class libm2k::digital::GenericDigital::setCyclic"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setCyclicDefinition, "cyclic", "logical");
validate(setCyclicDefinition);

%% C++ class method |getCyclic| for C++ class |libm2k::digital::GenericDigital| 
% C++ Signature: bool libm2k::digital::GenericDigital::getCyclic()
getCyclicDefinition = addMethod(GenericDigitalDefinition, ...
    "bool libm2k::digital::GenericDigital::getCyclic()", ...
    "Description", "clib.libm2k.libm2k.digital.GenericDigital.getCyclic    Method of C++ class libm2k::digital::GenericDigital::getCyclic"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getCyclicDefinition, "RetVal", "logical");
validate(getCyclicDefinition);

%% C++ class method |enableChannel| for C++ class |libm2k::digital::GenericDigital| 
% C++ Signature: void libm2k::digital::GenericDigital::enableChannel(unsigned int index,bool enable)
enableChannelDefinition = addMethod(GenericDigitalDefinition, ...
    "void libm2k::digital::GenericDigital::enableChannel(unsigned int index,bool enable)", ...
    "Description", "clib.libm2k.libm2k.digital.GenericDigital.enableChannel    Method of C++ class libm2k::digital::GenericDigital::enableChannel"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(enableChannelDefinition, "index", "uint32");
defineArgument(enableChannelDefinition, "enable", "logical");
validate(enableChannelDefinition);

%% C++ class |libm2k::devices::M2k| with MATLAB name |clib.libm2k.libm2k.devices.M2k| 
M2kDefinition = addClass(libDef, "libm2k::devices::M2k", "MATLABName", "clib.libm2k.libm2k.devices.M2k", ...
    "Description", "clib.libm2k.libm2k.devices.M2k    Representation of C++ class libm2k::devices::M2k"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::devices::M2k| 
% C++ Signature: libm2k::devices::M2k::M2k(std::string uri,iio_context * ctx,std::string name,bool sync)
%M2kConstructor1Definition = addConstructor(M2kDefinition, ...
%    "libm2k::devices::M2k::M2k(std::string uri,iio_context * ctx,std::string name,bool sync)", ...
%    "Description", "clib.libm2k.libm2k.devices.M2k    Constructor of C++ class libm2k::devices::M2k"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(M2kConstructor1Definition, "uri", "string");
%defineArgument(M2kConstructor1Definition, "ctx", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineArgument(M2kConstructor1Definition, "name", "string");
%defineArgument(M2kConstructor1Definition, "sync", "logical");
%validate(M2kConstructor1Definition);

%% C++ class method |init| for C++ class |libm2k::devices::M2k| 
% C++ Signature: void libm2k::devices::M2k::init()
initDefinition = addMethod(M2kDefinition, ...
    "void libm2k::devices::M2k::init()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.init    Method of C++ class libm2k::devices::M2k::init"); % This description is shown as help to user. Modify it to appropriate description.
validate(initDefinition);

%% C++ class method |deinitialize| for C++ class |libm2k::devices::M2k| 
% C++ Signature: void libm2k::devices::M2k::deinitialize()
deinitializeDefinition = addMethod(M2kDefinition, ...
    "void libm2k::devices::M2k::deinitialize()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.deinitialize    Method of C++ class libm2k::devices::M2k::deinitialize"); % This description is shown as help to user. Modify it to appropriate description.
validate(deinitializeDefinition);

%% C++ class method |scanAllAnalogIn| for C++ class |libm2k::devices::M2k| 
% C++ Signature: void libm2k::devices::M2k::scanAllAnalogIn()
scanAllAnalogInDefinition = addMethod(M2kDefinition, ...
    "void libm2k::devices::M2k::scanAllAnalogIn()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.scanAllAnalogIn    Method of C++ class libm2k::devices::M2k::scanAllAnalogIn"); % This description is shown as help to user. Modify it to appropriate description.
validate(scanAllAnalogInDefinition);

%% C++ class method |scanAllAnalogOut| for C++ class |libm2k::devices::M2k| 
% C++ Signature: void libm2k::devices::M2k::scanAllAnalogOut()
scanAllAnalogOutDefinition = addMethod(M2kDefinition, ...
    "void libm2k::devices::M2k::scanAllAnalogOut()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.scanAllAnalogOut    Method of C++ class libm2k::devices::M2k::scanAllAnalogOut"); % This description is shown as help to user. Modify it to appropriate description.
validate(scanAllAnalogOutDefinition);

%% C++ class method |scanAllPowerSupply| for C++ class |libm2k::devices::M2k| 
% C++ Signature: void libm2k::devices::M2k::scanAllPowerSupply()
scanAllPowerSupplyDefinition = addMethod(M2kDefinition, ...
    "void libm2k::devices::M2k::scanAllPowerSupply()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.scanAllPowerSupply    Method of C++ class libm2k::devices::M2k::scanAllPowerSupply"); % This description is shown as help to user. Modify it to appropriate description.
validate(scanAllPowerSupplyDefinition);

%% C++ class method |scanAllDigital| for C++ class |libm2k::devices::M2k| 
% C++ Signature: void libm2k::devices::M2k::scanAllDigital()
scanAllDigitalDefinition = addMethod(M2kDefinition, ...
    "void libm2k::devices::M2k::scanAllDigital()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.scanAllDigital    Method of C++ class libm2k::devices::M2k::scanAllDigital"); % This description is shown as help to user. Modify it to appropriate description.
validate(scanAllDigitalDefinition);

%% C++ class method |calibrate| for C++ class |libm2k::devices::M2k| 
% C++ Signature: void libm2k::devices::M2k::calibrate()
calibrateDefinition = addMethod(M2kDefinition, ...
    "void libm2k::devices::M2k::calibrate()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.calibrate    Method of C++ class libm2k::devices::M2k::calibrate"); % This description is shown as help to user. Modify it to appropriate description.
validate(calibrateDefinition);

%% C++ class method |calibrateADC| for C++ class |libm2k::devices::M2k| 
% C++ Signature: bool libm2k::devices::M2k::calibrateADC()
calibrateADCDefinition = addMethod(M2kDefinition, ...
    "bool libm2k::devices::M2k::calibrateADC()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.calibrateADC    Method of C++ class libm2k::devices::M2k::calibrateADC"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(calibrateADCDefinition, "RetVal", "logical");
validate(calibrateADCDefinition);

%% C++ class method |calibrateDAC| for C++ class |libm2k::devices::M2k| 
% C++ Signature: bool libm2k::devices::M2k::calibrateDAC()
calibrateDACDefinition = addMethod(M2kDefinition, ...
    "bool libm2k::devices::M2k::calibrateDAC()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.calibrateDAC    Method of C++ class libm2k::devices::M2k::calibrateDAC"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(calibrateDACDefinition, "RetVal", "logical");
validate(calibrateDACDefinition);

%% C++ class method |resetCalibration| for C++ class |libm2k::devices::M2k| 
% C++ Signature: bool libm2k::devices::M2k::resetCalibration()
resetCalibrationDefinition = addMethod(M2kDefinition, ...
    "bool libm2k::devices::M2k::resetCalibration()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.resetCalibration    Method of C++ class libm2k::devices::M2k::resetCalibration"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(resetCalibrationDefinition, "RetVal", "logical");
validate(resetCalibrationDefinition);

%% C++ class method |getDigital| for C++ class |libm2k::devices::M2k| 
% C++ Signature: libm2k::digital::M2kDigital * libm2k::devices::M2k::getDigital()
getDigitalDefinition = addMethod(M2kDefinition, ...
   "libm2k::digital::M2kDigital * libm2k::devices::M2k::getDigital()", ...
   "Description", "clib.libm2k.libm2k.devices.M2k.getDigital    Method of C++ class libm2k::devices::M2k::getDigital"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getDigitalDefinition, "RetVal", "clib.libm2k.libm2k.digital.M2kDigital", 1);
validate(getDigitalDefinition);

%% C++ class method |getPowerSupply| for C++ class |libm2k::devices::M2k| 
% C++ Signature: libm2k::analog::M2kPowerSupply * libm2k::devices::M2k::getPowerSupply()
getPowerSupplyDefinition = addMethod(M2kDefinition, ...
   "libm2k::analog::M2kPowerSupply * libm2k::devices::M2k::getPowerSupply()", ...
   "Description", "clib.libm2k.libm2k.devices.M2k.getPowerSupply    Method of C++ class libm2k::devices::M2k::getPowerSupply"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getPowerSupplyDefinition, "RetVal", "clib.libm2k.libm2k.analog.M2kPowerSupply", 1);
validate(getPowerSupplyDefinition);

%% C++ class method |getAnalogIn| for C++ class |libm2k::devices::M2k| 
% C++ Signature: libm2k::analog::M2kAnalogIn * libm2k::devices::M2k::getAnalogIn()
getAnalogInDefinition = addMethod(M2kDefinition, ...
   "libm2k::analog::M2kAnalogIn * libm2k::devices::M2k::getAnalogIn()", ...
   "Description", "clib.libm2k.libm2k.devices.M2k.getAnalogIn    Method of C++ class libm2k::devices::M2k::getAnalogIn"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getAnalogInDefinition, "RetVal", "clib.libm2k.libm2k.analog.M2kAnalogIn", 1);
validate(getAnalogInDefinition);

%% C++ class method |getAnalogIn| for C++ class |libm2k::devices::M2k| 
% C++ Signature: libm2k::analog::M2kAnalogIn * libm2k::devices::M2k::getAnalogIn(std::string dev_name)
getAnalogInDefinition = addMethod(M2kDefinition, ...
   "libm2k::analog::M2kAnalogIn * libm2k::devices::M2k::getAnalogIn(std::string dev_name)", ...
   "Description", "clib.libm2k.libm2k.devices.M2k.getAnalogIn    Method of C++ class libm2k::devices::M2k::getAnalogIn"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getAnalogInDefinition, "dev_name", "string");
defineOutput(getAnalogInDefinition, "RetVal", "clib.libm2k.libm2k.analog.M2kAnalogIn", 1);
validate(getAnalogInDefinition);

%% C++ class method |getAnalogOut| for C++ class |libm2k::devices::M2k| 
% C++ Signature: libm2k::analog::M2kAnalogOut * libm2k::devices::M2k::getAnalogOut()
getAnalogOutDefinition = addMethod(M2kDefinition, ...
   "libm2k::analog::M2kAnalogOut * libm2k::devices::M2k::getAnalogOut()", ...
   "Description", "clib.libm2k.libm2k.devices.M2k.getAnalogOut    Method of C++ class libm2k::devices::M2k::getAnalogOut"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getAnalogOutDefinition, "RetVal", "clib.libm2k.libm2k.analog.M2kAnalogOut", 1);
validate(getAnalogOutDefinition);

%% C++ class method |getDacBCalibrationOffset| for C++ class |libm2k::devices::M2k| 
% C++ Signature: int libm2k::devices::M2k::getDacBCalibrationOffset()
getDacBCalibrationOffsetDefinition = addMethod(M2kDefinition, ...
    "int libm2k::devices::M2k::getDacBCalibrationOffset()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.getDacBCalibrationOffset    Method of C++ class libm2k::devices::M2k::getDacBCalibrationOffset"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getDacBCalibrationOffsetDefinition, "RetVal", "int32");
validate(getDacBCalibrationOffsetDefinition);

%% C++ class method |getDacACalibrationOffset| for C++ class |libm2k::devices::M2k| 
% C++ Signature: int libm2k::devices::M2k::getDacACalibrationOffset()
getDacACalibrationOffsetDefinition = addMethod(M2kDefinition, ...
    "int libm2k::devices::M2k::getDacACalibrationOffset()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.getDacACalibrationOffset    Method of C++ class libm2k::devices::M2k::getDacACalibrationOffset"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getDacACalibrationOffsetDefinition, "RetVal", "int32");
validate(getDacACalibrationOffsetDefinition);

%% C++ class method |getDacBCalibrationGain| for C++ class |libm2k::devices::M2k| 
% C++ Signature: double libm2k::devices::M2k::getDacBCalibrationGain()
getDacBCalibrationGainDefinition = addMethod(M2kDefinition, ...
    "double libm2k::devices::M2k::getDacBCalibrationGain()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.getDacBCalibrationGain    Method of C++ class libm2k::devices::M2k::getDacBCalibrationGain"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getDacBCalibrationGainDefinition, "RetVal", "double");
validate(getDacBCalibrationGainDefinition);

%% C++ class method |getDacACalibrationGain| for C++ class |libm2k::devices::M2k| 
% C++ Signature: double libm2k::devices::M2k::getDacACalibrationGain()
getDacACalibrationGainDefinition = addMethod(M2kDefinition, ...
    "double libm2k::devices::M2k::getDacACalibrationGain()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.getDacACalibrationGain    Method of C++ class libm2k::devices::M2k::getDacACalibrationGain"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getDacACalibrationGainDefinition, "RetVal", "double");
validate(getDacACalibrationGainDefinition);

%% C++ class method |getAdcCalibrationOffset| for C++ class |libm2k::devices::M2k| 
% C++ Signature: int libm2k::devices::M2k::getAdcCalibrationOffset(unsigned int chn)
getAdcCalibrationOffsetDefinition = addMethod(M2kDefinition, ...
    "int libm2k::devices::M2k::getAdcCalibrationOffset(unsigned int chn)", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.getAdcCalibrationOffset    Method of C++ class libm2k::devices::M2k::getAdcCalibrationOffset"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getAdcCalibrationOffsetDefinition, "chn", "uint32");
defineOutput(getAdcCalibrationOffsetDefinition, "RetVal", "int32");
validate(getAdcCalibrationOffsetDefinition);

%% C++ class method |getAdcCalibrationGain| for C++ class |libm2k::devices::M2k| 
% C++ Signature: double libm2k::devices::M2k::getAdcCalibrationGain(unsigned int chn)
getAdcCalibrationGainDefinition = addMethod(M2kDefinition, ...
    "double libm2k::devices::M2k::getAdcCalibrationGain(unsigned int chn)", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.getAdcCalibrationGain    Method of C++ class libm2k::devices::M2k::getAdcCalibrationGain"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getAdcCalibrationGainDefinition, "chn", "uint32");
defineOutput(getAdcCalibrationGainDefinition, "RetVal", "double");
validate(getAdcCalibrationGainDefinition);

%% C++ class method |setTimeout| for C++ class |libm2k::devices::M2k| 
% C++ Signature: void libm2k::devices::M2k::setTimeout(unsigned int timeout)
setTimeoutDefinition = addMethod(M2kDefinition, ...
    "void libm2k::devices::M2k::setTimeout(unsigned int timeout)", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.setTimeout    Method of C++ class libm2k::devices::M2k::setTimeout"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setTimeoutDefinition, "timeout", "uint32");
validate(setTimeoutDefinition);

%% C++ class method |setLed| for C++ class |libm2k::devices::M2k| 
% C++ Signature: void libm2k::devices::M2k::setLed(bool on)
setLedDefinition = addMethod(M2kDefinition, ...
    "void libm2k::devices::M2k::setLed(bool on)", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.setLed    Method of C++ class libm2k::devices::M2k::setLed"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(setLedDefinition, "on", "logical");
validate(setLedDefinition);

%% C++ class method |getLed| for C++ class |libm2k::devices::M2k| 
% C++ Signature: bool libm2k::devices::M2k::getLed()
getLedDefinition = addMethod(M2kDefinition, ...
    "bool libm2k::devices::M2k::getLed()", ...
    "Description", "clib.libm2k.libm2k.devices.M2k.getLed    Method of C++ class libm2k::devices::M2k::getLed"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getLedDefinition, "RetVal", "logical");
validate(getLedDefinition);

%% C++ class constructor for C++ class |libm2k::devices::M2k| 
% C++ Signature: libm2k::devices::M2k::M2k(libm2k::devices::M2k const & input1)
M2kConstructor2Definition = addConstructor(M2kDefinition, ...
    "libm2k::devices::M2k::M2k(libm2k::devices::M2k const & input1)", ...
    "Description", "clib.libm2k.libm2k.devices.M2k    Constructor of C++ class libm2k::devices::M2k"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(M2kConstructor2Definition, "input1", "clib.libm2k.libm2k.devices.M2k", "input");
validate(M2kConstructor2Definition);

%% C++ class |libm2k::devices::Context| with MATLAB name |clib.libm2k.libm2k.devices.Context| 
ContextDefinition = addClass(libDef, "libm2k::devices::Context", "MATLABName", "clib.libm2k.libm2k.devices.Context", ...
    "Description", "clib.libm2k.libm2k.devices.Context    Representation of C++ class libm2k::devices::Context"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::devices::Context| 
% C++ Signature: libm2k::devices::Context::Context(std::string uri,iio_context * input1,std::string name,bool sync)
%ContextConstructor1Definition = addConstructor(ContextDefinition, ...
%    "libm2k::devices::Context::Context(std::string uri,iio_context * input1,std::string name,bool sync)", ...
%    "Description", "clib.libm2k.libm2k.devices.Context    Constructor of C++ class libm2k::devices::Context"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(ContextConstructor1Definition, "uri", "string");
%defineArgument(ContextConstructor1Definition, "input2", "clib.libm2k.iio_context", "input", <SHAPE>);
%defineArgument(ContextConstructor1Definition, "name", "string");
%defineArgument(ContextConstructor1Definition, "sync", "logical");
%validate(ContextConstructor1Definition);

%% C++ class method |init| for C++ class |libm2k::devices::Context| 
% C++ Signature: void libm2k::devices::Context::init()
initDefinition = addMethod(ContextDefinition, ...
    "void libm2k::devices::Context::init()", ...
    "Description", "clib.libm2k.libm2k.devices.Context.init    Method of C++ class libm2k::devices::Context::init"); % This description is shown as help to user. Modify it to appropriate description.
validate(initDefinition);

%% C++ class method |deinitialize| for C++ class |libm2k::devices::Context| 
% C++ Signature: void libm2k::devices::Context::deinitialize()
deinitializeDefinition = addMethod(ContextDefinition, ...
    "void libm2k::devices::Context::deinitialize()", ...
    "Description", "clib.libm2k.libm2k.devices.Context.deinitialize    Method of C++ class libm2k::devices::Context::deinitialize"); % This description is shown as help to user. Modify it to appropriate description.
validate(deinitializeDefinition);

%% C++ class method |scanAllPowerSupply| for C++ class |libm2k::devices::Context| 
% C++ Signature: void libm2k::devices::Context::scanAllPowerSupply()
scanAllPowerSupplyDefinition = addMethod(ContextDefinition, ...
    "void libm2k::devices::Context::scanAllPowerSupply()", ...
    "Description", "clib.libm2k.libm2k.devices.Context.scanAllPowerSupply    Method of C++ class libm2k::devices::Context::scanAllPowerSupply"); % This description is shown as help to user. Modify it to appropriate description.
validate(scanAllPowerSupplyDefinition);

%% C++ class method |scanAllDigital| for C++ class |libm2k::devices::Context| 
% C++ Signature: void libm2k::devices::Context::scanAllDigital()
scanAllDigitalDefinition = addMethod(ContextDefinition, ...
    "void libm2k::devices::Context::scanAllDigital()", ...
    "Description", "clib.libm2k.libm2k.devices.Context.scanAllDigital    Method of C++ class libm2k::devices::Context::scanAllDigital"); % This description is shown as help to user. Modify it to appropriate description.
validate(scanAllDigitalDefinition);

%% C++ class method |scanAllDMM| for C++ class |libm2k::devices::Context| 
% C++ Signature: void libm2k::devices::Context::scanAllDMM()
scanAllDMMDefinition = addMethod(ContextDefinition, ...
    "void libm2k::devices::Context::scanAllDMM()", ...
    "Description", "clib.libm2k.libm2k.devices.Context.scanAllDMM    Method of C++ class libm2k::devices::Context::scanAllDMM"); % This description is shown as help to user. Modify it to appropriate description.
validate(scanAllDMMDefinition);

%% C++ class method |getUri| for C++ class |libm2k::devices::Context| 
% C++ Signature: std::string libm2k::devices::Context::getUri()
getUriDefinition = addMethod(ContextDefinition, ...
    "std::string libm2k::devices::Context::getUri()", ...
    "Description", "clib.libm2k.libm2k.devices.Context.getUri    Method of C++ class libm2k::devices::Context::getUri"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getUriDefinition, "RetVal", "string");
validate(getUriDefinition);

%% C++ class method |getAnalogIn| for C++ class |libm2k::devices::Context| 
% C++ Signature: libm2k::analog::GenericAnalogIn * libm2k::devices::Context::getAnalogIn(unsigned int input1)
%getAnalogInDefinition = addMethod(ContextDefinition, ...
%    "libm2k::analog::GenericAnalogIn * libm2k::devices::Context::getAnalogIn(unsigned int input1)", ...
%    "Description", "clib.libm2k.libm2k.devices.Context.getAnalogIn    Method of C++ class libm2k::devices::Context::getAnalogIn"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(getAnalogInDefinition, "input1", "uint32");
%defineOutput(getAnalogInDefinition, "RetVal", "clib.libm2k.libm2k.analog.GenericAnalogIn", <SHAPE>);
%validate(getAnalogInDefinition);

%% C++ class method |getAnalogIn| for C++ class |libm2k::devices::Context| 
% C++ Signature: libm2k::analog::GenericAnalogIn * libm2k::devices::Context::getAnalogIn(std::string input1)
%getAnalogInDefinition = addMethod(ContextDefinition, ...
%    "libm2k::analog::GenericAnalogIn * libm2k::devices::Context::getAnalogIn(std::string input1)", ...
%    "Description", "clib.libm2k.libm2k.devices.Context.getAnalogIn    Method of C++ class libm2k::devices::Context::getAnalogIn"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(getAnalogInDefinition, "input1", "string");
%defineOutput(getAnalogInDefinition, "RetVal", "clib.libm2k.libm2k.analog.GenericAnalogIn", <SHAPE>);
%validate(getAnalogInDefinition);

%% C++ class method |getAnalogOut| for C++ class |libm2k::devices::Context| 
% C++ Signature: libm2k::analog::GenericAnalogOut * libm2k::devices::Context::getAnalogOut(unsigned int input1)
%getAnalogOutDefinition = addMethod(ContextDefinition, ...
%    "libm2k::analog::GenericAnalogOut * libm2k::devices::Context::getAnalogOut(unsigned int input1)", ...
%    "Description", "clib.libm2k.libm2k.devices.Context.getAnalogOut    Method of C++ class libm2k::devices::Context::getAnalogOut"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(getAnalogOutDefinition, "input1", "uint32");
%defineOutput(getAnalogOutDefinition, "RetVal", "clib.libm2k.libm2k.analog.GenericAnalogOut", <SHAPE>);
%validate(getAnalogOutDefinition);

%% C++ class method |getAnalogOut| for C++ class |libm2k::devices::Context| 
% C++ Signature: libm2k::analog::GenericAnalogOut * libm2k::devices::Context::getAnalogOut(std::string input1)
%getAnalogOutDefinition = addMethod(ContextDefinition, ...
%    "libm2k::analog::GenericAnalogOut * libm2k::devices::Context::getAnalogOut(std::string input1)", ...
%    "Description", "clib.libm2k.libm2k.devices.Context.getAnalogOut    Method of C++ class libm2k::devices::Context::getAnalogOut"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(getAnalogOutDefinition, "input1", "string");
%defineOutput(getAnalogOutDefinition, "RetVal", "clib.libm2k.libm2k.analog.GenericAnalogOut", <SHAPE>);
%validate(getAnalogOutDefinition);

%% C++ class method |getDMM| for C++ class |libm2k::devices::Context| 
% C++ Signature: libm2k::analog::DMM * libm2k::devices::Context::getDMM(unsigned int input1)
getDMMDefinition = addMethod(ContextDefinition, ...
   "libm2k::analog::DMM * libm2k::devices::Context::getDMM(unsigned int input1)", ...
   "Description", "clib.libm2k.libm2k.devices.Context.getDMM    Method of C++ class libm2k::devices::Context::getDMM"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getDMMDefinition, "input1", "uint32");
defineOutput(getDMMDefinition, "RetVal", "clib.libm2k.libm2k.analog.DMM", 1);
validate(getDMMDefinition);

%% C++ class method |getDMM| for C++ class |libm2k::devices::Context| 
% C++ Signature: libm2k::analog::DMM * libm2k::devices::Context::getDMM(std::string input1)
getDMMDefinition = addMethod(ContextDefinition, ...
   "libm2k::analog::DMM * libm2k::devices::Context::getDMM(std::string input1)", ...
   "Description", "clib.libm2k.libm2k.devices.Context.getDMM    Method of C++ class libm2k::devices::Context::getDMM"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getDMMDefinition, "input1", "string");
defineOutput(getDMMDefinition, "RetVal", "clib.libm2k.libm2k.analog.DMM", 1);
validate(getDMMDefinition);

%% C++ class method |getContextAttributeValue| for C++ class |libm2k::devices::Context| 
% C++ Signature: std::string libm2k::devices::Context::getContextAttributeValue(std::string attr)
getContextAttributeValueDefinition = addMethod(ContextDefinition, ...
    "std::string libm2k::devices::Context::getContextAttributeValue(std::string attr)", ...
    "Description", "clib.libm2k.libm2k.devices.Context.getContextAttributeValue    Method of C++ class libm2k::devices::Context::getContextAttributeValue"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(getContextAttributeValueDefinition, "attr", "string");
defineOutput(getContextAttributeValueDefinition, "RetVal", "string");
validate(getContextAttributeValueDefinition);

%% C++ class method |getContextDescription| for C++ class |libm2k::devices::Context| 
% C++ Signature: std::string libm2k::devices::Context::getContextDescription()
getContextDescriptionDefinition = addMethod(ContextDefinition, ...
    "std::string libm2k::devices::Context::getContextDescription()", ...
    "Description", "clib.libm2k.libm2k.devices.Context.getContextDescription    Method of C++ class libm2k::devices::Context::getContextDescription"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getContextDescriptionDefinition, "RetVal", "string");
validate(getContextDescriptionDefinition);

%% C++ class method |getSerialNumber| for C++ class |libm2k::devices::Context| 
% C++ Signature: std::string libm2k::devices::Context::getSerialNumber()
getSerialNumberDefinition = addMethod(ContextDefinition, ...
    "std::string libm2k::devices::Context::getSerialNumber()", ...
    "Description", "clib.libm2k.libm2k.devices.Context.getSerialNumber    Method of C++ class libm2k::devices::Context::getSerialNumber"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(getSerialNumberDefinition, "RetVal", "string");
validate(getSerialNumberDefinition);

%% C++ class method |toM2k| for C++ class |libm2k::devices::Context| 
% C++ Signature: libm2k::devices::M2k * libm2k::devices::Context::toM2k()
%toM2kDefinition = addMethod(ContextDefinition, ...
%    "libm2k::devices::M2k * libm2k::devices::Context::toM2k()", ...
%    "Description", "clib.libm2k.libm2k.devices.Context.toM2k    Method of C++ class libm2k::devices::Context::toM2k"); % This description is shown as help to user. Modify it to appropriate description.
%defineOutput(toM2kDefinition, "RetVal", "clib.libm2k.libm2k.devices.M2k", <SHAPE>);
%validate(toM2kDefinition);

%% C++ class method |iioChannelHasAttribute| for C++ class |libm2k::devices::Context| 
% C++ Signature: bool libm2k::devices::Context::iioChannelHasAttribute(iio_channel * chn,std::string const & attr)
%iioChannelHasAttributeDefinition = addMethod(ContextDefinition, ...
%    "bool libm2k::devices::Context::iioChannelHasAttribute(iio_channel * chn,std::string const & attr)", ...
%    "Description", "clib.libm2k.libm2k.devices.Context.iioChannelHasAttribute    Method of C++ class libm2k::devices::Context::iioChannelHasAttribute"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iioChannelHasAttributeDefinition, "chn", "clib.libm2k.iio_channel", "input", <SHAPE>);
%defineArgument(iioChannelHasAttributeDefinition, "attr", "string", "input");
%defineOutput(iioChannelHasAttributeDefinition, "RetVal", "logical");
%validate(iioChannelHasAttributeDefinition);

%% C++ class method |iioDevHasAttribute| for C++ class |libm2k::devices::Context| 
% C++ Signature: bool libm2k::devices::Context::iioDevHasAttribute(iio_device * dev,std::string const & attr)
%iioDevHasAttributeDefinition = addMethod(ContextDefinition, ...
%    "bool libm2k::devices::Context::iioDevHasAttribute(iio_device * dev,std::string const & attr)", ...
%    "Description", "clib.libm2k.libm2k.devices.Context.iioDevHasAttribute    Method of C++ class libm2k::devices::Context::iioDevHasAttribute"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(iioDevHasAttributeDefinition, "dev", "clib.libm2k.iio_device", "input", <SHAPE>);
%defineArgument(iioDevHasAttributeDefinition, "attr", "string", "input");
%defineOutput(iioDevHasAttributeDefinition, "RetVal", "logical");
%validate(iioDevHasAttributeDefinition);

%% C++ class constructor for C++ class |libm2k::devices::Context| 
% C++ Signature: libm2k::devices::Context::Context(libm2k::devices::Context const & input1)
ContextConstructor2Definition = addConstructor(ContextDefinition, ...
    "libm2k::devices::Context::Context(libm2k::devices::Context const & input1)", ...
    "Description", "clib.libm2k.libm2k.devices.Context    Constructor of C++ class libm2k::devices::Context"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(ContextConstructor2Definition, "input1", "clib.libm2k.libm2k.devices.Context", "input");
validate(ContextConstructor2Definition);

%% C++ class |libm2k::devices::ContextBuilder| with MATLAB name |clib.libm2k.libm2k.devices.ContextBuilder| 
ContextBuilderDefinition = addClass(libDef, "libm2k::devices::ContextBuilder", "MATLABName", "clib.libm2k.libm2k.devices.ContextBuilder", ...
    "Description", "clib.libm2k.libm2k.devices.ContextBuilder    Representation of C++ class libm2k::devices::ContextBuilder"); % This description is shown as help to user. Modify it to appropriate description.

%% C++ class constructor for C++ class |libm2k::devices::ContextBuilder| 
% C++ Signature: libm2k::devices::ContextBuilder::ContextBuilder()
ContextBuilderConstructor1Definition = addConstructor(ContextBuilderDefinition, ...
    "libm2k::devices::ContextBuilder::ContextBuilder()", ...
    "Description", "clib.libm2k.libm2k.devices.ContextBuilder    Constructor of C++ class libm2k::devices::ContextBuilder"); % This description is shown as help to user. Modify it to appropriate description.
validate(ContextBuilderConstructor1Definition);

%% C++ class method |deviceOpen| for C++ class |libm2k::devices::ContextBuilder| 
% C++ Signature: libm2k::devices::Context * libm2k::devices::ContextBuilder::deviceOpen(char const * input1)
%deviceOpenDefinition = addMethod(ContextBuilderDefinition, ...
%    "libm2k::devices::Context * libm2k::devices::ContextBuilder::deviceOpen(char const * input1)", ...
%    "Description", "clib.libm2k.libm2k.devices.ContextBuilder.deviceOpen    Method of C++ class libm2k::devices::ContextBuilder::deviceOpen"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(deviceOpenDefinition, "input1", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(deviceOpenDefinition, "RetVal", "clib.libm2k.libm2k.devices.Context", <SHAPE>);
%validate(deviceOpenDefinition);

%% C++ class method |deviceOpen| for C++ class |libm2k::devices::ContextBuilder| 
% C++ Signature: libm2k::devices::Context * libm2k::devices::ContextBuilder::deviceOpen()
deviceOpenDefinition = addMethod(ContextBuilderDefinition, ...
   "libm2k::devices::Context * libm2k::devices::ContextBuilder::deviceOpen()", ...
   "Description", "clib.libm2k.libm2k.devices.ContextBuilder.deviceOpen    Method of C++ class libm2k::devices::ContextBuilder::deviceOpen"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(deviceOpenDefinition, "RetVal", "clib.libm2k.libm2k.devices.Context", 1);
validate(deviceOpenDefinition);

%% C++ class method |m2kOpen| for C++ class |libm2k::devices::ContextBuilder| 
% C++ Signature: libm2k::devices::M2k * libm2k::devices::ContextBuilder::m2kOpen(char const * input1)
%m2kOpenDefinition = addMethod(ContextBuilderDefinition, ...
%    "libm2k::devices::M2k * libm2k::devices::ContextBuilder::m2kOpen(char const * input1)", ...
%    "Description", "clib.libm2k.libm2k.devices.ContextBuilder.m2kOpen    Method of C++ class libm2k::devices::ContextBuilder::m2kOpen"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(m2kOpenDefinition, "input1", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(m2kOpenDefinition, "RetVal", "clib.libm2k.libm2k.devices.M2k", <SHAPE>);
%validate(m2kOpenDefinition);

%% C++ class method |m2kOpen| for C++ class |libm2k::devices::ContextBuilder| 
% C++ Signature: libm2k::devices::M2k * libm2k::devices::ContextBuilder::m2kOpen()
m2kOpenDefinition = addMethod(ContextBuilderDefinition, ...
   "libm2k::devices::M2k * libm2k::devices::ContextBuilder::m2kOpen()", ...
   "Description", "clib.libm2k.libm2k.devices.ContextBuilder.m2kOpen    Method of C++ class libm2k::devices::ContextBuilder::m2kOpen"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(m2kOpenDefinition, "RetVal", "clib.libm2k.libm2k.devices.M2k", 1);
validate(m2kOpenDefinition);

%% C++ class method |deviceClose| for C++ class |libm2k::devices::ContextBuilder| 
% C++ Signature: void libm2k::devices::ContextBuilder::deviceClose(libm2k::devices::Context * input1,bool deinit)
%deviceCloseDefinition = addMethod(ContextBuilderDefinition, ...
%    "void libm2k::devices::ContextBuilder::deviceClose(libm2k::devices::Context * input1,bool deinit)", ...
%    "Description", "clib.libm2k.libm2k.devices.ContextBuilder.deviceClose    Method of C++ class libm2k::devices::ContextBuilder::deviceClose"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(deviceCloseDefinition, "input1", "clib.libm2k.libm2k.devices.Context", "input", <SHAPE>);
%defineArgument(deviceCloseDefinition, "deinit", "logical");
%validate(deviceCloseDefinition);

%% C++ class method |deviceCloseAll| for C++ class |libm2k::devices::ContextBuilder| 
% C++ Signature: void libm2k::devices::ContextBuilder::deviceCloseAll()
deviceCloseAllDefinition = addMethod(ContextBuilderDefinition, ...
    "void libm2k::devices::ContextBuilder::deviceCloseAll()", ...
    "Description", "clib.libm2k.libm2k.devices.ContextBuilder.deviceCloseAll    Method of C++ class libm2k::devices::ContextBuilder::deviceCloseAll"); % This description is shown as help to user. Modify it to appropriate description.
validate(deviceCloseAllDefinition);

%% C++ class constructor for C++ class |libm2k::devices::ContextBuilder| 
% C++ Signature: libm2k::devices::ContextBuilder::ContextBuilder(libm2k::devices::ContextBuilder const & input1)
ContextBuilderConstructor2Definition = addConstructor(ContextBuilderDefinition, ...
    "libm2k::devices::ContextBuilder::ContextBuilder(libm2k::devices::ContextBuilder const & input1)", ...
    "Description", "clib.libm2k.libm2k.devices.ContextBuilder    Constructor of C++ class libm2k::devices::ContextBuilder"); % This description is shown as help to user. Modify it to appropriate description.
defineArgument(ContextBuilderConstructor2Definition, "input1", "clib.libm2k.libm2k.devices.ContextBuilder", "input");
validate(ContextBuilderConstructor2Definition);

%% C++ function |libm2k::devices::deviceOpen| with MATLAB name |clib.libm2k.libm2k.devices.deviceOpen|
% C++ Signature: libm2k::devices::Context * libm2k::devices::deviceOpen()
%deviceOpenDefinition = addFunction(libDef, ...
%    "libm2k::devices::Context * libm2k::devices::deviceOpen()", ...
%    "MATLABName", "clib.libm2k.libm2k.devices.deviceOpen", ...
%    "Description", "clib.libm2k.libm2k.devices.deviceOpen    Representation of C++ function libm2k::devices::deviceOpen"); % This description is shown as help to user. Modify it to appropriate description.
%defineOutput(deviceOpenDefinition, "RetVal", "clib.libm2k.libm2k.devices.Context", <SHAPE>);
%validate(deviceOpenDefinition);

%% C++ function |libm2k::devices::deviceOpen| with MATLAB name |clib.libm2k.libm2k.devices.deviceOpen|
% C++ Signature: libm2k::devices::Context * libm2k::devices::deviceOpen(char const * uri)
%deviceOpenDefinition = addFunction(libDef, ...
%    "libm2k::devices::Context * libm2k::devices::deviceOpen(char const * uri)", ...
%    "MATLABName", "clib.libm2k.libm2k.devices.deviceOpen", ...
%    "Description", "clib.libm2k.libm2k.devices.deviceOpen    Representation of C++ function libm2k::devices::deviceOpen"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(deviceOpenDefinition, "uri", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(deviceOpenDefinition, "RetVal", "clib.libm2k.libm2k.devices.Context", <SHAPE>);
%validate(deviceOpenDefinition);

%% C++ function |libm2k::devices::m2kOpen| with MATLAB name |clib.libm2k.libm2k.devices.m2kOpen|
% C++ Signature: libm2k::devices::M2k * libm2k::devices::m2kOpen(char const * uri)
%m2kOpenDefinition = addFunction(libDef, ...
%    "libm2k::devices::M2k * libm2k::devices::m2kOpen(char const * uri)", ...
%    "MATLABName", "clib.libm2k.libm2k.devices.m2kOpen", ...
%    "Description", "clib.libm2k.libm2k.devices.m2kOpen    Representation of C++ function libm2k::devices::m2kOpen"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(m2kOpenDefinition, "uri", <MLTYPE>, "input", <SHAPE>); % '<MLTYPE>' can be int8,string, or char
%defineOutput(m2kOpenDefinition, "RetVal", "clib.libm2k.libm2k.devices.M2k", <SHAPE>);
%validate(m2kOpenDefinition);

%% C++ function |libm2k::devices::m2kOpen| with MATLAB name |clib.libm2k.libm2k.devices.m2kOpen|
% C++ Signature: libm2k::devices::M2k * libm2k::devices::m2kOpen()
m2kOpenDefinition = addFunction(libDef, ...
   "libm2k::devices::M2k * libm2k::devices::m2kOpen()", ...
   "MATLABName", "clib.libm2k.libm2k.devices.m2kOpen", ...
   "Description", "clib.libm2k.libm2k.devices.m2kOpen    Representation of C++ function libm2k::devices::m2kOpen"); % This description is shown as help to user. Modify it to appropriate description.
defineOutput(m2kOpenDefinition, "RetVal", "clib.libm2k.libm2k.devices.M2k", 1);
validate(m2kOpenDefinition);

%% C++ function |libm2k::devices::deviceClose| with MATLAB name |clib.libm2k.libm2k.devices.deviceClose|
% C++ Signature: void libm2k::devices::deviceClose(libm2k::devices::Context * ctx,bool deinit)
%deviceCloseDefinition = addFunction(libDef, ...
%    "void libm2k::devices::deviceClose(libm2k::devices::Context * ctx,bool deinit)", ...
%    "MATLABName", "clib.libm2k.libm2k.devices.deviceClose", ...
%    "Description", "clib.libm2k.libm2k.devices.deviceClose    Representation of C++ function libm2k::devices::deviceClose"); % This description is shown as help to user. Modify it to appropriate description.
%defineArgument(deviceCloseDefinition, "ctx", "clib.libm2k.libm2k.devices.Context", "input", <SHAPE>);
%defineArgument(deviceCloseDefinition, "deinit", "logical");
%validate(deviceCloseDefinition);

%% C++ function |libm2k::devices::deviceCloseAll| with MATLAB name |clib.libm2k.libm2k.devices.deviceCloseAll|
% C++ Signature: void libm2k::devices::deviceCloseAll()
deviceCloseAllDefinition = addFunction(libDef, ...
    "void libm2k::devices::deviceCloseAll()", ...
    "MATLABName", "clib.libm2k.libm2k.devices.deviceCloseAll", ...
    "Description", "clib.libm2k.libm2k.devices.deviceCloseAll    Representation of C++ function libm2k::devices::deviceCloseAll"); % This description is shown as help to user. Modify it to appropriate description.
validate(deviceCloseAllDefinition);

%% Validate the library definition
validate(libDef);

end
