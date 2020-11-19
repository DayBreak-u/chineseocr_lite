# ChOcrLiteAndroidOnnxToNcnn

#### Demo APK下载
[Gitee下载](https://gitee.com/benjaminwan/ch-ocr-lite-android-onnx-to-ncnn/releases)

#### 介绍
Chineseocr Lite Android Onnx To Ncnn Demo，超轻量级中文OCR Android Demo，支持ncnn推理 (DBNet+AngleNet+CRNN)
把onnx模型格式转为ncnn格式，并使用ncnn推理框架进行OCR识别。
**代码和模型均源自chineseocr lite的onnx分支**

详情请查看 [https://github.com/ouyanghuiyu/chineseocr_lite](https://github.com/ouyanghuiyu/chineseocr_lite)

采用ncnn神经网络前向计算框架[https://github.com/Tencent/ncnn](https://github.com/Tencent/ncnn)

整合了如下算法：
dbnet(图像分割)+anglenet(文字方向检测)+crnn(文字识别)
#### 说明
1. 封装为独立的Library。
2. Native层以C++编写。
3. demo app以Kotlin-JVM编写。
4. opencv动态库版本opencv-3.4.10-android-sdk.zip
5. 预编译库版本ncnn-android-lib.zip 20200916 b766c8c

#### 编译说明
1.  AndroidStudio 4.1或以上
2.  NDK
3.  cmake 3.4.1或以上
4.  下载opencv-3.4.10-android-sdk.zip，[下载地址](https://github.com/opencv/opencv/releases/tag/3.4.11)
解压后目录结构为
```
ChOcrLiteAndroidOnnx/OcrLibrary/src/main/sharedLibs
├── arm64-v8a
│   └── libopencv_java3.so
├── armeabi-v7a
│   └── libopencv_java3.so
├── x86
│   └── libopencv_java3.so
└── x86_64
    └── libopencv_java3.so
```
5.  [模型下载地址](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/models_ncnn)
```
ChOcrLiteAndroidOnnxToNcnn/OcrLibrary/src/main/assets
├── angle_op.bin
├── angle_op.param
├── crnn_lite_op.bin
├── crnn_lite_op.param
├── dbnet_op.bin
├── dbnet_op.param
└── keys.txt
```
6.  ncnn预编译库版本为 20200916 b766c8c，文件名:ncnn-android-lib.zip，[下载地址](https://github.com/Tencent/ncnn/releases/tag/20200916)
```
ChOcrLiteAndroidOnnxToNcnn/OcrLibrary/src/main/staticLibs
├── arm64-v8a
│   └── libncnn.a
├── armeabi-v7a
│   └── libncnn.a
├── x86
│   └── libncnn.a
└── x86_64
    └── libncnn.a

```


#### onnx版本
[https://gitee.com/benjaminwan/ch-ocr-lite-android-onnx](https://gitee.com/benjaminwan/ch-ocr-lite-android-onnx)

#### 输入参数说明
请参考[OcrLiteOnnx项目](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/pc_projects/OcrLiteOnnx)

#### 编译opencv3，裁剪不需要的模块，减小包体积
* 也可以直接下载编译好的sdk包，在顶部的下载地址内找opencv-3.4.11-android-sdk-static.zip。
* 因为只是用opencv做一些图像处理和变换，所以不需要它自带的推理模块，video模块等等，去掉这些模块后，最终的apk包体积能减少40MB
* 同步opencv源码，如果github访问很慢，可以使用镜像站hub.fastgit.org或者通过https://gitclone.com/缓存加速

##### gitclone缓存加速
```
下载安装cgit(推荐)
cgit clone https://github.com/opencv/opencv.git

或者
git config --global url."https://gitclone.com/github.com".insteadOf https://github.com
git clone https://github.com/opencv/opencv.git
```

##### 检出opencv3.4.11
```
cd opencv
git checkout 3.4.11
```

##### 准备用于编译静态库的shell脚本(仅linux或mac)
* scrips文件夹里的build-opencv3-static-android.sh复制到opencv文件夹，其中ANDROID_NDK路径要改为你的实际路径。
* 注意:下载ndk版本17.2.4988734，太新的ndk可能不支持

##### 开始编译
```
chmod a+x build-opencv3-static-android.sh
./build-opencv3-static-android.sh
```

##### 整合包
* 编译完成后，opencv目录下会有4个build开头的文件夹，对应了4种android的abi
* 把4个build-android-xxx下的install/sdk复制出来合并，合并后的sdk目录结构如下
```
sdk
└── native
    ├── 3rdparty
    │   └── libs
    │       ├── arm64-v8a
    │       │   ├── libcpufeatures.a
    │       │   └── libtegra_hal.a
    │       ├── armeabi-v7a
    │       │   ├── libcpufeatures.a
    │       │   └── libtegra_hal.a
    │       ├── x86
    │       │   └── libcpufeatures.a
    │       └── x86_64
    │           └── libcpufeatures.a
    ├── jni
    │   ├── OpenCV-arm64-v8a.mk
    │   ├── OpenCV-armeabi-v7a.mk
    │   ├── OpenCV-x86.mk
    │   ├── OpenCV-x86_64.mk
    │   ├── OpenCV.mk
    │   ├── OpenCVConfig-version.cmake
    │   ├── OpenCVConfig.cmake
    │   ├── abi-arm64-v8a
    │   │   ├── OpenCVConfig-version.cmake
    │   │   ├── OpenCVConfig.cmake
    │   │   ├── OpenCVModules-release.cmake
    │   │   └── OpenCVModules.cmake
    │   ├── abi-armeabi-v7a
    │   │   ├── OpenCVConfig-version.cmake
    │   │   ├── OpenCVConfig.cmake
    │   │   ├── OpenCVModules-release.cmake
    │   │   └── OpenCVModules.cmake
    │   ├── abi-x86
    │   │   ├── OpenCVConfig-version.cmake
    │   │   ├── OpenCVConfig.cmake
    │   │   ├── OpenCVModules-release.cmake
    │   │   └── OpenCVModules.cmake
    │   ├── abi-x86_64
    │   │   ├── OpenCVConfig-version.cmake
    │   │   ├── OpenCVConfig.cmake
    │   │   ├── OpenCVModules-release.cmake
    │   │   └── OpenCVModules.cmake
    │   ├── android.toolchain.cmake
    │   └── include
    │       ├── opencv
    │       │   ├── cv.h
    │       │   ├── cv.hpp
    │       │   ├── cvaux.h
    │       │   ├── cvaux.hpp
    │       │   ├── cvwimage.h
    │       │   ├── cxcore.h
    │       │   ├── cxcore.hpp
    │       │   ├── cxeigen.hpp
    │       │   ├── cxmisc.h
    │       │   ├── highgui.h
    │       │   └── ml.h
    │       └── opencv2
    │           ├── core
    │           │   ├── affine.hpp
    │           │   ├── async.hpp
    │           │   ├── base.hpp
    │           │   ├── bindings_utils.hpp
    │           │   ├── bufferpool.hpp
    │           │   ├── check.hpp
    │           │   ├── core.hpp
    │           │   ├── core_c.h
    │           │   ├── cuda
    │           │   │   ├── block.hpp
    │           │   │   ├── border_interpolate.hpp
    │           │   │   ├── color.hpp
    │           │   │   ├── common.hpp
    │           │   │   ├── datamov_utils.hpp
    │           │   │   ├── detail
    │           │   │   │   ├── color_detail.hpp
    │           │   │   │   ├── reduce.hpp
    │           │   │   │   ├── reduce_key_val.hpp
    │           │   │   │   ├── transform_detail.hpp
    │           │   │   │   ├── type_traits_detail.hpp
    │           │   │   │   └── vec_distance_detail.hpp
    │           │   │   ├── dynamic_smem.hpp
    │           │   │   ├── emulation.hpp
    │           │   │   ├── filters.hpp
    │           │   │   ├── funcattrib.hpp
    │           │   │   ├── functional.hpp
    │           │   │   ├── limits.hpp
    │           │   │   ├── reduce.hpp
    │           │   │   ├── saturate_cast.hpp
    │           │   │   ├── scan.hpp
    │           │   │   ├── simd_functions.hpp
    │           │   │   ├── transform.hpp
    │           │   │   ├── type_traits.hpp
    │           │   │   ├── utility.hpp
    │           │   │   ├── vec_distance.hpp
    │           │   │   ├── vec_math.hpp
    │           │   │   ├── vec_traits.hpp
    │           │   │   ├── warp.hpp
    │           │   │   ├── warp_reduce.hpp
    │           │   │   └── warp_shuffle.hpp
    │           │   ├── cuda.hpp
    │           │   ├── cuda.inl.hpp
    │           │   ├── cuda_stream_accessor.hpp
    │           │   ├── cuda_types.hpp
    │           │   ├── cv_cpu_dispatch.h
    │           │   ├── cv_cpu_helper.h
    │           │   ├── cvdef.h
    │           │   ├── cvstd.hpp
    │           │   ├── cvstd.inl.hpp
    │           │   ├── detail
    │           │   │   ├── async_promise.hpp
    │           │   │   └── exception_ptr.hpp
    │           │   ├── directx.hpp
    │           │   ├── eigen.hpp
    │           │   ├── fast_math.hpp
    │           │   ├── hal
    │           │   │   ├── hal.hpp
    │           │   │   ├── interface.h
    │           │   │   ├── intrin.hpp
    │           │   │   ├── intrin_avx.hpp
    │           │   │   ├── intrin_avx512.hpp
    │           │   │   ├── intrin_cpp.hpp
    │           │   │   ├── intrin_forward.hpp
    │           │   │   ├── intrin_msa.hpp
    │           │   │   ├── intrin_neon.hpp
    │           │   │   ├── intrin_sse.hpp
    │           │   │   ├── intrin_sse_em.hpp
    │           │   │   ├── intrin_vsx.hpp
    │           │   │   ├── intrin_wasm.hpp
    │           │   │   ├── msa_macros.h
    │           │   │   └── simd_utils.impl.hpp
    │           │   ├── ippasync.hpp
    │           │   ├── mat.hpp
    │           │   ├── mat.inl.hpp
    │           │   ├── matx.hpp
    │           │   ├── neon_utils.hpp
    │           │   ├── ocl.hpp
    │           │   ├── ocl_genbase.hpp
    │           │   ├── opencl
    │           │   │   ├── ocl_defs.hpp
    │           │   │   ├── opencl_info.hpp
    │           │   │   ├── opencl_svm.hpp
    │           │   │   └── runtime
    │           │   │       ├── autogenerated
    │           │   │       │   ├── opencl_clamdblas.hpp
    │           │   │       │   ├── opencl_clamdfft.hpp
    │           │   │       │   ├── opencl_core.hpp
    │           │   │       │   ├── opencl_core_wrappers.hpp
    │           │   │       │   ├── opencl_gl.hpp
    │           │   │       │   └── opencl_gl_wrappers.hpp
    │           │   │       ├── opencl_clamdblas.hpp
    │           │   │       ├── opencl_clamdfft.hpp
    │           │   │       ├── opencl_core.hpp
    │           │   │       ├── opencl_core_wrappers.hpp
    │           │   │       ├── opencl_gl.hpp
    │           │   │       ├── opencl_gl_wrappers.hpp
    │           │   │       ├── opencl_svm_20.hpp
    │           │   │       ├── opencl_svm_definitions.hpp
    │           │   │       └── opencl_svm_hsa_extension.hpp
    │           │   ├── opengl.hpp
    │           │   ├── operations.hpp
    │           │   ├── optim.hpp
    │           │   ├── ovx.hpp
    │           │   ├── persistence.hpp
    │           │   ├── ptr.inl.hpp
    │           │   ├── saturate.hpp
    │           │   ├── simd_intrinsics.hpp
    │           │   ├── softfloat.hpp
    │           │   ├── sse_utils.hpp
    │           │   ├── traits.hpp
    │           │   ├── types.hpp
    │           │   ├── types_c.h
    │           │   ├── utility.hpp
    │           │   ├── utils
    │           │   │   ├── allocator_stats.hpp
    │           │   │   ├── allocator_stats.impl.hpp
    │           │   │   ├── filesystem.hpp
    │           │   │   ├── instrumentation.hpp
    │           │   │   ├── logger.defines.hpp
    │           │   │   ├── logger.hpp
    │           │   │   ├── tls.hpp
    │           │   │   └── trace.hpp
    │           │   ├── va_intel.hpp
    │           │   ├── version.hpp
    │           │   ├── vsx_utils.hpp
    │           │   └── wimage.hpp
    │           ├── core.hpp
    │           ├── cvconfig.h
    │           ├── highgui
    │           │   ├── highgui.hpp
    │           │   └── highgui_c.h
    │           ├── highgui.hpp
    │           ├── imgcodecs
    │           │   ├── imgcodecs.hpp
    │           │   ├── imgcodecs_c.h
    │           │   └── ios.h
    │           ├── imgcodecs.hpp
    │           ├── imgproc
    │           │   ├── detail
    │           │   │   └── distortion_model.hpp
    │           │   ├── hal
    │           │   │   ├── hal.hpp
    │           │   │   └── interface.h
    │           │   ├── imgproc.hpp
    │           │   ├── imgproc_c.h
    │           │   └── types_c.h
    │           ├── imgproc.hpp
    │           ├── opencv.hpp
    │           ├── opencv_modules.hpp
    │           └── world.hpp
    └── staticlibs
        ├── arm64-v8a
        │   └── libopencv_world.a
        ├── armeabi-v7a
        │   └── libopencv_world.a
        ├── x86
        │   └── libopencv_world.a
        └── x86_64
            └── libopencv_world.a
```

##### 修复静态库造成的编译错误
1. 因为许多模块去除了，要把头文件的引用也注释，否则会造成编译错误
```
修改sdk/native/jni/include/opencv/cv.h，注释如下行
//#include "opencv2/photo/photo_c.h"
//#include "opencv2/video/tracking_c.h"
//#include "opencv2/objdetect/objdetect_c.h"

修改sdk/native/jni/include/opencv/cv.hpp，注释如下行
//#include "opencv2/photo.hpp"
//#include "opencv2/video.hpp"
//#include "opencv2/features2d.hpp"
//#include "opencv2/calib3d.hpp"
//#include "opencv2/objdetect.hpp"
```
2. 修改4个cmake文件
```
4个文件路径分别为
sdk/native/jni/abi-armeabi-v7a/OpenCVConfig.cmake
sdk/native/jni/abi-arm64-v8a/OpenCVConfig.cmake
sdk/native/jni/abi-x86/OpenCVConfig.cmake
sdk/native/jni/abi-x86_64/OpenCVConfig.cmake

把此行
set(OpenCV_LIB_COMPONENTS opencv_core;opencv_highgui;opencv_imgcodecs;opencv_imgproc;opencv_world)

修改成这样
set(OpenCV_LIB_COMPONENTS opencv_world)
```

##### 使用静态库编译apk
1. 把sdk文件夹复制到OcrLibrary/src/sdk
2. 删除OcrLibrary/src/main/sharedLibs(如果有的话)
3. 删除OcrLibrary/src/main/cpp/include/opencv和OcrLibrary/src/main/cpp/include/opencv2
4. 修改OcrLibrary/src/main/cpp/CMakeLists.txt
```
把原来的
add_library(libopencv_java3 SHARED IMPORTED)
set_target_properties(libopencv_java3 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../sharedLibs/${ANDROID_ABI}/libopencv_java3.so)

改成如下
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/../../sdk/native/jni")
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
    message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "opencv Not Found!")
endif (OpenCV_FOUND)

把原来的
target_link_libraries(
        OcrLite
        ncnn
        libopencv_java3
        android
        z
        ${log-lib}
        ${android-lib}
        jnigraphics)

改成如下
target_link_libraries(
        OcrLite
        ncnn
        ${OpenCV_LIBS}
        android
        z
        ${log-lib}
        ${android-lib}
        jnigraphics)
```

5. 删除缓存，重新编译
删除项目根目录下的如下文件夹
```
.idea
build
app/build
OcrLibrary/.cxx
OcrLibrary/build
```
6. 使用命令编译```./gradlew assembleRelease```

