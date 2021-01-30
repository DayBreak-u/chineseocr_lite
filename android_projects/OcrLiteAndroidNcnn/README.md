# OcrLiteAndroidNcnn

### Project下载
* 有整合好源码和依赖库的完整工程项目，文件比较大，可到Q群共享内下载，找以Project开头的压缩包文件
* 如果想自己折腾，则请继续阅读本说明

### Demo APK下载
编译好的demo文件可以到Q群共享内下载
或者[Gitee下载](https://gitee.com/benjaminwan/ocr-lite-android-ncnn/releases)

### 介绍
Chineseocr Lite Android Ncnn Demo，超轻量级中文OCR Android Demo，支持ncnn推理 (DBNet+AngleNet+CRNN)
把onnx模型格式转为ncnn格式，并使用ncnn推理框架进行OCR识别。
**代码和模型均源自chineseocr lite的onnx分支**

详情请查看 [https://github.com/ouyanghuiyu/chineseocr_lite](https://github.com/ouyanghuiyu/chineseocr_lite)

采用ncnn神经网络前向计算框架[https://github.com/Tencent/ncnn](https://github.com/Tencent/ncnn)

### 说明
1. 封装为独立的Library。
2. Native层以C++编写。
3. demo app以Kotlin-JVM编写。
4. Android版与其它版本不同，包含了几个应用场景，包括相册识别、摄像头识别、手机IMEI号识别、摄像头身份证识别这几个功能页面。
5. 自己编译的opencv 3.4.10，并精简了不需要的模块，减小apk体积。
6. 可自己选择是否要支持vulkan(GPU加速)。

### 编译说明
1. AndroidStudio 4.1.2或以上
2. NDK
3. cmake 3.4.1或以上
4. [模型下载](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/models_ncnn)
目录结构为
```
  OcrLiteAndroidNcnn/OcrLibrary/src/main/assets
  ├── angle_op.bin
  ├── angle_op.param
  ├── crnn_lite_op.bin
  ├── crnn_lite_op.param
  ├── dbnet_op.bin
  ├── dbnet_op.param
  └── keys.txt
  ```
5. 下载opencv-3.4.10-android-sdk-static-lite.7z，[下载地址](https://gitee.com/benjaminwan/ocr-lite-android-ncnn/releases/1.0.0.20201007)
解压后目录结构为
```
OcrLiteAndroidNcnn/OcrLibrary/src/sdk
    └── native
        ├── 3rdparty
        ├── jni
        └── staticlibs
```

6. ncnn预编译库版本为 20210124 5e4ea0b，[下载地址](https://github.com/Tencent/ncnn/releases/tag/20210124)
* 目标是编译cpu版和gpu版，所以需要下载 "不带vulkan的"和"带vulkan的"两种库。
* 但是每种库又分为静态库和动态库，所以一共就有四种库。
* 下载ncnn-20210124-android.zip或ncnn-20210124-android-shared.zip，解压到OcrLiteAndroidNcnn/OcrLibrary/src/main/ncnn
* 下载ncnn-20210124-android-vulkan.zip或ncnn-20210124-android-vulkan-shared.zip，解压到OcrLiteAndroidNcnn/OcrLibrary/src/main/ncnn-vulkan
* 解压后目录结构为
```
OcrLiteAndroidNcnn/OcrLibrary/src/main/ncnn
    ├── arm64-v8a
    ├── armeabi-v7a
    ├── x86
    └── x86_64
OcrLiteAndroidNcnn/OcrLibrary/src/main/ncnn-vulkan
    ├── arm64-v8a
    ├── armeabi-v7a
    ├── x86
    └── x86_64
```
* **注意：解压后还必须修改每个abi目录下的lib/cmake/ncnn/ncnn.cmake，注释掉此行```#  INTERFACE_COMPILE_OPTIONS "-fno-rtti;-fno-exceptions"```
一共有4个文件需要修改，否则会造成编译错误。**

7. cpu版:即ncnn不带vulkan支持的版本
* app/build.gradle和OcrLibrary/build.gradle里的minSdkVersion通过productFlavors设置为21
* minSdkVersion=21时，最终编译出来的apk大约21MB

8. gpu版:即ncnn带vulkan支持的版本
* app/build.gradle和OcrLibrary/build.gradle里的minSdkVersion通过productFlavors设置为24
* 因为sdk24(Android N/7.0)启用了新的打包和签名方式，再加上vulkan支持增加了不少体积，最终apk大约58MB

### 编译Release包
* 使用命令编译```./gradlew assembleRelease```
* 输出文件在app/build/outputs/apk

### Android Studio 调试启动
* 先在左侧边栏中找到"Build Variants"选项卡
* 在选项卡里，有app和OcrLibrary两项，且"Active Build Variant"可以选择CpuDebug/CpuRelease/GpuDebug/GpuRelease
* 选中需要的"Active Build Variant"，注意app和OcrLibrary必须选择相同选项，等待刷新，然后直接用工具栏的运行或调试按钮启动。

### 输入参数说明
请参考[OcrLiteOnnx项目](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/cpp_projects/OcrLiteOnnx)

### 删除缓存，重新编译
删除项目根目录下的如下文件夹
```
.idea
build
app/build
OcrLibrary/.cxx
OcrLibrary/build
```

