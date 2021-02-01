#!/usr/bin/env bash

function cmakeParamsMac(){
  mkdir -p "build-mac-$1"
  pushd "build-mac-$1"
  cmake -DCMAKE_BUILD_TYPE=$1 \
      -DNCNN_OPENMP=ON \
      -DOpenMP_C_FLAGS="-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include" \
      -DOpenMP_CXX_FLAGS="-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include" \
      -DOpenMP_CXX_LIB_NAMES="omp" \
      -DOpenMP_C_LIB_NAMES="omp" \
      -DOpenMP_omp_LIBRARY=/usr/local/opt/libomp/lib/libomp.dylib \
      -DNCNN_BUILD_BENCHMARK=OFF \
      -DNCNN_BUILD_EXAMPLES=OFF \
      -DNCNN_BUILD_TOOLS=OFF \
      -DNCNN_PYTHON=OFF \
      -DNCNN_VULKAN=OFF \
  ..
  make -j $NUM_THREADS
  make install
  popd
}

function cmakeParamsLinux(){
  mkdir -p "build-linux-$1"
  pushd "build-linux-$1"
  cmake -DCMAKE_BUILD_TYPE=$1 \
      -DNCNN_OPENMP=ON \
      -DNCNN_BUILD_BENCHMARK=OFF \
      -DNCNN_BUILD_EXAMPLES=OFF \
      -DNCNN_BUILD_TOOLS=OFF \
      -DNCNN_PYTHON=OFF \
      -DNCNN_VULKAN=OFF \
  ..
  make -j $NUM_THREADS
  make install
}

sysOS=`uname -s`
NUM_THREADS=1
if [ $sysOS == "Darwin" ];then
    #echo "I'm MacOS"
    NUM_THREADS=$(sysctl -n hw.ncpu)
elif [ $sysOS == "Linux" ];then
    #echo "I'm Linux"
    NUM_THREADS=$(grep ^processor /proc/cpuinfo | wc -l)
else
    echo "Other OS: $sysOS"
fi

echo "请选择编译选项并回车: 1)Release，2)Debug"
echo "Please Select Build Type: 1)Release，2)Debug"
read -p "" BUILD_TYPE
if [ $BUILD_TYPE == 0 ]; then
    BUILD_TYPE="Release"
elif [ $BUILD_TYPE == 1 ]; then
    BUILD_TYPE="Release"
elif [ $BUILD_TYPE == 2 ]; then
    BUILD_TYPE="Debug"
else
  echo -e "输入错误！Input Error!"
fi

if [ $sysOS == "Darwin" ];then
    #echo "I'm MacOS"
    cmakeParamsMac $BUILD_TYPE
elif [ $sysOS == "Linux" ];then
    #echo "I'm Linux"
    cmakeParamsLinux $BUILD_TYPE
else
    echo "Other OS: $sysOS"
fi