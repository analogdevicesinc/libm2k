$ErrorActionPreference = "Stop"
$ErrorView = "NormalView"

$COMPILER = $Env:COMPILER
$ARCH = $Env:ARCH     
$PLATFORM = $Env:PLATFORM
$LIBIIO_VERSION = $env:LIBIIO_VERSION  


$SRC_DIR = Get-Item -Path $env:BUILD_SOURCESDIRECTORY # path to repo
$OUTSIDE_BUILD = $SRC_DIR.Parent.FullName
$DEPS_DIR = Join-Path $OUTSIDE_BUILD "deps" 

$LIBIIO_DEPS_DIR = Join-Path $DEPS_DIR "libiio-deps"
$LIBIIO_COMPILE_OUTPUT = Join-Path $OUTSIDE_BUILD "libiio-$ARCH"  

Write-Output "Python located at:"
Get-Command python

Write-Output "Running script from $SRC_DIR"
Write-Output "COMPILER  = $COMPILER"
Write-Output "ARCH  = $ARCH"
Write-Output "SRC_DIR to $SRC_DIR"
Write-Output "DEPS_DIR to $DEPS_DIR"
Write-Output "OUTSIDE_BUILD to $OUTSIDE_BUILD"
Write-Output "LIBIIO_COMPILE_OUTPUT = $LIBIIO_COMPILE_OUTPUT"

if (-not (Test-Path -Path ($DEPS_DIR))) {
    New-Item -Path ($DEPS_DIR) -ItemType Directory
}

function Get-Libiio-Deps {
    param (
        [string]$DIR_PATH
    )
    Write-Output "# Building libiio dependencies"

    Write-Output "## Installing libxml"
    $LIBXML_DIR = Join-Path $DIR_PATH "libxml"
    New-Item -Path $LIBXML_DIR -ItemType Directory
    Set-Location $LIBXML_DIR
    Invoke-WebRequest -Uri "https://www.zlatkovic.com/pub/libxml/64bit/libxml2-2.9.3-win32-x86_64.7z" -OutFile "libxml.7z"
    7z x -y "libxml.7z"
    Remove-Item "libxml.7z"

    Write-Output "## Installing libiio-deps" 
    if (-not (Test-Path -Path ($LIBIIO_DEPS_DIR))) {
        New-Item -Path ($LIBIIO_DEPS_DIR) -ItemType Directory
    }
    Set-Location $LIBIIO_DEPS_DIR
    Invoke-WebRequest -Uri "http://swdownloads.analog.com/cse/build/libiio-win-deps-libusb1.0.24.zip " -OutFile "libiio-win-deps.zip"
    7z x -y "libiio-win-deps.zip"
    Remove-Item "libiio-win-deps.zip"

    # NOTE: organize cmake folder structure
    Set-Location "include"
    New-Item -Path (Join-Path $LIBIIO_DEPS_DIR "include\libserialport") -ItemType Directory
    Move-Item -Path ./libserialport.h -Destination libserialport
}

