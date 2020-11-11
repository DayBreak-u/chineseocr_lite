:: build opencv 3.4.11 for windows static (.lib) by benjaminwan

@ECHO OFF
mkdir build-win-vs2017-x86
pushd build-win-vs2017-x86
call :cmakeParams "Visual Studio 15 2017" "Win32"
popd

mkdir build-win-vs2017-x64
pushd build-win-vs2017-x64
call :cmakeParams "Visual Studio 15 2017" "x64"
popd

mkdir build-win-vs2019-x86
pushd build-win-vs2019-x86
call :cmakeParams "Visual Studio 16 2019" "Win32"
popd

mkdir build-win-vs2019-x64
pushd build-win-vs2019-x64
call :cmakeParams "Visual Studio 16 2019" "x64"
popd

GOTO:EOF

:cmakeParams
cmake -G "%~1" -A "%~2" ^
  -DBUILD_DOCS=OFF ^
  -DBUILD_EXAMPLES=OFF ^
  -DBUILD_PERF_TESTS=OFF ^
  -DBUILD_PROTOBUF=OFF ^
  -DBUILD_SHARED_LIBS=OFF ^
  -DBUILD_TESTS=OFF ^
  -DBUILD_WITH_DEBUG_INFO=OFF ^
  -DBUILD_WITH_STATIC_CRT=OFF ^
  -DBUILD_opencv_world=ON ^
  ..
GOTO:EOF