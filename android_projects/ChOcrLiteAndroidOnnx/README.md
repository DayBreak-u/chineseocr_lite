# ChOcrLiteAndroidOnnx

#### Demo APK下载
[Gitee下载](https://gitee.com/benjaminwan/ch-ocr-lite-android-onnx/releases)

#### 介绍
Chineseocr Lite Android Onnx Demo，超轻量级中文OCR Android Demo，支持onnx推理 (DBNet+AngleNet+CRNN)

**代码和模型均源自chineseocr lite的onnx分支**

详情请查看 [https://github.com/ouyanghuiyu/chineseocr_lite](https://github.com/ouyanghuiyu/chineseocr_lite)

onnxruntime框架[https://github.com/microsoft/onnxruntime](https://github.com/microsoft/onnxruntime)

整合了如下算法：
dbnet(图像分割)+anglenet(文字方向检测)+crnn(文字识别)
#### 说明
1. 封装为独立的Library。
2. Native层以C++编写。
3. demo app以Kotlin-JVM编写。
4. opencv动态库版本opencv-3.4.10-android-sdk.zip
5. onnxruntime动态库版本，自己编译的pre 1.7.0版

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
5.  [模型下载地址](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/models)
```
ChOcrLiteAndroidOnnx/OcrLibrary/src/main/assets
├── angle_net.onnx
├── crnn_lite_lstm.onnx
├── dbnet.onnx
└── keys.txt
```
6.  onnxruntime，可以自行同步源码编译，也可以从顶部下载地址找到onnxruntime-android-1.7.0-pre.zip
```
ChOcrLiteAndroidOnnx/OcrLibrary/src/main/onnx
├── ONNXConfig.cmake
├── arm64-v8a
│   └── libonnxruntime.so
├── armeabi-v7a
│   └── libonnxruntime.so
├── include
│   └── onnx
│       ├── automl_data_containers.h
│       ├── environment.h
│       ├── experimental_onnxruntime_cxx_api.h
│       ├── experimental_onnxruntime_cxx_inline.h
│       ├── onnxruntime_c_api.h
│       ├── onnxruntime_cxx_api.h
│       ├── onnxruntime_cxx_inline.h
│       └── onnxruntime_session_options_config_keys.h
├── x86
│   └── libonnxruntime.so
└── x86_64
    └── libonnxruntime.so
```

#### onnxToncnn版本
[https://gitee.com/benjaminwan/ch-ocr-lite-android-onnx-to-ncnn](https://gitee.com/benjaminwan/ch-ocr-lite-android-onnx-to-ncnn)