function Install-Libiiio {
    param (
        [string]$DIR_PATH,
        [string]$GENERATOR,
        [string]$ARCH
    )
    Write-Output "# Installing libiio"
    Set-Location $DIR_PATH
    
    git clone "https://github.com/analogdevicesinc/libiio" -b $LIBIIO_VERSION "libiio"
    Set-Location (Join-Path $DIR_PATH "libiio")
    git submodule update --init
    
    Write-Output "## Running cmake for LIBIIO $COMPILER on $ARCH..."                   # x64 instead of ARCH in example
    New-Item -Path (Join-Path $DIR_PATH "libiio\build-$ARCH") -ItemType Directory
    Copy-Item -Path ".\libiio.iss.cmakein" -Destination (Join-Path $DIR_PATH "libiio\build-$ARCH")
    Set-Location (Join-Path $DIR_PATH "libiio\build-$ARCH")
    
    # cmake config from libiio CI scripts
    $pythonExecutable = (python -c "import sys; print(sys.executable)") -replace '\\', '/'
    $PATH_LIBXML2_LIB = (Join-Path $DIR_PATH "libiio-deps\libs\64\libxml2.lib") -replace '\\', '/'
    $PATH_LIBUSB_LIB = (Join-Path $DIR_PATH "libiio-deps\libs\64\libusb-1.0.lib") -replace '\\', '/'
    $PATH_LIBSERIALPORT_LIB = (Join-Path $DIR_PATH "libiio-deps\libs\64\libserialport.dll.a") -replace '\\', '/'
    $PATH_LIBUSB_INCLUDE = (Join-Path $DIR_PATH "libiio-deps\include\libusb-1.0") -replace '\\', '/'
    $PATH_LIBXML2_INCLUDE = (Join-Path $DIR_PATH "libiio-deps\include\libxml2") -replace '\\', '/'
    $PATH_LIBSERIALPORT_INCLUDE = (Join-Path $DIR_PATH "libiio-deps\include\libserialport") -replace '\\', '/'

    cmake -G "$COMPILER" -DPYTHON_EXECUTABLE:FILEPATH="$pythonExecutable" -DCMAKE_SYSTEM_PREFIX_PATH="C:" -Werror=dev -DCOMPILE_WARNING_AS_ERROR=ON -DENABLE_IPV6=ON -DWITH_USB_BACKEND=ON -DWITH_SERIAL_BACKEND=ON -DPYTHON_BINDINGS=ON -DCSHARP_BINDINGS:BOOL=OFF -DLIBXML2_LIBRARIES="$PATH_LIBXML2_LIB" -DLIBUSB_LIBRARIES="$PATH_LIBUSB_LIB" -DLIBSERIALPORT_LIBRARIES="$PATH_LIBSERIALPORT_LIB" -DLIBUSB_INCLUDE_DIR="$PATH_LIBUSB_INCLUDE" -DLIBXML2_INCLUDE_DIR="$PATH_LIBXML2_INCLUDE" -DLIBSERIALPORT_INCLUDE_DIR="$PATH_LIBSERIALPORT_INCLUDE" ..
    cmake --build . --config Release

    if ($LASTEXITCODE -ne 0) {
        Write-Output "## cmake failed with exit code $LASTEXITCODE"
        exit $LASTEXITCODE
    }

    # After building libiio, copy files to a known directory
    if (-not (Test-Path -Path ($LIBIIO_COMPILE_OUTPUT))) {
        New-Item -Path ($LIBIIO_COMPILE_OUTPUT) -ItemType Directory
    }
    Copy-Item -Path ".\Release\*.dll" -Destination ($LIBIIO_COMPILE_OUTPUT)
    Copy-Item -Path ".\Release\*.lib" -Destination ($LIBIIO_COMPILE_OUTPUT)
    Copy-Item -Path "*.iss" -Destination ($LIBIIO_COMPILE_OUTPUT)
    Set-Location -Path ..
    Copy-Item -Path "iio.h" -Destination ($LIBIIO_COMPILE_OUTPUT)
    Copy-Item -Path ".\*.dll" -Destination ($LIBIIO_COMPILE_OUTPUT) # might not need this: no .dll files in libiio sources
    # copy .dll used to build libiio
    Copy-Item -Path (Join-Path $LIBIIO_DEPS_DIR "libs\64\*.dll") -Destination ($LIBIIO_COMPILE_OUTPUT)

    # NOTE: the result of these commands is a .tar.gz file -> copied from LIBIIO CI scripts
    Set-Location (Join-Path $DIR_PATH "libiio\build-$ARCH")

    Set-Location -Path .\bindings\python
    python.exe setup.py sdist
    Get-ChildItem dist\pylibiio-*.tar.gz | Rename-Item -NewName "libiio-py39-amd64.tar.gz"
    Move-Item -Path .\dist\*.gz -Destination .
    Remove-Item -Path .\dist\*.gz
}

function Install-Swig {
    param (
        [string]$DIR_PATH
    )
    Write-Output "# Installing swig"
    Set-Location $DIR_PATH

    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
    $crnt_attempt=0
    $max_attemps=10
    do {
        try {
            curl.exe -L -o "swigwin-4.0.0.zip" "https://sourceforge.net/projects/swig/files/swigwin/swigwin-4.0.0/swigwin-4.0.0.zip/download"
        } catch {
            Write-Output "## curl failed, retrying..."
            Start-Sleep -s 3
        }
    } while (-not (Test-Path -Path "swigwin-4.0.0.zip") -and ($crnt_attempt++ -lt $max_attemps))
    
    7z x "swigwin-4.0.0.zip" -oswig
    Remove-Item "swigwin-4.0.0.zip"
    Set-Location (Join-Path "swig" "swigwin-4.0.0")
    xcopy * .. /s /e /h /Q
    Get-ChildItem
}

function Install-Inno-Setup {
    Write-Output "# Installing inno-setup"
    choco install InnoSetup -y
}

function Install-Glog {
    param (
        [string]$DIR_PATH,
        [string]$CONFIGURATION,
        [string]$ARCH,
        [string]$GENERATOR
    )
    Write-Output "# Installing glog"
    Set-Location $DIR_PATH

    if (-not (Test-Path -Path (Join-Path $DIR_PATH "glog"))) {
        git clone --branch v0.3.5 --depth 1 https://github.com/google/glog
    }
    Set-Location (Join-Path $DIR_PATH "glog")
    git checkout "tags/v0.3.5"

    if (-not (Test-Path -Path (Join-Path $DIR_PATH "glog\build_0_4_0-$ARCH"))) {
        New-Item -Path (Join-Path $DIR_PATH "glog\build_0_4_0-$ARCH") -ItemType Directory
    }
    Set-Location ((Join-Path $DIR_PATH "glog\build_0_4_0-$ARCH"))

    cmake -DWITH_GFLAGS=off -DBUILD_SHARED_LIBS=on -G "$GENERATOR" -A "$ARCH" ..
    cmake --build . --target install --config "$CONFIGURATION"
}

function Install-All {
    Install-Inno-Setup
    Install-Glog -DIR_PATH $DEPS_DIR -CONFIGURATION "Release" -GENERATOR $COMPILER -ARCH $ARCH
    Install-Swig -DIR_PATH $DEPS_DIR
    Get-Libiio-Deps -DIR_PATH $DEPS_DIR
    Install-Libiiio -DIR_PATH $DEPS_DIR -GENERATOR $COMPILER -ARCH $ARCH
}

Install-All
Set-Location $SRC_DIR
