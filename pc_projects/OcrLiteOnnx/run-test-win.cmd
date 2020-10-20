:: Set Param
@ECHO OFF
@SETLOCAL
chcp 65001
:: run Win x64
copy onnx\windows\onnxruntime.dll build-win /Y
copy opencv\build\x64\vc14\bin\opencv_world3411.dll build-win /Y
pushd build-win
OcrLiteOnnx.exe ../../test/1.jpg ../models 0 0.6 0.3 3 1.3 1.3 1.6 1.6
popd
PAUSE
@ENDLOCAL
