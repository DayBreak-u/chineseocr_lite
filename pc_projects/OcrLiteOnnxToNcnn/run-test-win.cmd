:: Set Param
@ECHO OFF
@SETLOCAL
chcp 65001
:: run Win x64
copy opencv\build\x64\vc14\bin\opencv_world3411.dll build-win /Y
pushd build-win
OcrLiteOnnxToNcnn.exe ../../test_imgs/1.jpg ../models 0 0.6 0.3 3 1.3 1.3 1.6 1.6
popd
PAUSE
@ENDLOCAL
