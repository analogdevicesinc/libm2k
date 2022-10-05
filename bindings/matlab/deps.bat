set root=%cd%

:: libiio
mkdir deps

curl.exe  --output  libiio-win.zip --url https://github.com/analogdevicesinc/libiio/releases/download/v0.24/Windows-VS-2022-x64.zip -L
powershell -Command "Expand-Archive -Path libiio-win.zip -DestinationPath libiio"
powershell -Command "Get-ChildItem -Path libiio -Recurse -Filter *.dll | Copy-Item -Destination deps "
powershell -Command "Get-ChildItem -Path libiio -Recurse -Filter libiio.exp | Copy-Item -Destination deps "
powershell -Command "Get-ChildItem -Path libiio -Recurse -Filter libiio.lib | Copy-Item -Destination deps "
mkdir deps\include
powershell -Command "Get-ChildItem -Path libiio -Recurse -Filter iio.h | Copy-Item -Destination deps/include "

::libm2k
git clone -b v0.7.0 https://github.com/analogdevicesinc/libm2k.git
cd libm2k
mkdir build
cd build
cmake -DENABLE_CSHARP=OFF -DENABLE_PYTHON=OFF -DIIO_LIBRARIES:FILEPATH="%root%/deps/libiio.lib" -DIIO_INCLUDE_DIRS:PATH="%root%/deps/include" -DCMAKE_CONFIGURATION_TYPES=RELEASE -G "Visual Studio 17 2022" ..

cmake --build . --config Release
cd ../..
powershell -Command "Get-ChildItem -Path libm2k/build/ -Recurse -Filter libm2k.dll | Copy-Item -Destination deps "
powershell -Command "Get-ChildItem -Path libm2k/build/ -Recurse -Filter libm2k.exp | Copy-Item -Destination deps "
powershell -Command "Get-ChildItem -Path libm2k/build/ -Recurse -Filter libm2k.lib | Copy-Item -Destination deps "
powershell -Command "Get-ChildItem -Path libm2k/include -Recurse  | Copy-Item -Destination deps/include "

:: Cleanup
powershell -Command "Remove-Item 'libiio' -Recurse -Force"
powershell -Command "Remove-Item 'libm2k' -Recurse -Force"
powershell -Command "Remove-Item 'libiio-win.zip' -Recurse -Force"
powershell -Command "Rename-Item -Path deps -NewName libm2k"

