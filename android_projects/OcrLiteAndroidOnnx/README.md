# OcrLiteAndroidOnnx

[![Issue](https://img.shields.io/github/issues/benjaminwan/OcrLiteAndroidOnnx.svg)](https://github.com/benjaminwan/OcrLiteAndroidOnnx/issues)
[![Star](https://img.shields.io/github/stars/benjaminwan/OcrLiteAndroidOnnx.svg)](https://github.com/benjaminwan/OcrLiteAndroidOnnx)

<details open>
    <summary>目录</summary>

- [OcrLiteAndroidOnnx](#OcrLiteAndroidOnnx)
    - [联系方式](#联系方式)
    - [项目完整源码](#项目完整源码)
    - [APK下载](#APK下载)
    - [简介](#简介)
    - [总体说明](#总体说明)
    - [更新说明](#更新说明)
    - [编译说明](#编译说明)
        - [编译Release包](#编译Release包)
        - [重新编译](#重新编译)
    - [项目结构](#项目结构)
    - [常见问题](#常见问题)
        - [输入参数说明](#输入参数说明)
    - [关于作者](#关于作者)
    - [版权声明](#版权声明)
    - [示例图](#示例图)
        - [IMEI识别](#IMEI识别)
        - [身份证识别](#身份证识别)
        - [车牌识别](#车牌识别)

</details>

## 联系方式

* QQ①群：994929053
* QQ②群：820132154
* QQ③群：904091319

## 项目完整源码

* 整合好源码和依赖库的完整工程项目，文件比较大，可到Q群共享内下载，以Project开头的压缩包文件为源码工程，例：Project_OcrLiteAndroidOnnx-版本号.7z
* 如果想自己折腾，则请继续阅读本说明

## APK下载

* 编译好的demo apk，可以在release中下载，或者Q群共享内下载，文件名例：OcrLiteAndroidOnnx-版本号-release.apk
* 或者[Github下载](https://github.com/benjaminwan/OcrLiteAndroidOnnx/releases)

## 简介

Chineseocr Lite Android Onnx Demo，超轻量级中文OCR Android Demo，支持onnx推理 (DBNet+AngleNet+CRNN)

**代码和模型均源自chineseocr lite的onnx分支**

详情请查看 [https://github.com/ouyanghuiyu/chineseocr_lite](https://github.com/ouyanghuiyu/chineseocr_lite)

onnxruntime框架[https://github.com/microsoft/onnxruntime](https://github.com/microsoft/onnxruntime)

## 总体说明

1. 封装为独立的Library，可以编译为aar，作为模块来调用；
2. Native层以C++编写；
3. Demo App以Kotlin-JVM编写；
4. Android版与其它版本不同，包含了几个应用场景，包括相册识别、摄像头识别、手机IMEI号识别、摄像头身份证识别这几个功能页面；
5. opencv 3.4.15，使用了[opencv-mobile](https://github.com/nihui/opencv-mobile)的代码，编译脚本在script文件夹里；
6. onnxruntime动态库 1.9.0

## 更新说明

#### 2021-09-29 update 1.6.0

* opencv 3.4.15(特别说明：4.5.3也可以支持，如果换成cv4，minSdkVersion必须改为24)
* onnxruntime 1.9.0
* 升级kotlin: 1.5.31
* 升级gradle: 7.0.2
* 升级各dependencies
* 使用kotlin coroutines代替rxjava

#### 2021-10-28 update 1.6.1

* 编辑build.gradle，把版本定义放到root build.gradle
* 编辑github workflow config，上传aar文件到release

## 编译说明

1. AndroidStudio 2020.3.1或以上；
2. NDK下载，在SDK Tools中下载，版本选最新版；
3. cmake 3.4.1或以上，最好是3.18.1，在SDK Tools中下载；
4. [模型下载](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/models)
   目录结构为

```
OcrLiteAndroidOnnx/OcrLibrary/src/main/assets
├── angle_net.onnx
├── crnn_lite_lstm.onnx
├── dbnet.onnx
└── keys.txt
```
5. 下载opencv-mobile-3.4.15-android.7z，[下载地址](https://gitee.com/benjaminwan/ocr-lite-android-ncnn/attach_files/843219/download/opencv-mobile-3.4.15-android.7z)
解压后目录结构为

```
OcrLiteAndroidNcnn/OcrLibrary/src/sdk
    └── native
        ├── jni
        └── staticlibs
```

6. 下载onnxruntime-1.9.0-android.7z，[下载地址](https://gitee.com/benjaminwan/ocr-lite-android-onnx/releases/v1.0.0.20201022)

* 解压后目录结构为
```
OcrLiteAndroidOnnx/OcrLibrary/src/main/onnx
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

7. 编译好的安装大约3xMB


### 编译Release包

* mac/linux使用命令编译```./gradlew assembleRelease```
* win使用命令编译```gradlew.bat assembleRelease```
* 输出apk文件在app/build/outputs/apk

### 重新编译

删除项目根目录下的如下缓存文件夹

```
.idea
build
app/build
OcrLibrary/.cxx
OcrLibrary/build
```

## 项目结构

```
OcrLiteAndroidOnnx
    ├── app               # demo app
    ├── capture           # 截图
    ├── common-aar        # app引用的aar库
    ├── keystore          # app签名密钥文件
    ├── OcrLibrary        # Ocr引擎库，包含Jni和C++代码
    └── scripts           # 编译脚本
```

## 常见问题

### 输入参数说明

请参考[OcrLiteOnnx项目](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/cpp_projects/OcrLiteOnnx)

## 关于作者

* Android demo编写：[benjaminwan](https://github.com/benjaminwan)
* 模型开发：[DayBreak](https://github.com/DayBreak-u)

## 版权声明

- OCR模型版权归[DayBreak](https://github.com/DayBreak-u)所有；
- 其它工程代码版权归本仓库所有者所有；

## 示例图

#### IMEI识别

![avatar](capture/detect_IMEI.gif)

#### 身份证识别

![avatar](capture/detect_id_card.gif)

#### 车牌识别

![avatar](capture/detect_plate.gif)

