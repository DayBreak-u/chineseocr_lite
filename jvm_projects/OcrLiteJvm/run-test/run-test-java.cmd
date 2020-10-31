chcp 65001
:: Set Param
@ECHO OFF
@SETLOCAL
echo "Setting the Number of Threads=%NUMBER_OF_PROCESSORS% Using an OpenMP Environment Variable"
set OMP_NUM_THREADS=%NUMBER_OF_PROCESSORS%

:: run Win x64
java -Djava.library.path=. -Dfile.encoding=UTF-8 -jar OcrLiteJvm.jar models test_imgs/1.jpg
PAUSE
@ENDLOCAL
