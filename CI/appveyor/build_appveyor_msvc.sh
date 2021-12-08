#!/bin/bash

set -e
MCS_EXECUTABLE_PATH="C:\Windows\Microsoft.NET\Framework\v4.0.30319"
OLD_PATH="$PATH"
DEST_LIBIIO="/c/libiio"
TOP_DIR="/c/projects/libm2k"

__build_python_wheel() {
	local PLATFORM="$1"
	local PY_PATH="$2"
	export PATH="/c/swig/;/c/swig/Lib/;$OLD_PATH"
	"$PY_PATH" -m pip install --user --upgrade pip setuptools wheel twine build virtualenv
	export COMPILE_BINDINGS=True
	"$PY_PATH" -m build
	export COMPILE_BINDINGS=
	cp dist/libm2k*.whl "${TOP_DIR}/build-$PLATFORM/dist/"
}

__build_libm2k() {
	local PLATFORM="$1"
	local PY_VERSION="$2"
	local PY_PATH="$3"
	local GENERATOR="$4"
	local ARCH="$5"
	local PLAT_NAME="${6:-$PLATFORM}"

	# Create the official build directory for this platform
	mkdir -p "${TOP_DIR}/build-$PLATFORM/dist"

	# Create and clear up the temporary build directory for this platform
	rm -rf "${TOP_DIR}/tmp-build-$PLATFORM"
	mkdir -p "${TOP_DIR}/tmp-build-$PLATFORM"
	export PATH="$PY_PATH;$PY_PATH/libs;$OLD_PATH"
	cd ${TOP_DIR}/tmp-build-"$PLATFORM"
	cmake -G "$GENERATOR" \
	-A "$ARCH" \
	-DIIO_LIBRARIES:FILEPATH="$DEST_LIBIIO"-"$PLATFORM"/libiio.lib \
	-DIIO_INCLUDE_DIRS:PATH="$DEST_LIBIIO"-"$PLATFORM" \
        -DCMAKE_CONFIGURATION_TYPES=RELEASE \
        -DSWIG_DIR=/c/swig/Lib \
        -DSWIG_EXECUTABLE=/c/swig/swig.exe \
        -DSWIG_VERSION="4.0.0" \
        -DENABLE_TOOLS=ON \
        -DENABLE_LOG=ON \
        -DPython_EXECUTABLE="$PY_PATH/python.exe" \
        -DBUILD_EXAMPLES=ON \
        -DENABLE_CSHARP=ON \
	-DENABLE_LABVIEW=ON \
        ..
	cmake --build . --config Release

	cat setup.py

	PY_SUFFIX=""
	if [[ "$PLATFORM" == "win64" ]]; then
		PY_SUFFIX="-x64"
	fi
	__build_python_wheel "$PLATFORM" "/c/Python37""$PY_SUFFIX""/python.exe"
	__build_python_wheel "$PLATFORM" "/c/Python38""$PY_SUFFIX""/python.exe"
	__build_python_wheel "$PLATFORM" "/c/Python39""$PY_SUFFIX""/python.exe"
	__build_python_wheel "$PLATFORM" "/c/Python310""$PY_SUFFIX""/python.exe"
}

__mv_to_build_dir() {
	local PLATFORM="$1"

	DST_FOLDER="${TOP_DIR}/build-$PLATFORM/"
	cd "${TOP_DIR}/tmp-build-$PLATFORM"
	cp *.dll $DST_FOLDER
	cp *.exe $DST_FOLDER
	cp *.lib $DST_FOLDER
	cp *.iss $DST_FOLDER
	cd ..
}

if [[ "$APPVEYOR_BUILD_WORKER_IMAGE" == "Visual Studio 2015" ]]; then
    generator="Visual Studio 14 2015"
elif [[ "$APPVEYOR_BUILD_WORKER_IMAGE" == "Visual Studio 2017" ]]; then
    generator="Visual Studio 15 2017"
elif [[ "$APPVEYOR_BUILD_WORKER_IMAGE" == "Visual Studio 2019" ]]; then
    generator="Visual Studio 16 2019"
fi

__build_libm2k win32 37 "/c/Python39" "$generator" Win32
__mv_to_build_dir win32

__build_libm2k win64 37 "/c/Python39-x64" "$generator" x64 "win_amd64"
__mv_to_build_dir win64
