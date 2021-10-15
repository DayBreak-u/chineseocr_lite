#!/usr/bin/env bash
# build opencv 3.5.x base on https://github.com/nihui/opencv-mobile/blob/master/opencv4_cmake_options.txt

export ANDROID_NDK=/Users/mywork/android-sdk-macosx/ndk-bundle

function cmakeParams() {
  mkdir -p "build-$1"
  pushd "build-$1"
  cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release \
    -DCMAKE_INSTALL_PREFIX=install \
    -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI="$1" -DANDROID_ARM_NEON=ON -DANDROID_CPP_FEATURES="no-rtti no-exceptions" \
    $(cat ../../opencv3_cmake_options.txt) -DBUILD_opencv_world=OFF \
    ..
  cmake --build . --config Release -j $NUM_THREADS
  cmake --build . --config Release --target install
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

# build
cmakeParams "armeabi-v7a"
cmakeParams "arm64-v8a"
cmakeParams "x86"
cmakeParams "x86_64"

# pack
mkdir opencv-mobile-3.4.15-android
cp -rf build-x86/install/* opencv-mobile-3.4.15-android/
cp -rf build-x86_64/install/* opencv-mobile-3.4.15-android/
cp -rf build-armeabi-v7a/install/* opencv-mobile-3.4.15-android/
cp -rf build-arm64-v8a/install/* opencv-mobile-3.4.15-android/
rm -rf opencv-mobile-3.4.15-android/apk
rm -rf opencv-mobile-3.4.15-android/sdk/etc