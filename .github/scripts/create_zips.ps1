$ErrorActionPreference = "Stop"
$ErrorView = "NormalView"

$COMPILER = $Env:COMPILER
$ARCH = $Env:ARCH    
$PLATFORM = $Env:PLATFORM
$WINDOWS_OS = $Env:WINDOWS_OS

$SRC_DIR = Get-Item -Path $env:BUILD_SOURCESDIRECTORY # path to repo
$OUTSIDE_BUILD = $SRC_DIR.Parent.FullName
$BUILD_DIR = Join-Path $OUTSIDE_BUILD "libm2k-$PLATFORM\*"


$ARTIFACTS_DIR = Get-Item -Path $env:BUILD_ARTIFACTSTAGINGDIRECTORY # path to repo
New-Item -Path (Join-Path $ARTIFACTS_DIR "libm2k-$PLATFORM") -ItemType Directory
Copy-Item -Path $BUILD_DIR -Destination (Join-Path $ARTIFACTS_DIR "libm2k-$PLATFORM") -Recurse -Container

Set-Location $SRC_DIR

7z a "libm2k-$PLATFORM-$WINDOWS_OS.zip" "$env:BUILD_ARTIFACTSTAGINGDIRECTORY/libm2k-$PLATFORM/*"
New-Item -ItemType Directory -Path "$env:BUILD_SOURCESDIRECTORY/artifacts"
Move-Item -Path "libm2k-$PLATFORM-$WINDOWS_OS.zip" -Destination "$env:BUILD_SOURCESDIRECTORY/artifacts"
