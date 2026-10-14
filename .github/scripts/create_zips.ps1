$ErrorActionPreference = "Stop"
$ErrorView = "NormalView"

$COMPILER = $Env:COMPILER
$ARCH = $Env:ARCH    
$PLATFORM = $Env:PLATFORM

$SRC_DIR = Get-Item -Path $env:BUILD_SOURCESDIRECTORY # path to repo
$OUTSIDE_BUILD = $SRC_DIR.Parent.FullName
$BUILD_DIR = Join-Path $OUTSIDE_BUILD "libm2k-$PLATFORM\*"


$ARTIFACTS_DIR = Get-Item -Path $env:BUILD_ARTIFACTSTAGINGDIRECTORY # path to repo
New-Item -Path (Join-Path $ARTIFACTS_DIR "libm2k-$PLATFORM") -ItemType Directory
Copy-Item -Path $BUILD_DIR -Destination (Join-Path $ARTIFACTS_DIR "libm2k-$PLATFORM") -Recurse -Container

Set-Location $SRC_DIR
