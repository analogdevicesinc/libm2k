# https://docs.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_preference_variables?view=powershell-7.2#erroractionpreference
$ErrorActionPreference = "Stop"
$ErrorView = "NormalView"

$COMPILER = $Env:COMPILER
$ARCH = $Env:ARCH        
$PLATFORM = $Env:PLATFORM

$SRC_DIR = Get-Item -Path $env:BUILD_SOURCESDIRECTORY # path to repo
$OUTSIDE_BUILD = $SRC_DIR.Parent.FullName
$DEPS_DIR= Join-Path $OUTSIDE_BUILD "deps"

$BUILD_DIR = Join-Path $OUTSIDE_BUILD "libm2k-$PLATFORM"
$TEMP_BUILD_DIR = Join-Path $SRC_DIR "tmp-build-$PLATFORM"
$DEST_LIBIIO = Join-Path $OUTSIDE_BUILD "libiio-$ARCH"

Write-Output "COMPILER  = $COMPILER"
Write-Output "ARCH  = $ARCH"
Write-Output "PLATFORM  = $PLATFORM"
Write-Output "SRC DIR = $SRC_DIR"
Write-Output "OUTSIDE_BUILD = $OUTSIDE_BUILD"
Write-Output "DEPS_DIR = $DEPS_DIR"
Write-Output "BUILD_DIR = $BUILD_DIR"
Write-Output "TEMP_BUILD_DIR  = $TEMP_BUILD_DIR"
Write-Output "DEST_LIBIIO  = $DEST_LIBIIO"

if (-not (Test-Path -Path $BUILD_DIR)) {
    New-Item -Path $BUILD_DIR -ItemType Directory
    New-Item -Path (Join-Path $BUILD_DIR "dist") -ItemType Directory
}

function Build-Libm2k {
    param(
        [string]$PLATFORM,
        [string]$PYTHON_VERSION,
        [string]$GENERATOR,
        [string]$ARCH
    )

    if (Test-Path -Path $TEMP_BUILD_DIR) {
        Remove-Item $TEMP_BUILD_DIR -Recurse -Force
    }
    New-Item -Path $TEMP_BUILD_DIR -ItemType Directory

    Set-Location $TEMP_BUILD_DIR

    $pythonExecutable = (python -c "import sys; print(sys.executable)") -replace '\\', '/'
    $PATH_SWIG_DIR = (Join-Path $DEPS_DIR "swig\Lib") -replace '\\', '/'
    $PATH_SWIG_EXE = (Join-Path $DEPS_DIR "swig\swig.exe") -replace '\\', '/'
    $SWIG_VERSION = "4.0.0"
    $IIO_LIBS_DIR= (Join-Path $DEST_LIBIIO "libiio.lib") -replace '\\', '/'
    $IIO_INCLUDE_DIR= ($DEST_LIBIIO) -replace '\\', '/'

    Write-Output "Running cmake on generator $GENERATOR for $ARCH"

    cmake -G "$GENERATOR" -A "$ARCH" -DIIO_LIBRARIES:FILEPATH="$IIO_LIBS_DIR" -DIIO_INCLUDE_DIRS:PATH="$IIO_INCLUDE_DIR" -DCMAKE_CONFIGURATION_TYPES=RELEASE -DSWIG_DIR="$PATH_SWIG_DIR" -DSWIG_EXECUTABLE="$PATH_SWIG_EXE" -DSWIG_VERSION="$SWIG_VERSION" -DENABLE_TOOLS=ON -DENABLE_LOG=ON -DPython_EXECUTABLE="$pythonExecutable" -DBUILD_EXAMPLES=ON -DENABLE_CSHARP=ON -DENABLE_LABVIEW=ON -DENABLE_PYTHON=ON ..
    
    if ($LASTEXITCODE -ne 0) {
        Write-Output "## cmake #1 failed with exit code $LASTEXITCODE"
        exit $LASTEXITCODE
    }

    Write-Output "Running second cmake"
    cmake --build . --config Release

    if ($LASTEXITCODE -ne 0) {
        Write-Output "## cmake #2 failed with exit code $LASTEXITCODE"
        exit $LASTEXITCODE
    }
	Get-Content setup.py
}

function Get-Dll-Paths() {
    $vsPath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if ($vsPath -eq $null) {
            Write-Host "Visual Studio (vswhere.exe) not found - can't find dll paths."
            return $null
    } 

    $vsInstallerPath = & $vsPath -latest -requires Microsoft.Component.MSBuild -property installationPath
    if ($vsInstallerPath -eq $null) {
        Write-Host "Visual Studio Installer path is not available"
        return $null
    }
    
    $msvcp140Path = Join-Path $vsInstallerPath 'VC\Tools\MSVC\14.*\bin\Hostx64\x64\msvcp140.dll'
    $vcruntime140Path = Join-Path $vsInstallerPath 'VC\Tools\MSVC\14.*\bin\Hostx64\x64\vcruntime140.dll'
    if (($msvcp140Path -eq $null) -or ($vcruntime140Path -eq $null)) {
        Write-Host "Dll paths not found in Visual Studion installation path."
        return $null
    }

    $result = New-Object PSObject -Property @{
        msvcp140Path = $msvcp140Path
        vcruntime140Path = $vcruntime140Path
    }
    return $result
}

function Move-To-Build-Dir() {
    $ddlPaths = Get-Dll-Paths

    if ($ddlPaths -ne $null) {
        Write-Host "msvcp140Path: $($ddlPaths.msvcp140Path)"
        Write-Host "vcruntime140Path: $($ddlPaths.vcruntime140Path)"
    } else {
        Write-Host "Dll Paths not found."
    }

    Copy-Item -Path $ddlPaths.msvcp140Path -Destination $BUILD_DIR
    Copy-Item -Path $ddlPaths.vcruntime140Path -Destination $BUILD_DIR

    Set-Location $TEMP_BUILD_DIR
    Copy-Item -Path "*.dll" -Destination $BUILD_DIR
    Copy-Item -Path "*.exe" -Destination $BUILD_DIR
    Copy-Item -Path "*libm2k.lib" -Destination $BUILD_DIR
    Copy-Item -Path "*.iss" -Destination $BUILD_DIR

    Copy-Item -Path (Join-Path $OUTSIDE_BUILD "libiio-$ARCH\*.dll") -Destination $BUILD_DIR   
    Copy-Item -Path (Join-Path $OUTSIDE_BUILD "libiio-$ARCH\*.dll") -Destination $BUILD_DIR   
    Copy-Item -Path (Join-Path $OUTSIDE_BUILD "deps\glog\build_0_4_0-x64\Release\glog.dll") -Destination $BUILD_DIR  

    Set-Location $SRC_DIR
    xcopy include $BUILD_DIR /s /e /h
}

Build-Libm2k -PLATFORM $PLATFORM -GENERATOR $COMPILER -ARCH $ARCH 
Move-To-Build-Dir

Set-Location $SRC_DIR
