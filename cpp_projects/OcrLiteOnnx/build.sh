#!/usr/bin/env bash

function buildAll() {
  mkdir -p ${sysOS}
  pushd ${sysOS}
  cmake -DCMAKE_BUILD_TYPE=Release -DOCR_OPENMP=OFF -DOCR_JNI=OFF -DOCR_CLIB=OFF -DOCR_STATIC=ON -DONNXRUNTIME_STATIC=OFF ..
  make -j $NUM_THREADS
  popd

  mkdir -p ${sysOS}-Jni
  pushd ${sysOS}-Jni
  cmake -DCMAKE_BUILD_TYPE=Release -DOCR_OPENMP=OFF -DOCR_JNI=ON -DOCR_CLIB=OFF -DOCR_STATIC=ON -DONNXRUNTIME_STATIC=OFF ..
  make -j $NUM_THREADS
  popd
  
  mkdir -p ${sysOS}-CLib
  pushd ${sysOS}-CLib
  cmake -DCMAKE_BUILD_TYPE=Release -DOCR_OPENMP=OFF -DOCR_JNI=OFF -DOCR_CLIB=ON -DOCR_STATIC=ON -DONNXRUNTIME_STATIC=OFF ..
  make -j $NUM_THREADS
  popd
}

sysOS=$(uname -s)
NUM_THREADS=1
if [ $sysOS == "Darwin" ]; then
  #echo "I'm MacOS"
  NUM_THREADS=$(sysctl -n hw.ncpu)
elif [ $sysOS == "Linux" ]; then
  #echo "I'm Linux"
  NUM_THREADS=$(grep ^processor /proc/cpuinfo | wc -l)
else
  echo "Other OS: $sysOS"
fi

echo "========请先参考README.md准备好编译环境========"
echo

echo "========编译选项========"
echo "请输入编译选项并回车: 1)Release, 2)Debug, 3)预设"
read -p "" BUILD_TYPE
if [ $BUILD_TYPE == 1 ]; then
  BUILD_TYPE=Release
elif [ $BUILD_TYPE == 2 ]; then
  BUILD_TYPE=Debug
elif [ $BUILD_TYPE == 3 ]; then
  buildAll
  exit
else
  echo -e "输入错误！Input Error!"
fi

echo "请输入OpenMP选项并回车: 1)启用OpenMP(Angle阶段和Crnn阶段多线程并行执行), 2)禁用OpenMP(Angle阶段和Crnn阶段单线程执行)"
read -p "" BUILD_OPENMP
if [ $BUILD_OPENMP == 1 ]; then
  BUILD_OPENMP=ON
elif [ $BUILD_OPENMP == 2 ]; then
  BUILD_OPENMP=OFF
else
  echo -e "输入错误！Input Error!"
fi

echo "使用静态库时，编译出来的可执行文件较大，但部署起来比较方便。"
echo "使用动态库时，编译出来的可执行文件较小，但Linux部署时要配置LD_LIBRARY_PATH或链接|复制到/usr/lib。"
echo "请选择要使用的OnnxRuntime和Opencv库选项并回车: 1)Static静态库，2)Shared动态库，3)onnxruntime静态，opencv动态"
read -p "" BUILD_STATIC
BUILD_ONNXRUNTIME_STATIC=OFF
if [ $BUILD_STATIC == 1 ]; then
  BUILD_STATIC=ON
elif [ $BUILD_STATIC == 2 ]; then
  BUILD_STATIC=OFF
elif [ $BUILD_STATIC == 3 ]; then
  BUILD_STATIC=OFF
  BUILD_ONNXRUNTIME_STATIC=ON
else
  echo -e "输入错误！Input Error!"
fi

echo "请注意：如果选择2)编译为JNI动态库时，必须安装配置Oracle JDK"
echo "请选择编译输出类型并回车: 1)编译成可执行文件，2)编译成JNI动态库，3)编译成C层动态库"
read -p "" BUILD_JNI
BUILD_CLIB=OFF
if [ $BUILD_JNI == 1 ]; then
  BUILD_JNI=OFF
elif [ $BUILD_JNI == 2 ]; then
  BUILD_JNI=ON
elif [ $BUILD_JNI == 3 ]; then
  BUILD_JNI=OFF
  BUILD_CLIB=ON
else
  echo -e "输入错误！Input Error!"
fi

if [ $BUILD_JNI == ON ]; then
  mkdir -p build-jni
  pushd build-jni
elif [ $BUILD_CLIB == ON ]; then
  mkdir -p build-clib
  pushd build-clib
else
  mkdir -p build
  pushd build
fi

echo "cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DOCR_OPENMP=${BUILD_OPENMP} -DOCR_JNI=${BUILD_JNI} -DOCR_CLIB=${BUILD_CLIB} -DOCR_STATIC=${BUILD_STATIC} -DONNXRUNTIME_STATIC=${BUILD_ONNXRUNTIME_STATIC} .."
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DOCR_OPENMP=$BUILD_OPENMP -DOCR_JNI=$BUILD_JNI -DOCR_CLIB=$BUILD_CLIB -DOCR_STATIC=$BUILD_STATIC -DONNXRUNTIME_STATIC=$BUILD_ONNXRUNTIME_STATIC ..
make -j $NUM_THREADS
popd
