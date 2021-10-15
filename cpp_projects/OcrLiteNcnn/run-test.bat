chcp 65001
:: Set Param
@ECHO OFF
@SETLOCAL
echo "Setting the Number of Threads=%NUMBER_OF_PROCESSORS% Using an OpenMP Environment Variable"
set OMP_NUM_THREADS=%NUMBER_OF_PROCESSORS%

:MainExec
echo "Gpu版本测试前请先安装Vulkan SDK v1.2.162.0，https://vulkan.lunarg.com/sdk/home"
echo "请输入测试选项并回车: 1)CPU-x64, 2)CPU-x86, 3)GPU-x64, 4)GPU-x86"
set GPU_INDEX=-1
set /p flag=
if %flag% == 1 (call :PrepareCpuX64)^
else if %flag% == 2 (call :PrepareCpuX86)^
else if %flag% == 3 (call :PrepareGpuX64)^
else if %flag% == 4 (call :PrepareGpuX86)^
else (echo 输入错误！Input Error!)

SET TARGET_IMG=images/1.jpg
if not exist %TARGET_IMG% (
echo "找不到待识别的目标图片：%TARGET_IMG%，请打开本文件并编辑TARGET_IMG"
PAUSE
exit
)

if exist %EXE_PATH%\install\bin (
SET EXE_PATH=%EXE_PATH%\install\bin
)

%EXE_PATH%\OcrLiteNcnn.exe --version
%EXE_PATH%\OcrLiteNcnn.exe --models models ^
--det dbnet_op ^
--cls angle_op ^
--rec crnn_lite_op ^
--keys keys.txt ^
--image %TARGET_IMG% ^
--numThread %NUMBER_OF_PROCESSORS% ^
--padding 50 ^
--maxSideLen 0 ^
--boxScoreThresh 0.6 ^
--boxThresh 0.3 ^
--unClipRatio 2.0 ^
--doAngle 1 ^
--mostAngle 1 ^
--GPU %GPU_INDEX%

echo.
GOTO:MainExec

:PrepareCpuX64
set EXE_PATH=win-BIN-CPU-x64
set GPU_INDEX=-1
GOTO:EOF

:PrepareCpuX86
set EXE_PATH=win-BIN-CPU-Win32
set GPU_INDEX=-1
GOTO:EOF

:PrepareGpuX64
set EXE_PATH=win-BIN-GPU-x64
set GPU_INDEX=0
GOTO:EOF

:PrepareGpuX86
set EXE_PATH=win-BIN-GPU-Win32
set GPU_INDEX=0
GOTO:EOF

@ENDLOCAL
