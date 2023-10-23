#!/bin/sh -e

LIBIIO_VERSION=libiio-v0
PACKAGE_DIR=${1-build}
echo $PACKAGE_DIR

apt-get -qq update
apt-get install -y git wget tar graphviz libavahi-common-dev libavahi-client-dev libaio-dev libusb-1.0-0-dev libxml2-dev rpm tar bzip2 gzip flex bison git swig python3 python3-dev python3-setuptools python3-pip python3-all libserialport-dev

python3 -m pip install cmake
cmake --version

git clone -b $LIBIIO_VERSION --single-branch --depth 1 https://github.com/analogdevicesinc/libiio.git libiio
cd libiio
mkdir -p build
cd build
cmake -DHAVE_DNS_SD=OFF ..
make
make install
cd ../..

#Install glog
git clone --branch v0.4.0 --depth 1 https://github.com/google/glog
mkdir -p glog/build_0_4_0
cd glog/build_0_4_0
cmake -DCMAKE_PREFIX_PATH=/usr ..
make
make install
cd ../..

cd $PACKAGE_DIR
rm -rf *
PYTHON_INCLUDE_DIR=$(/usr/bin/python3 -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())")  \
PYTHON_LIBRARY=$(/usr/bin/python3 -c "import distutils.sysconfig as sysconfig; print(sysconfig.get_config_var('LIBDIR'))")
PYTHON_EXECUTABLE=$(which /usr/bin/python3)
export COMPILE_BINDINGS=
cmake -DENABLE_PYTHON=ON -DENABLE_TOOLS=ON -DPython_EXECUTABLE=$PYTHON_EXECUTABLE -DPython_INCLUDE_DIRS=$PYTHON_INCLUDE_DIR -DPython_LIBRARIES=$PYTHON_LIBRARY ..
make
#rm -rf build
cat setup.py
make install

