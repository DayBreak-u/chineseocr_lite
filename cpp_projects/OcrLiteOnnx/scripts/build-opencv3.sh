#!/usr/bin/env bash
# build opencv 3.4.x by benjaminwan

function cmakeParams(){
  mkdir -p "build-$1"
  pushd "build-$1"
  cmake -DCMAKE_BUILD_TYPE=$1 -DCMAKE_CONFIGURATION_TYPES=$1\
  -DCMAKE_INSTALL_PREFIX=install \
  -DBUILD_DOCS=OFF \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_PERF_TESTS=OFF \
  -DBUILD_PROTOBUF=OFF \
  -DBUILD_SHARED_LIBS=$BUILD_SHARED \
  -DBUILD_TESTS=OFF \
  -DBUILD_WITH_DEBUG_INFO=OFF \
  -DBUILD_WITH_STATIC_CRT=OFF \
  -DBUILD_IPP_IW=ON \
  -DBUILD_ITT=ON \
  -DBUILD_JASPER=ON \
  -DBUILD_JPEG=ON \
  -DBUILD_PNG=ON \
  -DBUILD_TBB=ON \
  -DBUILD_TIFF=ON \
  -DBUILD_WEBP=ON \
  -DBUILD_ZLIB=ON \
  -DWITH_EIGEN=OFF \
  -DWITH_FFMPEG=OFF \
  -DBUILD_opencv_dnn=OFF \
  -DBUILD_opencv_world=ON \
  ..
  make -j $NUM_THREADS
  make install
  popd
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

echo "请选择生成库选项并回车: 1)Shared动态库，2)Static静态库"
echo "Please Select Lib Type: 1)Shared Lib，2)Static Lib"
read -t 30 -p "" BUILD_SHARED
if [ $BUILD_SHARED == "" ]; then
    BUILD_SHARED=ON
elif [ $BUILD_SHARED == 1 ]; then
    BUILD_SHARED=ON
elif [ $BUILD_SHARED == 2 ]; then
    BUILD_SHARED=OFF
else
  echo -e "输入错误！Input Error!"
fi

echo "请选择编译选项并回车: 1)Release，2)Debug"
echo "Please Select Build Type: 1)Release，2)Debug"
read -t 30 -p "" BUILD_TYPE
if [ $BUILD_TYPE == 0 ]; then
    cmakeParams "Release"
elif [ $BUILD_TYPE == 1 ]; then
    cmakeParams "Release"
elif [ $BUILD_TYPE == 2 ]; then
    cmakeParams "Debug"
else
  echo -e "输入错误！Input Error!"
fi

