chcp 65001
:: Set Param
@ECHO OFF
@SETLOCAL
echo "Setting the Number of Threads=%NUMBER_OF_PROCESSORS% Using an OpenMP Environment Variable"
set OMP_NUM_THREADS=%NUMBER_OF_PROCESSORS%

:: run Win x64
copy onnx\windows\onnxruntime.dll build-win /Y
copy opencv\build\x64\vc14\bin\opencv_world3411.dll build-win /Y
pushd build
OcrLiteOnnx.exe --models ../models --image ../../test_imgs/1.jpg ^
                --numThread $NUM_THREADS --padding 50 --imgResize 0 ^
                --boxScoreThresh 0.6 --boxThresh 0.3 --minArea 3 ^
                --unClipRatio 2.0  --doAngle 1 --mostAngle 1
popd
PAUSE
@ENDLOCAL
