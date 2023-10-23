$ErrorActionPreference = "Stop"
$ErrorView = "NormalView"

$COMPILER = $Env:COMPILER
$ARCH = $Env:ARCH        
$PLATFORM = $Env:PLATFORM

$SRC_DIR = Get-Item -Path $env:BUILD_SOURCESDIRECTORY # path to repo
$OUTSIDE_BUILD = $SRC_DIR.Parent.FullName
$BUILD_DIR = Join-Path $OUTSIDE_BUILD "libm2k-$PLATFORM"
$INSTALLER_PATH = "C:\libm2k-system-setup.exe"

Write-Output "Running script from $SRC_DIR"
Write-Output "OUTSIDE_BUILD to $OUTSIDE_BUILD"
Write-Output "BUILD_DIR to $BUILD_DIR"
Write-Output "INSTALLER_PATH to $INSTALLER_PATH"


Set-Location $SRC_DIR
ISCC (Join-Path $BUILD_DIR "libm2k.iss")

$ARTIFACTS_DIR = Get-Item -Path $env:BUILD_ARTIFACTSTAGINGDIRECTORY # path to repo
New-Item -Path (Join-Path $ARTIFACTS_DIR "installer") -ItemType Directory
Copy-Item -Path $INSTALLER_PATH -Destination (Join-Path $ARTIFACTS_DIR "installer")

Set-Location $SRC_DIR