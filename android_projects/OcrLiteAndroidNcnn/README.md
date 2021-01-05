# ChOcrLiteAndroidOnnxToNcnn

### Demo APK下载
[Gitee下载](https://gitee.com/benjaminwan/ocr-lite-android-ncnn/releases)

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
1. AndroidStudio 4.1或以上
2. NDK
3. cmake 3.4.1或以上
4. [模型下载](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/models_ncnn)
解压后目录结构为
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

6. ncnn预编译库版本为 20201218 5650b77，[下载地址](https://github.com/Tencent/ncnn/releases/tag/20201218)
可以选择ncnn-android.zip或者ncnn-android-vulkan.zip，解压后目录结构为
```
OcrLiteAndroidNcnn/OcrLibrary/src/main/ncnn-static
    ├── arm64-v8a
    ├── armeabi-v7a
    ├── x86
    └── x86_64
```
* **注意：解压后还必须修改每个abi目录下的lib/cmake/ncnn/ncnn.cmake，注释掉此行```#  INTERFACE_COMPILE_OPTIONS "-fno-rtti;-fno-exceptions"```
一共有4个文件需要修改，否则会造成编译错误。**

7. 当选择ncnn不带vulkan支持的版本时
* app/build.gradle和OcrLibrary/build.gradle里的minSdkVersion可以改为21，编译出来的apk体积小10MB
* minSdkVersion=21时，最终编译出来的apk大约21MB

8. 当选择ncnn带vulkan支持的版本时
* app/build.gradle和OcrLibrary/build.gradle里的minSdkVersion必须>=24
* 因为sdk24(Android N/7.0)启用了新的打包和签名方式，再加上vulkan支持增加了不少体积，最终apk大约58MB
* 可以通过修改OcrLibrary/src/main/cpp/CMakeLists.txt的```set(OCR_LITE_VULKAN OFF)```来关闭gpu计算

### 输入参数说明
请参考[OcrLiteOnnx项目](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/pc_projects/OcrLiteOnnx)

### 删除缓存，重新编译
删除项目根目录下的如下文件夹
```
.idea
build
app/build
OcrLibrary/.cxx
OcrLibrary/build
```
### 编译Release包
使用命令编译```./gradlew assembleRelease```

