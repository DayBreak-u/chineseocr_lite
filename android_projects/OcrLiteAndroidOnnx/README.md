# ChOcrLiteAndroidOnnx

### Demo APK下载
[Gitee下载](https://gitee.com/benjaminwan/ocr-lite-android-onnx/releases)

### 介绍
Chineseocr Lite Android Onnx Demo，超轻量级中文OCR Android Demo，支持onnx推理 (DBNet+AngleNet+CRNN)

**代码和模型均源自chineseocr lite的onnx分支**

详情请查看 [https://github.com/ouyanghuiyu/chineseocr_lite](https://github.com/ouyanghuiyu/chineseocr_lite)

onnxruntime框架[https://github.com/microsoft/onnxruntime](https://github.com/microsoft/onnxruntime)

### 说明
1. 封装为独立的Library。
2. Native层以C++编写。
3. demo app以Kotlin-JVM编写。
4. Android版与其它版本不同，包含了几个应用场景，包括相册识别、摄像头识别、手机IMEI号识别、摄像头身份证识别这几个功能页面。
5. 自己编译的opencv 3.4.10，并精简了不需要的模块，减小apk体积。
6. onnxruntime动态库为自己编译的1.6.0版

### 编译说明
1.  AndroidStudio 4.1.2或以上
2.  NDK
3.  cmake 3.4.1或以上
4.  [模型下载](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/models)
解压后目录结构为
```
ChOcrLiteAndroidOnnx/OcrLibrary/src/main/assets
├── angle_net.onnx
├── crnn_lite_lstm.onnx
├── dbnet.onnx
└── keys.txt
```
5.  下载opencv-3.4.10-android-sdk-static-lite.7z，[下载地址](https://gitee.com/benjaminwan/ocr-lite-android-onnx/releases/v1.0.0.20201022)
解压后目录结构为
```
OcrLiteAndroidNcnn/OcrLibrary/src/sdk
    └── native
        ├── 3rdparty
        ├── jni
        └── staticlibs
```

6. 下载onnxruntime-1.6.0-android.7z，[下载地址](https://gitee.com/benjaminwan/ocr-lite-android-onnx/releases/v1.0.0.20201022)
```
ChOcrLiteAndroidOnnx/OcrLibrary/src/main/onnx
├── ONNXConfig.cmake
├── arm64-v8a
│   └── libonnxruntime.so
├── armeabi-v7a
│   └── libonnxruntime.so
├── include
├── x86
│   └── libonnxruntime.so
└── x86_64
    └── libonnxruntime.so
```

7. 编译好的安装大约34MB

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
### 编译Release包
使用命令编译```./gradlew assembleRelease```
