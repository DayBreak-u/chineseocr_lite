chcp 65001
:: Set Param
@ECHO OFF
@SETLOCAL
echo "Setting the Number of Threads=%NUMBER_OF_PROCESSORS% Using an OpenMP Environment Variable"
set OMP_NUM_THREADS=%NUMBER_OF_PROCESSORS%

:: run Win x64
pushd build
OcrLiteNcnn.exe --models ../models --image ../../test_imgs/1.jpg ^
                --numThread %NUMBER_OF_PROCESSORS% --padding 50 --imgResize 0 ^
                --boxScoreThresh 0.6 --boxThresh 0.3 --minArea 3 ^
                --unClipRatio 2.0  --doAngle 1 --mostAngle 1 -G 0
popd
PAUSE
@ENDLOCAL
