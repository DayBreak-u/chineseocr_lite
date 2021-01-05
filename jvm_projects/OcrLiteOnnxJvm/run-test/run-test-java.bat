chcp 65001
:: Set Param
@ECHO OFF
@SETLOCAL
echo "Setting the Number of Threads=%NUMBER_OF_PROCESSORS% Using an OpenMP Environment Variable"
set OMP_NUM_THREADS=%NUMBER_OF_PROCESSORS%

:: run Windows
java -Djava.library.path=. -Dfile.encoding=UTF-8 -jar OcrLiteOnnxJvm.jar models test_imgs/1.jpg ^
%NUMBER_OF_PROCESSORS% ^
50 ^
0 ^
0.6 ^
0.3 ^
3 ^
2.0 ^
1 ^
1

:: numThread
:: padding
:: imgResize
:: boxScoreThresh
:: boxThresh
:: minArea
:: unClipRatio
:: doAngle
:: mostAngle

PAUSE
@ENDLOCAL
