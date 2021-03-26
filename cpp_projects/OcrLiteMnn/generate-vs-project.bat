@ECHO OFF
chcp 65001
cls
@SETLOCAL
echo "========请先参考README.md准备好编译环境========"
echo.

echo "========编译选项========"
echo "请注意：项目默认使用Release库，除非您自行编译Debug版的MNN和Opencv，否则请不要选择Debug编译"
echo "请输入编译选项并回车: 1)Release, 2)Debug"
set BUILD_TYPE=Release
set /p flag=
if %flag% == 1 (set BUILD_TYPE=Release)^
else if %flag% == 2 (set BUILD_TYPE=Debug)^
else (echo 输入错误！Input Error!)
echo.

echo "请输入OpenMP选项并回车: 1)启用OpenMP(Angle阶段和Crnn阶段多线程并行执行), 2)禁用OpenMP(Angle阶段和Crnn阶段单线程执行)"
set BUILD_OPENMP=ON
set /p flag=
if %flag% == 1 (set BUILD_OPENMP=ON)^
else if %flag% == 2 (set BUILD_OPENMP=OFF)^
else (echo 输入错误！Input Error!)
echo.

echo "使用静态库时，编译出来的可执行文件较大，但部署起来比较方便。"
echo "使用动态库时，编译出来的可执行文件较小，但部署的时候记得把dll复制到可执行文件目录"
echo "请选择要使用的MNN和Opencv库选项并回车: 1)Static静态库，2)Shared动态库"
set BUILD_STATIC=ON
set /p flag=
if %flag% == 1 (set BUILD_STATIC=ON)^
else if %flag% == 2 (set BUILD_STATIC=OFF)^
else (echo "输入错误！Input Error!")
echo.

echo "请注意：如果选择2)编译为JNI动态库时，必须安装配置Oracle JDK"
echo "请选择编译输出类型并回车: 1)编译成可执行文件，2)编译成JNI动态库"
set BUILD_LIB=OFF
set /p flag=
if %flag% == 1 (set BUILD_LIB=OFF)^
else if %flag% == 2 (set BUILD_LIB=ON)^
else (echo 输入错误！Input Error!)
echo.

echo "请输入选项并回车: 0)ALL, 1)vs2017-x86, 2)vs2017-x64, 3)vs2019-x86, 4)vs2019-x64:"
set /p flag=
if %flag% == 0 (call :buildALL)^
else if %flag% == 1 (call :gen2017-x86)^
else if %flag% == 2 (call :gen2017-x64)^
else if %flag% == 3 (call :gen2019-x86)^
else if %flag% == 4 (call :gen2019-x64)^
else (echo "输入错误！Input Error!")
GOTO:EOF

:buildALL
call :gen2017-x86
call :gen2017-x64
call :gen2019-x86
call :gen2019-x64
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
echo cmake -G "%~1" -A "%~2" -DOCR_OPENMP=%BUILD_OPENMP% -DOCR_LIB=%BUILD_LIB% -DOCR_STATIC=%BUILD_STATIC% ..
cmake -G "%~1" -A "%~2" -DOCR_OPENMP=%BUILD_OPENMP% -DOCR_LIB=%BUILD_LIB% -DOCR_STATIC=%BUILD_STATIC% ..
GOTO:EOF

@ENDLOCAL
