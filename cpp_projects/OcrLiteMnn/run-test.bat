chcp 65001
:: Set Param
@ECHO OFF
@SETLOCAL
echo "Setting the Number of Threads=%NUMBER_OF_PROCESSORS% Using an OpenMP Environment Variable"
set OMP_NUM_THREADS=%NUMBER_OF_PROCESSORS%

SET TARGET_IMG=../images/1.jpg
if not exist %TARGET_IMG% (
echo "找不到待识别的目标图片：%TARGET_IMG%，请打开本文件并编辑TARGET_IMG"
PAUSE
exit
)

:: run Windows
build\OcrLiteMnn.exe --version
build\OcrLiteMnn.exe --models models ^
--det dbnet.mnn ^
--cls angle_net.mnn ^
--rec crnn_lite_lstm.mnn ^
--keys keys.txt ^
--image %TARGET_IMG% ^
--numThread %NUMBER_OF_PROCESSORS% ^
--padding 50 ^
--maxSideLen 1024 ^
--boxScoreThresh 0.6 ^
--boxThresh 0.3 ^
--unClipRatio 2.0 ^
--doAngle 1 ^
--mostAngle 1
PAUSE
@ENDLOCAL
