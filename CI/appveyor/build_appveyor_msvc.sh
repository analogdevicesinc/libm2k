#!/bin/bash

set -e
MCS_EXECUTABLE_PATH="C:\Windows\Microsoft.NET\Framework\v4.0.30319"
OLD_PATH="$PATH"

__build_libm2k() {
	local PLATFORM="$1"
	local PY_VERSION="$2"
	local PY_PATH="$3"
	local GENERATOR="$4"
	local PLAT_NAME="${5:-$PLATFORM}"

	# Create the official build directory for this platform
	mkdir -p "/c/projects/libm2k/build-$PLATFORM/dist"

	# Create and clear up the temporary build directory for this platform
	rm -rf "/c/projects/libm2k/tmp-build-$PLATFORM"
	mkdir -p "/c/projects/libm2k/tmp-build-$PLATFORM"
	export PATH="$PY_PATH;$PY_PATH/libs;$OLD_PATH"
	cd /c/projects/libm2k/tmp-build-"$PLATFORM"
	cmake -G "$GENERATOR" \
        -DIIO_LIBRARIES:FILEPATH=/c/libiio-"$PLATFORM"/libiio.lib \
        -DIIO_INCLUDE_DIRS:PATH=/c/libiio-"$PLATFORM" \
        -DCMAKE_CONFIGURATION_TYPES=RELEASE \
        -DSWIG_DIR=/c/swig/Lib \
        -DSWIG_EXECUTABLE=/c/swig/swig.exe \
        -DSWIG_VERSION="4.0.0" \
        -DENABLE_TOOLS=ON \
        -DENABLE_LOG=ON \
        -DPython_EXECUTABLE="$PY_PATH/python.exe" \
        ..
	cmake --build . --config Release

	"$PY_PATH/python.exe" -m pip install --user --upgrade setuptools wheel
	SETUPTOOLS_USE_DISTUTILS=stdlib "$PY_PATH/python.exe" setup.py bdist_wininst
	"$PY_PATH/python.exe" setup.py sdist bdist_wheel --plat-name "$PLAT_NAME" --python-tag py"$PY_VERSION"
    cp dist/libm2k-*.exe "/c/projects/libm2k/build-$PLATFORM/dist/libm2k-py$PY_VERSION-$PLATFORM.exe"
	cp dist/libm2k-*.whl "/c/projects/libm2k/build-$PLATFORM/dist/"
}

__mv_to_build_dir() {
	local PLATFORM="$1"

	DST_FOLDER="/c/projects/libm2k/build-$PLATFORM/"
	cd "/c/projects/libm2k/tmp-build-$PLATFORM"
	cp *.dll $DST_FOLDER
	cp *.exe $DST_FOLDER
	cp *.lib $DST_FOLDER
	cp *.iss $DST_FOLDER
	cd ..
}

__build_libm2k win32 37 "/c/Python37" "Visual Studio 15"
__build_libm2k win32 38 "/c/Python38" "Visual Studio 15"
__mv_to_build_dir win32

__build_libm2k win64 37 "/c/Python37-x64" "Visual Studio 15 Win64" "win_amd64"
__build_libm2k win64 38 "/c/Python38-x64" "Visual Studio 15 Win64" "win_amd64"
__mv_to_build_dir win64
