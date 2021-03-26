@ECHO OFF
chcp 65001
cls
@SETLOCAL
echo "========请先参考README.md准备好编译环境========"
echo.

echo "========编译选项========"
echo "请注意：项目默认使用Release库，除非您自行编译Debug版的MNN和Opencv，否则请不要选择Debug编译"
echo "请输入编译选项并回车: 1)Release, 2)Debug, 3)预设"
set BUILD_TYPE=Release
set /p flag=
if %flag% == 1 (set BUILD_TYPE=Release)^
else if %flag% == 2 (set BUILD_TYPE=Debug)^
else if %flag% == 3 (goto :makeAllExe)^
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
if %BUILD_LIB% == OFF (call :makeExe)^
else (call :makeLib)
echo cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DOCR_LIB=%BUILD_LIB% -DOCR_STATIC=%BUILD_STATIC% ..
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%  -DOCR_LIB=%BUILD_LIB% -DOCR_STATIC=%BUILD_STATIC% ..
nmake
popd
GOTO:EOF

:makeExe
mkdir build
pushd build
GOTO:EOF

:makeLib
mkdir build-lib
pushd build-lib
GOTO:EOF

:makeAllExe
mkdir win-%VSCMD_ARG_TGT_ARCH%
pushd win-%VSCMD_ARG_TGT_ARCH%
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release  -DOCR_LIB=OFF -DOCR_STATIC=ON ..
nmake
popd

mkdir win-lib-%VSCMD_ARG_TGT_ARCH%
pushd win-lib-%VSCMD_ARG_TGT_ARCH%
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release  -DOCR_LIB=ON -DOCR_STATIC=ON ..
nmake
popd

@ENDLOCAL
