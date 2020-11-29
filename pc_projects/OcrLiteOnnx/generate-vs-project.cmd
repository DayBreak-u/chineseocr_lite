@ECHO OFF
chcp 65001
cls
@SETLOCAL

echo 请输入选项并回车: 1)vs2015-x86, 2)vs2015-x64, 3)vs2017-x86, 4)vs2017-x64, 5)vs2019-x86, 6)vs2019-x64:
echo Please Input VS Version: 1)vs2015-x86, 2)vs2015-x64, 3)vs2017-x86, 4)vs2017-x64, 5)vs2019-x86, 6)vs2019-x64:
set /p flag=
if %flag% == 1 (call :gen2015-x86)^
else if %flag% == 2 (call :gen2015-x64)^
else if %flag% == 3 (call :gen2017-x86)^
else if %flag% == 4 (call :gen2017-x64)^
else if %flag% == 5 (call :gen2019-x86)^
else if %flag% == 6 (call :gen2019-x64)^
else (echo "输入错误！Input Error!")
GOTO:EOF

:gen2015-x86
mkdir build-win-vs2015-x86
pushd build-win-vs2015-x86
call :cmakeParams "Visual Studio 14 2015" "Win32"
popd
GOTO:EOF

:gen2015-x64
mkdir build-win-vs2015-x64
pushd build-win-vs2015-x64
call :cmakeParams "Visual Studio 14 2015" "x64"
popd
GOTO:EOF

:gen2017-x86
mkdir build-win-vs2017-x86
pushd build-win-vs2017-x86
call :cmakeParams "Visual Studio 15 2017" "Win32"
popd
GOTO:EOF

:gen2017-x64
mkdir build-win-vs2017-x64
pushd build-win-vs2017-x64
call :cmakeParams "Visual Studio 15 2017" "x64"
popd
GOTO:EOF

:gen2019-x86
mkdir build-win-vs2019-x86
pushd build-win-vs2019-x86
call :cmakeParams "Visual Studio 16 2019" "Win32"
popd
GOTO:EOF

:gen2019-x64
mkdir build-win-vs2019-x64
pushd build-win-vs2019-x64
call :cmakeParams "Visual Studio 16 2019" "x64"
popd
GOTO:EOF

:cmakeParams
cmake -G "%~1" -A "%~2" -DCMAKE_BUILD_TYPE=Release -DOCR_LITE_OPENMP=ON ..
GOTO:EOF

@ENDLOCAL
