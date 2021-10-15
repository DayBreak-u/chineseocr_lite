#!/bin/bash
# build onnxruntime 1.6~1.9 by benjaminwan
# CMakeFiles/onnxruntime.dir/link.txt link lib*.a

function cmakeParams(){
./build.sh --build_dir "$1_$2_$3" --parallel --config $1 --skip_tests \
--build_shared_lib --cmake_extra_defines CMAKE_INSTALL_PREFIX=./install \
--android --android_sdk_path /Users/mywork/android-sdk-macosx \
--android_ndk_path /Users/mywork/android-sdk-macosx/ndk-bundle \
--android_abi $2 \
--android_api $3 \
--target install
}

echo "请选择编译选项并回车: 1)Release，2)Debug"
echo "Please Select Build Type: 1)Release，2)Debug"
read -p "" BUILD_TYPE
if [ $BUILD_TYPE == 0 ]; then
    echo
elif [ $BUILD_TYPE == 1 ]; then
    cmakeParams "Release" "armeabi-v7a" 19
    cmakeParams "Release" "arm64-v8a" 21
    cmakeParams "Release" "x86" 19
    cmakeParams "Release" "x86_64" 21
elif [ $BUILD_TYPE == 2 ]; then
    cmakeParams "Debug" "armeabi-v7a" 19
    cmakeParams "Debug" "arm64-v8a" 21
    cmakeParams "Debug" "x86" 19
    cmakeParams "Debug" "x86_64" 21
else
  echo -e "输入错误！Input Error!"
fi