#!/bin/bash

set -e

PACKAGE_DIR=${1-build}

[[ "$ARCHITECTURE" == "arm64" ]] && SUDO="" || SUDO="sudo"

cd $PACKAGE_DIR
rm -rf *
PYTHON_INCLUDE_DIR=$(/usr/bin/python3 -c "import sysconfig; print(sysconfig.get_path('include'))")
PYTHON_LIBRARY=$(/usr/bin/python3 -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))")
PYTHON_EXECUTABLE=$(which /usr/bin/python3)
export COMPILE_BINDINGS=
cmake -DENABLE_PYTHON=ON -DENABLE_TOOLS=ON -DPython_EXECUTABLE=$PYTHON_EXECUTABLE -DPython_INCLUDE_DIRS=$PYTHON_INCLUDE_DIR -DPython_LIBRARIES=$PYTHON_LIBRARY ..
make
#rm -rf build
cat setup.py
${SUDO} make install

# Repair wheel step fails if it does not find the .so file
echo "## Content from src folder"
ls ./src
echo "creating links to .so"
${SUDO} cp ./src/libm2k.so* /usr/lib/ && ldconfig -n -v /usr/lib
