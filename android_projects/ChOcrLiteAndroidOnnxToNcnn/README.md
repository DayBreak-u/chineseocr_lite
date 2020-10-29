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
1.  AndroidStudio 4.0.2或以上
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

