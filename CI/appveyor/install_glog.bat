set configuration=%~1
set arch=%~2
set folder_arch=%~3

if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2015" (
    set generator=Visual Studio 14 2015
) else if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2017" (
    set generator=Visual Studio 15 2017
) else if "%APPVEYOR_BUILD_WORKER_IMAGE%"=="Visual Studio 2019" (
    set generator=Visual Studio 16 2019
)

echo "%generator%"
echo "%arch%"


if not exist "glog" ( git clone --branch v0.3.5 --depth 1 https://github.com/google/glog )
pushd glog
git checkout tags/v0.3.5
if not exist "build_0_3_5-%folder_arch%" ( mkdir build_0_3_5-%folder_arch% )
pushd build_0_3_5-%folder_arch%

cmake -DWITH_GFLAGS=off -DBUILD_SHARED_LIBS=on -G "%generator%" -A "%arch%" ..
cmake --build . --target install --config %configuration%

popd
popd
