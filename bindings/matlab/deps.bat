set root=%cd%

// libiio
mkdir -p deps
wget https://github.com/analogdevicesinc/libiio/releases/tag/v0.24/Windows-VS-2022-x64.zip -O libiio-win.zip
unzip libiio-win.zip
cp -r libiio*/*.dll deps/
cp -r libiio*/libiio.exp deps/
cp -r libiio*/libiio.lib deps/
mkdir -p deps\include
cp -r libiio*/iio.h deps/include/


// libm2k
git clone -b v0.7.0 https://github.com/analogdevicesinc/libm2k.git
cd libm2k
mkdir -p build
cd build
cmake -DENABLE_CSHARP=OFF -DENABLE_PYTHON=OFF -DIIO_LIBRARIES:FILEPATH="%root%\deps\libiio.lib" -DIIO_INCLUDE_DIRS:PATH="%root%\deps\include" -DCMAKE_CONFIGURATION_TYPES=RELEASE -G "Visual Studio 17 2022" ..

cmake --build . --config Release
cd ../..
cp libm2k/build/libm2k.dll deps/
cp libm2k/build/libm2k.exp deps/
cp libm2k/build/libm2k.lib deps/
cp -r libm2k/include/* deps/include/

// Cleanup
rm -rf libiio*
rm -rf libm2k
rm -rf libiio-win.zip
mv deps libm2k

