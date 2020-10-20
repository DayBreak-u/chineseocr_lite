:: Set Param
@ECHO OFF
@SETLOCAL

:: build Win x64
mkdir build-win
pushd build-win
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
nmake
popd

@ENDLOCAL
