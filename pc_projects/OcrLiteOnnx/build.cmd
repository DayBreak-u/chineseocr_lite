@ECHO OFF
chcp 65001
cls
@SETLOCAL

echo 请选择编译选项并回车: 1)编译可执行文件，2)编译动态库:
echo Please Select Build Type: 1)Executable，2)Dynamic Library:
set /p flag=
if %flag% == 1 (call :buildExe)^
else if %flag% == 2 (call :buildLib)^
else (echo "输入错误！Input Error!")
GOTO:EOF

:buildExe
mkdir build
pushd build
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DOCR_LITE_OPENMP=ON ..
nmake
popd
GOTO:EOF

:buildLib
mkdir build-lib
pushd build-lib
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DOCR_LITE_LIB=ON -DOCR_LITE_OPENMP=ON ..
nmake
popd
GOTO:EOF

@ENDLOCAL
