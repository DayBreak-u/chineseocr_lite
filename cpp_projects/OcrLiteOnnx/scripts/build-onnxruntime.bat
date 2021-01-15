:: build onnxruntime 1.6 for windows static (.lib) by benjaminwan
:: build\Windows\Release\onnxruntime.dir\Release\onnxruntime.tlog\link.command.1.tlog
:: use vs2019 add --cmake_generator "Visual Studio 16 2019"
@ECHO OFF
chcp 65001
cls
@SETLOCAL

echo 请选择要生成的选项并回车: 0)ALL，1)Release-x64，2)Debug-x64，1)Release-x86，2)Debug-x86:
echo Please Select Build Type: 0)ALL，1)Release-x64，2)Debug-x64，1)Release-x86，2)Debug-x86:
set /p flag=
if %flag% == 0 (call :buildALL)^
else if %flag% == 1 (call :buildReleaseX64)^
else if %flag% == 2 (call :buildDebugX64)^
else if %flag% == 3 (call :buildReleaseX86)^
else if %flag% == 4 (call :buildDebugX86)^
else (echo "输入错误！Input Error!")
GOTO:EOF

:buildALL
call :buildReleaseX64
call :buildDebugX64
call :buildReleaseX86
call :buildDebugX86
GOTO:EOF

:buildReleaseX64
call build.bat --build_dir Release-x64 --skip_submodule_sync --config Release --build_shared_lib --parallel
GOTO:EOF

:buildDebugX64
call build.bat --build_dir Debug-x64 --skip_submodule_sync --config Debug --build_shared_lib --parallel
GOTO:EOF

:buildReleaseX86
call build.bat --build_dir Release-x86 --skip_submodule_sync --config Release --build_shared_lib --parallel --x86
GOTO:EOF

:buildDebugX86
call build.bat --build_dir Debug-x86 --skip_submodule_sync --config Debug --build_shared_lib --parallel --x86
GOTO:EOF