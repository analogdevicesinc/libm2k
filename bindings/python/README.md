![libm2k Python logo](/doc/img/libm2k-python_logo.png)

# libm2k : Python bindings

This package contains the python bindings for libm2k.
libm2k is a C++ library for interfacing with the ADALM2000, splitted into more correlated components, interconnected by a context.

[[Docs](https://analogdevicesinc.github.io/libm2k/python/html/sphinx/build/html/index.html)]
[[Support](https://ez.analog.com/adieducation/university-program)]
[[Github](https://github.com/analogdevicesinc/libm2k/issues)]
[[Wiki](https://wiki.analog.com/libm2k)]

## Requirements
To use these bindings you need the core C++ library they depend upon. This is not packaged with the pypi release but you can install the [latest release](https://github.com/analogdevicesinc/libm2k/releases/latest) or the latest **untested** binaries from the [![default branch](https://dev.azure.com/AnalogDevices/M2k/_apis/build/status/analogdevicesinc.libm2k?branchName=main)](https://dev.azure.com/AnalogDevices/M2k/_build/latest?definitionId=17&branchName=main)

### Installing
You can install these bindings using pip, if you already have the library installed:
```shell
(sudo) pip install libm2k
```

If you want to build them manually, please check the [build guide](https://wiki.analog.com/university/tools/m2k/libm2k/libm2k#building_bindings) for you specific operating system.

