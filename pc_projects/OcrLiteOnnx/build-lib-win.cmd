:: Set Param
@ECHO OFF
@SETLOCAL

:: build Win x64
mkdir build-lib-win
copy onnx\windows\onnxruntime.dll build-lib-win /Y
copy opencv\build\x64\vc14\bin\opencv_world3411.dll build-lib-win /Y
pushd build-lib-win
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DOCR_LITE_LIB=ON ..
nmake
popd

@ENDLOCAL
