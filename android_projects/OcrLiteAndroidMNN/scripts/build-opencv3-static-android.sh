#!/usr/bin/env bash

# build opencv 3.4.11 for android static (.a) by benjaminwan 2020.8.21

export ANDROID_NDK=/path/to/your/ndk/17.2.4988734

cmakeParams(){
  cmake -DCMAKE_BUILD_TYPE=RELEASE\
  -DANDROID_ABI=$1 \
  -DANDROID_ARM_NEON=ON \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
  -DBUILD_ANDROID_PROJECTS=OFF \
  -DBUILD_ANDROID_EXAMPLES=OFF \
  -DBUILD_CUDA_STUBS=OFF \
  -DBUILD_DOCS=OFF \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_IPP_IW=OFF \
  -DBUILD_ITT=OFF \
  -DBUILD_JASPER=OFF \
  -DBUILD_JAVA=OFF \
  -DBUILD_JPEG=OFF \
  -DBUILD_LIST=OFF \
  -DBUILD_OPENEXR=OFF \
  -DBUILD_PACKAGE=OFF \
  -DBUILD_PERF_TESTS=OFF \
  -DBUILD_PNG=OFF \
  -DBUILD_PROTOBUF=OFF \
  -DBUILD_SHARED_LIBS=OFF \
  -DBUILD_TBB=OFF \
  -DBUILD_TESTS=OFF \
  -DBUILD_TIFF=OFF \
  -DBUILD_USE_SYMLINKS=OFF \
  -DBUILD_WEBP=OFF \
  -DBUILD_WITH_DEBUG_INFO=OFF \
  -DBUILD_WITH_DYNAMIC_IPP=OFF \
  -DBUILD_ZLIB=OFF \
  -DBUILD_opencv_apps=OFF \
  -DBUILD_opencv_calib3d=OFF \
  -DBUILD_opencv_core=ON \
  -DBUILD_opencv_dnn=OFF \
  -DBUILD_opencv_features2d=OFF \
  -DBUILD_opencv_flann=OFF \
  -DBUILD_opencv_highgui=ON \
  -DBUILD_opencv_imgcodecs=ON \
  -DBUILD_opencv_imgproc=ON \
  -DBUILD_opencv_java_bindings_generator=OFF \
  -DBUILD_opencv_js=OFF \
  -DBUILD_opencv_ml=OFF \
  -DBUILD_opencv_objdetect=OFF \
  -DBUILD_opencv_photo=OFF \
  -DBUILD_opencv_python2=OFF \
  -DBUILD_opencv_python_bindings_generator=OFF \
  -DBUILD_opencv_python_tests=OFF \
  -DBUILD_opencv_shape=OFF \
  -DBUILD_opencv_stitching=OFF \
  -DBUILD_opencv_superres=OFF \
  -DBUILD_opencv_ts=OFF \
  -DBUILD_opencv_video=OFF \
  -DBUILD_opencv_videoio=OFF \
  -DBUILD_opencv_videostab=OFF \
  -DBUILD_opencv_world=ON \
  -DWITH_1394=OFF \
  -DWITH_ARITH_DEC=OFF \
  -DWITH_ARITH_ENC=OFF \
  -DWITH_AVFOUNDATION=OFF \
  -DWITH_EIGEN=OFF \
  -DWITH_FFMPEG=OFF \
  -DWITH_GSTREAMER=OFF \
  -DWITH_IMGCODEC_HDR=OFF \
  -DWITH_IMGCODEC_PXM=OFF \
  -DWITH_IMGCODEC_SUNRASTER=OFF \
  -DWITH_IPP=OFF \
  -DWITH_ITT=OFF \
  -DWITH_JASPER=OFF \
  -DWITH_JPEG=OFF \
  -DWITH_LAPACK=OFF \
  -DWITH_OPENEXR=OFF \
  -DWITH_PNG=OFF \
  -DWITH_PROTOBUF=OFF \
  -DWITH_PTHREADS_PF=OFF \
  -DWITH_QUIRC=OFF \
  -DWITH_TIFF=OFF \
  -DWITH_VTK=OFF \
  -DWITH_WEBP=OFF \
  ..
  make -j8
  make install
}

##### android armv7
mkdir -p build-android-armv7
pushd build-android-armv7
cmakeParams "armeabi-v7a"
popd

##### android arm64-v8a
mkdir -p build-android-arm64v8
pushd build-android-arm64v8
cmakeParams "arm64-v8a"
popd

##### android x86
mkdir -p build-android-x86
pushd build-android-x86
cmakeParams "x86"
popd

##### android x86_64
mkdir -p build-android-x86_64
pushd build-android-x86_64
cmakeParams "x86_64"
popd