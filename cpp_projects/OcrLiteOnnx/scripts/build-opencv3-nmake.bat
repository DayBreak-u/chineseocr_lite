:: build opencv 3.4.x for windows by benjaminwan
@ECHO OFF
chcp 65001
cls
@SETLOCAL

echo 请选择生成库选项并回车: 1)Shared动态库，2)Static静态库:
echo Please Select Lib Type: 1)Shared Lib，2)Static Lib:
set BUILD_SHARED=ON
set /p flag=
if %flag% == "" (set BUILD_SHARED=ON)^
else if %flag% == 1 (set BUILD_SHARED=ON)^
else if %flag% == 2 (set BUILD_SHARED=OFF)^
else (echo "输入错误！Input Error!")

echo 请选择要生成的选项并回车: 1)Release，2)Debug:
echo Please Select Build Type: 1)Release，2)Debug:
set BUILD_TYPE=Release
set /p flag=
if %flag% == 0 (set BUILD_TYPE=Release)^
else if %flag% == 1 (set BUILD_TYPE=Release)^
else if %flag% == 2 (set BUILD_TYPE=Debug)^
else (echo "输入错误！Input Error!")
call :cmakeParams %BUILD_TYPE%
GOTO:EOF

:cmakeParams
mkdir "build-windows-%~1"
pushd "build-windows-%~1"
cmake -G "NMake Makefiles" ^
  -DCMAKE_BUILD_TYPE="%~1" -DCMAKE_CONFIGURATION_TYPES="%~1" ^
  -DBUILD_DOCS=OFF ^
  -DBUILD_EXAMPLES=OFF ^
  -DBUILD_PERF_TESTS=OFF ^
  -DBUILD_PROTOBUF=OFF ^
  -DBUILD_SHARED_LIBS=%BUILD_SHARED% ^
  -DBUILD_TESTS=OFF ^
  -DBUILD_WITH_DEBUG_INFO=OFF ^
  -DBUILD_WITH_STATIC_CRT=OFF ^
  -DBUILD_IPP_IW=ON ^
  -DBUILD_ITT=ON ^
  -DBUILD_JASPER=ON ^
  -DBUILD_JPEG=ON ^
  -DBUILD_PNG=ON ^
  -DBUILD_TBB=ON ^
  -DBUILD_TIFF=ON ^
  -DBUILD_WEBP=ON ^
  -DBUILD_ZLIB=ON ^
  -DWITH_EIGEN=OFF ^
  -DWITH_FFMPEG=OFF ^
  -DBUILD_opencv_dnn=OFF ^
  -DBUILD_opencv_world=ON ^
  ..
nmake
nmake install
popd
GOTO:EOF