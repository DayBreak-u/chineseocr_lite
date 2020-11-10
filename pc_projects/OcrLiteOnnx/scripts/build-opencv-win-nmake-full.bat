:: build opencv 3.4.11 for windows static (.lib) by benjaminwan

mkdir build-win-Release
pushd build-win-Release
call :cmakeParams "Release"
nmake
nmake install
popd

mkdir build-win-Debug
pushd build-win-Debug
call :cmakeParams "Debug"
nmake
nmake install
popd

GOTO:EOF

:cmakeParams
cmake -G "NMake Makefiles" ^
  -DCMAKE_BUILD_TYPE="%~1" -DCMAKE_CONFIGURATION_TYPES="%~1" ^
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