# libm2k : cmake

#### Overview
This folder contains helper cmake.in files for the project and also config files for other cmake projects to use in order to find libm2k.

#### Using the CMake build system
In order to build libm2k with cmake, we first need to have the only required dependency installed: libiio.

After creating a build folder and moving into it `mkdir -p build && cd build`, we can run cmake.

`cmake -D<option> <path_to_libm2k>`

which will generate all the necessary recipes for building and installing. Useful cmake options are:

| \<option\> | value | default | description |
| --------- | ----------- | ----------- | ----------- |
| ENABLE_DOC | on/off | off | Generate doc using Doxygen |
| BUILD_EXAMPLES | on/off | off | Build the default examples |
| ENABLE_LOG | on/off | off | Build libm2k with logging support |
| ENABLE_EXCEPTIONS | on/off | on | Build with exception handling support |
| ENABLE_PYTHON | on/off | on | Build the Python bindings |
| ENABLE_CSHARP | on/off | off | Build the C# bindings |
| ENABLE_TOOLS | on/off | off | Build the tools |
| INSTALL_UDEV_RULES | on/off | on | Install udev rles for the M2K |
| CMAKE_PREFIX_PATH | \<path\> | Empty | Specifies a path which will be used by the FIND_XXX() commands |
| CMAKE_INSTALL_PREFIX | \<path\> |  /usr/local on UNIX, c:/Program Files on Windows | Installation directory used by `cmake install` |

#### Building and Installing 

To build the sdk the following command is used:

`cmake --build . [--config <config>] [--target <target>]`

Where `<config>` is the build type: `Debug, Release ...` and target is one of the following:

| \<target\> | description |
| --------- | ----------- |
| install | Install the SDK in the system |
| doc | Build the doxygen documentation |

Example: Consider a user that wants to install libm2k in `/opt/libm2k`, without bindings. The following set of commands will do:
```
cd libm2k
mkdir build && cd build
cmake -DENABLE_PYTHON=off -DENABLE_CSHARP=off -DCMAKE_INSTALL_PREFIX="/opt/libm2k" ..
[sudo] cmake --build . --target install
```

##### Note:
1. `sudo` is required when installing the library only if the install prefix is a location where the user does not have rights to it.
2. After installing you should run `ldconfig` to update the links/cache that the dynamic loader uses.

#### Consuming libm2k in a CMake Project
To use libm2k in your own project, simply add this two lines to your projects CMakeLists.txt and your good to go:
```
find_package(libm2k <version> REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE libm2k::libm2k)
```
Compile definitions of libm2k will automatically be added to your target as needed.

Example: if `ENABLE_LOG` is `on` your target will be aware of the compile definition `LIBM2K_ENABLE_LOG` 

The prerequisite for this to work is to have libm2k installed in the system. If the path for libm2k is not in the default `PATH` you might need to add a `CMAKE_PREFIX_PATH` that points to libm2k when running the cmake command for your project.

Example: Consider a user that has libm2k installed in `/opt/libm2k` and wants to use it with another project. In the CMakeLists.txt of the project the two lines from above are required to be added and the cmake command should specify

```
-DCMAKE_PREFIX_PATH="/opt/libm2k"
```
