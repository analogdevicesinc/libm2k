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

# libm2k.iss.cmakein hard-codes the Azure DevOps layout (sources at D:\a\1\s,
# build root at D:\a\1). Rewrite those literals to the actual paths so the
# installer builds on any runner. No-op on Azure, where they already match.
$iss = Join-Path $BUILD_DIR "libm2k.iss"
(Get-Content $iss) `
    -replace [regex]::Escape("D:\a\1\s"), $SRC_DIR.FullName `
    -replace [regex]::Escape("D:\a\1"), $OUTSIDE_BUILD |
    Set-Content $iss

ISCC $iss

$ARTIFACTS_DIR = Get-Item -Path $env:BUILD_ARTIFACTSTAGINGDIRECTORY # path to repo
New-Item -Path (Join-Path $ARTIFACTS_DIR "installer") -ItemType Directory
Copy-Item -Path $INSTALLER_PATH -Destination (Join-Path $ARTIFACTS_DIR "installer")

Set-Location $SRC_DIR