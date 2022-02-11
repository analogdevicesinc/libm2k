#!/bin/bash

set -e
DEST_LIBIIO="/c/libiio"
SRC_LIBIIO="/c/projects/libiio"
DEPS_LIBIIO="/c/libs"
TOP_DIR=$(pwd)


__build_libiio() {
	local PLATFORM="$1"
	local VERSION="$2"
	local CONFIGURATION="$3"
	local GENERATOR="$4"
	local ARCH="$5"

	# Create the build directory for this platform
	mkdir -p "$SRC_LIBIIO/build-$PLATFORM"
	cd "$SRC_LIBIIO/build-$PLATFORM"

	cmake -G "$GENERATOR" -A "$ARCH" -DCMAKE_CONFIGURATION_TYPES="$CONFIGURATION" -DWITH_TESTS=OFF -DENABLE_IPV6:BOOL=OFF -DCMAKE_SYSTEM_PREFIX_PATH="C:" -DCSHARP_BINDINGS:BOOL=OFF -DPYTHON_BINDINGS:BOOL=OFF -DLIBXML2_LIBRARIES="$DEPS_LIBIIO/$VERSION/libxml2.lib" -DLIBUSB_LIBRARIES="$DEPS_LIBIIO/$VERSION/libusb-1.0.lib" ..
	cmake --build . --config "$CONFIGURATION"

	__mv_to_build_dir $PLATFORM $VERSION $CONFIGURATION
}

__mv_to_build_dir() {
	local PLATFORM="$1"
	local VERSION="$2"
	local CONFIGURATION="$3"

	DST_FOLDER="$DEST_LIBIIO-$PLATFORM/"
	mkdir -p "$DST_FOLDER"
	cd "$SRC_LIBIIO/build-$PLATFORM"
	cp $CONFIGURATION/*.dll $DST_FOLDER
	cp $CONFIGURATION/*.lib $DST_FOLDER
	cp *.iss $DST_FOLDER
	cp $DEPS_LIBIIO/$VERSION/*.dll $DST_FOLDER
	cd ..
	cp iio.h $DST_FOLDER
}

git clone https://github.com/analogdevicesinc/libiio "$SRC_LIBIIO"
cd "$SRC_LIBIIO"
git checkout 0939f127ca5fa431df6d88758796c0cb1590174f
git submodule update --init

if [[ "$APPVEYOR_BUILD_WORKER_IMAGE" == "Visual Studio 2015" ]]; then
    generator="Visual Studio 14 2015"
elif [[ "$APPVEYOR_BUILD_WORKER_IMAGE" == "Visual Studio 2017" ]]; then
    generator="Visual Studio 15 2017"
elif [[ "$APPVEYOR_BUILD_WORKER_IMAGE" == "Visual Studio 2019" ]]; then
    generator="Visual Studio 16 2019"
fi

__build_libiio win64 64 Release "$generator" x64
__build_libiio win32 32 Release "$generator" Win32

