@ECHO OFF
REM
REM Copyright (c) 2024 Analog Devices Inc.
REM
REM This file is part of libm2k
REM (see http://www.github.com/analogdevicesinc/libm2k).
REM
REM This program is free software; you can redistribute it and/or modify
REM it under the terms of the GNU Lesser General Public License as published by
REM the Free Software Foundation, either version 2.1 of the License, or
REM (at your option) any later version.
REM
REM This program is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM GNU Lesser General Public License for more details.
REM
REM You should have received a copy of the GNU Lesser General Public License
REM along with this program. If not, see <http://www.gnu.org/licenses/>.
REM
@ECHO ON

set root=%cd%

:: libiio
mkdir deps

curl.exe  --output  libiio-win.zip --url https://github.com/analogdevicesinc/libiio/releases/download/v0.25/libiio-0.25-gb6028fd-windows.zip -L
powershell -Command "Expand-Archive -Path libiio-win.zip -DestinationPath libiio"
powershell -Command "Get-ChildItem -Path libiio/Windows-VS-2022-x64 -Recurse -Filter *.dll | Copy-Item -Destination deps "
powershell -Command "Get-ChildItem -Path libiio/Windows-VS-2022-x64 -Recurse -Filter libiio.exp | Copy-Item -Destination deps "
powershell -Command "Get-ChildItem -Path libiio/Windows-VS-2022-x64 -Recurse -Filter libiio.lib | Copy-Item -Destination deps "
mkdir deps\include
powershell -Command "Get-ChildItem -Path libiio/include -Recurse -Filter iio.h | Copy-Item -Destination deps/include "

::libm2k
git clone -b v0.8.0 https://github.com/analogdevicesinc/libm2k.git
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

