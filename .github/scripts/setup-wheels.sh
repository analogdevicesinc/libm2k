#!/bin/bash

set -e

echo "=== Installing system dependencies ==="
yum install -y libxml2-devel libusbx-devel libaio-devel cmake git flex bison swig || \
    dnf install -y libxml2-devel libusbx-devel libaio-devel cmake git flex bison swig || true

echo "=== Building glog ==="
cd /tmp
git clone --branch v0.4.0 --depth 1 https://github.com/google/glog
cd glog
mkdir -p build && cd build
cmake -DCMAKE_PREFIX_PATH=/usr -DCMAKE_POLICY_VERSION_MINIMUM=3.5 ..
make -j$(nproc)
make install

echo "=== Building libiio from source ==="
cd /tmp
git clone --depth 1 --branch v0.26 https://github.com/analogdevicesinc/libiio.git
cd libiio
mkdir build && cd build
cmake .. -DHAVE_DNS_SD=OFF -DPYTHON_BINDINGS=OFF -DCSHARP_BINDINGS=OFF -DWITH_DOC=OFF
make -j$(nproc)
make install
ldconfig

echo "=== Building libm2k ==="
cd /project
mkdir -p build && cd build
rm -rf *

# Find a Python in /opt/python (manylinux)
PYTHON_BIN=$(ls /opt/python/cp3*/bin/python3 2>/dev/null | head -1)
if [ -z "$PYTHON_BIN" ]; then
    PYTHON_BIN=$(which python3)
fi
PYTHON_INCLUDE=$("$PYTHON_BIN" -c "import sysconfig; print(sysconfig.get_path('include'))")
PYTHON_VERSION=$("$PYTHON_BIN" -c "import sysconfig; print(sysconfig.get_config_var('VERSION'))")

# manylinux has no libpython.so - create a dummy one to satisfy cmake
# (Linux extensions don't actually need to link libpython - symbols resolve at runtime)
DUMMY_LIB="/tmp/libpython${PYTHON_VERSION}.so"
echo "int dummy;" | gcc -x c -shared -o "$DUMMY_LIB" -

echo "Using Python: $PYTHON_BIN"
echo "Include: $PYTHON_INCLUDE"
echo "Library (dummy): $DUMMY_LIB"

cmake .. -DENABLE_PYTHON=ON -DENABLE_TOOLS=ON -DENABLE_CSHARP=OFF -DENABLE_DOC=OFF \
    -DPYTHON_EXECUTABLE="$PYTHON_BIN" \
    -DPYTHON_INCLUDE_DIR="$PYTHON_INCLUDE" \
    -DPYTHON_LIBRARY="$DUMMY_LIB"

# Build everything except the libm2kpy target (which runs setup.py)
make -j$(nproc) libm2k _libm2k m2kcli

# Copy/link libm2k to where setup.py looks (-L/project/build -L/usr/lib64)
cp -a ./src/libm2k.so* /usr/lib64/
ln -sf libm2k.so.0.9.1 ./libm2k.so
ln -sf libm2k.so.0.9.1 ./libm2k.so.0
cp ./src/libm2k.so.0.9.1 .
ldconfig

# Now run setup.py manually
"$PYTHON_BIN" setup.py build

make install
ldconfig

echo "=== Done ==="
