# ChOcrLiteAndroidOnnx

#### Demo APK下载
[Gitee下载](https://gitee.com/benjaminwan/ch-ocr-lite-android-onnx/releases)

#### 介绍
Chineseocr Lite Android Onnx Demo，超轻量级中文OCR Android Demo，支持ncnn推理 (DBNet+AngleNet+CRNN)

采用ncnn神经网络前向计算框架[https://github.com/Tencent/ncnn](https://github.com/Tencent/ncnn)

#### 说明
1. 封装为独立的Library。
2. Native层以C++编写。
3. demo app以Kotlin-JVM编写。

#### 编译说明
1.  AndroidStudio 4.0.2或以上
2.  NDK
3.  cmake 3.4.1或以上
4.  请自行下载ncnn预编译库ncnn-android-lib.zip 20200916 b766c8c,[下载地址](https://github.com/Tencent/ncnn/releases)，放到目录OcrLibrary/src/main/staticLibs
5. 请自行下载opencv动态库opencv-3.4.10-android-sdk.zip，[下载地址](https://github.com/opencv/opencv/releases/tag/3.4.11)，放到目录OcrLibrary/src/main/sharedLibs

6. 依赖库目录结构  
```
staticLibs  
├── arm64-v8a  
│   └── libncnn.a  
├── armeabi-v7a  
│   └── libncnn.a  
├── x86  
│   └── libncnn.a  
└── x86_64  
    └── libncnn.a  
sharedLibs   
├── arm64-v8a   
│   └── libopencv_java3.so   
├── armeabi-v7a   
│   └── libopencv_java3.so   
├── x86   
│   └── libopencv_java3.so   
└── x86_64   
    └── libopencv_java3.so   
```

#### master分支PseNet版本
[ChOcrLiteAndroidPseNet](https://github.com/ouyanghuiyu/chineseocr_lite/tree/master/ncnn_android_projects/ChOcrLiteAndroidPseNet)

####  master分支DBNet版本
[ChOcrLiteAndroidDBNet](https://github.com/ouyanghuiyu/chineseocr_lite/tree/master/ncnn_android_projects/ChOcrLiteAndroidDBNet)

#### Android各版本区别(仅供参考)
1. 模型总大小对比：未压缩的情况下，PseNet版(27.3M) > DBNet版(23M) > onnx版(4.72M)。
2. PseNet与DBNet版本仅图像分割的算法不同，文字方向检测与文字识别部分完全相同。分割速度方面，DBNet版本稍微快一点点，分割效果方面，PseNet效果稍好一点点。
3. onnx版本与其它两个版本最大不同在于模型超轻量，当然总体的效果就不如其它两个版本，没有专门针对竖向文字的模型，所以对竖向文字的识别效果比其它两个版本差(比如春联)。速度方面与DBNet版本差不多。
