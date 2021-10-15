@ECHO OFF
chcp 65001
cls
@SETLOCAL

mkdir win-BIN-CPU-x64
pushd win-BIN-CPU-x64
cmake -T "v141,host=x64" -A "x64" ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_BUILD_TYPE=Release -DOCR_OUTPUT="BIN" -DOCR_VULKAN="CPU" ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
cmake --build . --config Release --target install
popd

mkdir win-BIN-CPU-Win32
pushd win-BIN-CPU-Win32
cmake -T "v141,host=x64" -A "Win32" ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_BUILD_TYPE=Release -DOCR_OUTPUT="BIN" -DOCR_VULKAN="CPU" ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
cmake --build . --config Release --target install
popd

mkdir win-JNI-CPU-x64
pushd win-JNI-CPU-x64
cmake -T "v141,host=x64" -A "x64" ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_BUILD_TYPE=Release -DOCR_OUTPUT="JNI" -DOCR_VULKAN="CPU" ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
cmake --build . --config Release --target install
popd

mkdir win-JNI-CPU-Win32
pushd win-JNI-CPU-Win32
cmake -T "v141,host=x64" -A "Win32" ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_BUILD_TYPE=Release -DOCR_OUTPUT="JNI" -DOCR_VULKAN="CPU" ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
cmake --build . --config Release --target install
popd

mkdir win-CLIB-CPU-x64
pushd win-CLIB-CPU-x64
cmake -T "v141,host=x64" -A "x64" ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_BUILD_TYPE=Release -DOCR_OUTPUT="CLIB" -DOCR_VULKAN="CPU" ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
cmake --build . --config Release --target install
popd

mkdir win-CLIB-CPU-Win32
pushd win-CLIB-CPU-Win32
cmake -T "v141,host=x64" -A "Win32" ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_BUILD_TYPE=Release -DOCR_OUTPUT="CLIB" -DOCR_VULKAN="CPU" ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
cmake --build . --config Release --target install
popd

mkdir win-BIN-GPU-x64
pushd win-BIN-GPU-x64
cmake -T "v141,host=x64" -A "x64" ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_BUILD_TYPE=Release -DOCR_OUTPUT="BIN" -DOCR_VULKAN="GPU" ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
cmake --build . --config Release --target install
popd

mkdir win-BIN-GPU-Win32
pushd win-BIN-GPU-Win32
cmake -T "v141,host=x64" -A "Win32" ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_BUILD_TYPE=Release -DOCR_OUTPUT="BIN" -DOCR_VULKAN="GPU" ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
cmake --build . --config Release --target install
popd

mkdir win-JNI-GPU-x64
pushd win-JNI-GPU-x64
cmake -T "v141,host=x64" -A "x64" ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_BUILD_TYPE=Release -DOCR_OUTPUT="JNI" -DOCR_VULKAN="GPU" ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
cmake --build . --config Release --target install
popd

mkdir win-JNI-GPU-Win32
pushd win-JNI-GPU-Win32
cmake -T "v141,host=x64" -A "Win32" ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_BUILD_TYPE=Release -DOCR_OUTPUT="JNI" -DOCR_VULKAN="GPU" ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
cmake --build . --config Release --target install
popd

mkdir win-CLIB-GPU-x64
pushd win-CLIB-GPU-x64
cmake -T "v141,host=x64" -A "x64" ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_BUILD_TYPE=Release -DOCR_OUTPUT="CLIB" -DOCR_VULKAN="GPU" ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
cmake --build . --config Release --target install
popd

mkdir win-CLIB-GPU-Win32
pushd win-CLIB-GPU-Win32
cmake -T "v141,host=x64" -A "Win32" ^
  -DCMAKE_INSTALL_PREFIX=install ^
  -DCMAKE_BUILD_TYPE=Release -DOCR_OUTPUT="CLIB" -DOCR_VULKAN="GPU" ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
cmake --build . --config Release --target install
popd

@ENDLOCAL
