@ECHO OFF
@SETLOCAL

mkdir build-win
pushd build-win
cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DNCNN_OPENMP=ON -DNCNN_VULKAN=OFF ..
nmake
nmake install
popd

@ENDLOCAL
