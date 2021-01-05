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

echo 请选择要生成的选项并回车: 0)ALL，1)VS2015-x86，2)VS2015-x64，3)VS2017-x86，4)VS2017-x64，5)VS2019-x86，6)VS2019-x64，:
echo Please Select VS Ver: 0)ALL，1)VS2015-x86，2)VS2015-x64，3)VS2017-x86，4)VS2017-x64，5)VS2019-x86，6)VS2019-x64，:
set /p flag=
if %flag% == 0 (call :buildALL)^
else if %flag% == 1 (call :build2015x86)^
else if %flag% == 2 (call :build2015x64)^
else if %flag% == 3 (call :build2017x86)^
else if %flag% == 4 (call :build2017x64)^
else if %flag% == 5 (call :build2019x86)^
else if %flag% == 6 (call :build2019x64)^
else (echo "输入错误！Input Error!")
GOTO:EOF

:buildALL
call :build2015x86
call :build2015x64
call :build2017x86
call :build2017x64
call :build2019x86
call :build2019x64
GOTO:EOF

:build2015x86
mkdir build-win-vs2015-x86
pushd build-win-vs2015-x86
call :cmakeParams "Visual Studio 14 2015" "Win32"
popd
GOTO:EOF

:build2015x64
mkdir build-win-vs2015-x64
pushd build-win-vs2015-x64
call :cmakeParams "Visual Studio 14 2015" "x64"
popd
GOTO:EOF

:build2017x86
mkdir build-win-vs2017-x86
pushd build-win-vs2017-x86
call :cmakeParams "Visual Studio 15 2017" "Win32"
popd
GOTO:EOF

:build2017x64
mkdir build-win-vs2017-x64
pushd build-win-vs2017-x64
call :cmakeParams "Visual Studio 15 2017" "x64"
popd
GOTO:EOF

:build2019x86
mkdir build-win-vs2019-x86
pushd build-win-vs2019-x86
call :cmakeParams "Visual Studio 16 2019" "Win32"
popd
GOTO:EOF

:build2019x64
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
GOTO:EOF