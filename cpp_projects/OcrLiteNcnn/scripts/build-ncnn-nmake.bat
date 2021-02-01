:: build opencv 3.4.x for windows by benjaminwan
@ECHO OFF
chcp 65001
cls
@SETLOCAL

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
  -DCMAKE_BUILD_TYPE="%~1" ^
  -DNCNN_OPENMP=ON ^
  -DNCNN_BUILD_BENCHMARK=OFF ^
  -DNCNN_BUILD_EXAMPLES=OFF ^
  -DNCNN_BUILD_TOOLS=OFF ^
  -DNCNN_PYTHON=OFF ^
  -DNCNN_VULKAN=OFF ^
  ..
nmake
nmake install
popd
GOTO:EOF