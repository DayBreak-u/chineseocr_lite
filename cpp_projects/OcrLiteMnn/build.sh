#!/usr/bin/env bash

function buildAll() {
  mkdir -p ${sysOS}
  pushd ${sysOS}
  cmake -DCMAKE_BUILD_TYPE=Release  -DOCR_LIB=OFF -DOCR_STATIC=ON ..
  make -j $NUM_THREADS
  popd

  mkdir -p ${sysOS}-Lib
  pushd ${sysOS}-Lib
  cmake -DCMAKE_BUILD_TYPE=Release  -DOCR_LIB=ON -DOCR_STATIC=ON ..
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

echo "使用静态库时，编译出来的可执行文件较大，但部署起来比较方便。"
echo "使用动态库时，编译出来的可执行文件较小，但Linux部署时要配置LD_LIBRARY_PATH或链接|复制到/usr/lib。"
echo "请选择要使用的MNN和Opencv库选项并回车: 1)Static静态库，2)Shared动态库"
read -p "" BUILD_STATIC
if [ $BUILD_STATIC == 1 ]; then
  BUILD_STATIC=ON
elif [ $BUILD_STATIC == 2 ]; then
  BUILD_STATIC=OFF
else
  echo -e "输入错误！Input Error!"
fi

echo "请注意：如果选择2)编译为JNI动态库时，必须安装配置Oracle JDK"
echo "请选择编译输出类型并回车: 1)编译成可执行文件，2)编译成JNI动态库"
read -p "" BUILD_LIB
if [ $BUILD_LIB == 1 ]; then
  BUILD_LIB=OFF
elif [ $BUILD_LIB == 2 ]; then
  BUILD_LIB=ON
else
  echo -e "输入错误！Input Error!"
fi


if [ $BUILD_LIB == OFF ]; then
  mkdir -p build
  pushd build
else
  mkdir -p build-lib
  pushd build-lib
fi

echo "cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DOCR_LIB=${BUILD_LIB} -DOCR_STATIC=${BUILD_STATIC} .."
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE  -DOCR_LIB=$BUILD_LIB -DOCR_STATIC=$BUILD_STATIC ..
make -j $NUM_THREADS
popd