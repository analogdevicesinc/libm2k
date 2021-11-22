#!/bin/sh -e
TOP_DIR=$(pwd)
python3 --version
python3 -m pip install wheel twine build virtualenv
cd build
mkdir -p wheelhouse
export COMPILE_BINDINGS=True
python3 -m build
export COMPILE_BINDINGS=
cp dist/*.whl wheelhouse
cd $(TOP_DIR)