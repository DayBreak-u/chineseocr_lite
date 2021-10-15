#!/usr/bin/env bash

echo "========请先参考README.md准备好编译环境========"
echo

echo "========编译选项========"
echo "请输入编译选项并回车: 1)Release, 2)Debug"
read -p "" BUILD_TYPE
if [ $BUILD_TYPE == 1 ]; then
  BUILD_TYPE=Release
elif [ $BUILD_TYPE == 2 ]; then
  BUILD_TYPE=Debug
else
  echo -e "输入错误！Input Error!"
fi

echo "请注意：如果选择2)JNI动态库时，必须安装配置Oracle JDK"
echo "请选择编译输出类型并回车: 1)BIN可执行文件，2)JNI动态库，3)C动态库"
read -p "" BUILD_OUTPUT
if [ $BUILD_OUTPUT == 1 ]; then
  BUILD_OUTPUT="BIN"
elif [ $BUILD_OUTPUT == 2 ]; then
  BUILD_OUTPUT="JNI"
elif [ $BUILD_OUTPUT == 3 ]; then
  BUILD_OUTPUT="CLIB"
else
  echo -e "输入错误！Input Error!"
fi

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

mkdir -p $sysOS-$BUILD_OUTPUT
pushd $sysOS-$BUILD_OUTPUT

echo "cmake -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DOCR_OUTPUT=$BUILD_OUTPUT .."
cmake -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DOCR_OUTPUT=$BUILD_OUTPUT ..
cmake --build . --config $BUILD_TYPE -j $NUM_THREADS
cmake --build . --config $BUILD_TYPE --target install
popd
