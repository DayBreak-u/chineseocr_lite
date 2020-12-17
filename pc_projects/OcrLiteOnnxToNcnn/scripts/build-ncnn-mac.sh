#!/usr/bin/env bash

#brew install libomp
#export VULKAN_SDK=/~/vulkansdk-macos-1.1.114.0/macOS

##### MacOS
mkdir -p build-mac
pushd build-mac
cmake -DCMAKE_BUILD_TYPE=Release -DNCNN_OPENMP=ON \
      -DOpenMP_C_FLAGS="-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include" \
      -DOpenMP_CXX_FLAGS="-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include" \
      -DOpenMP_CXX_LIB_NAMES="omp" \
      -DOpenMP_C_LIB_NAMES="omp" \
      -DOpenMP_omp_LIBRARY=/usr/local/opt/libomp/lib/libomp.dylib \
      -DNCNN_VULKAN=OFF \
      ..
make -j 8
make install
popd
