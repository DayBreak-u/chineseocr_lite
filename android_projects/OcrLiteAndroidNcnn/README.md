# OcrLiteAndroidNcnn

[![Issue](https://img.shields.io/github/issues/benjaminwan/OcrLiteAndroidNcnn.svg)](https://github.com/benjaminwan/OcrLiteAndroidNcnn/issues)
[![Star](https://img.shields.io/github/stars/benjaminwan/OcrLiteAndroidNcnn.svg)](https://github.com/benjaminwan/OcrLiteAndroidNcnn)

<details open>
    <summary>目录</summary>

- [OcrLiteAndroidNcnn](#OcrLiteAndroidNcnn)
    - [联系方式](#联系方式)
    - [项目完整源码](#项目完整源码)
    - [APK下载](#APK下载)
    - [简介](#简介)
    - [总体说明](#总体说明)
    - [更新说明](#更新说明)
    - [编译说明](#编译说明)
        - [编译Release包](#编译Release包)
        - [AndroidStudio调试启动](#AndroidStudio调试启动)
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

* 整合好源码和依赖库的完整工程项目，文件比较大，可到Q群共享内下载，以Project开头的压缩包文件为源码工程，例：Project_OcrLiteAndroidNcnn-版本号.7z
* 如果想自己折腾，则请继续阅读本说明

## APK下载

* 编译好的demo apk，可以在release中下载，或者Q群共享内下载，文件名例：OcrLiteAndroidNcnn-版本号-cpu-release.apk
* 或者[Github下载](https://github.com/benjaminwan/OcrLiteAndroidNcnn/releases)

## 简介

Chineseocr Lite Android Ncnn Demo，超轻量级中文OCR Android Demo，支持ncnn推理 (DBNet+AngleNet+CRNN)
把onnx模型格式转为ncnn格式，并使用ncnn推理框架进行OCR识别。
**代码和模型均源自chineseocr lite的onnx分支**

详情请查看 [https://github.com/ouyanghuiyu/chineseocr_lite](https://github.com/ouyanghuiyu/chineseocr_lite)

采用ncnn神经网络前向计算框架[https://github.com/Tencent/ncnn](https://github.com/Tencent/ncnn)

## 总体说明

1. 封装为独立的Library，可以编译为aar，作为模块来调用；
2. Native层以C++编写；
3. Demo App以Kotlin-JVM编写；
4. Android版与其它版本不同，包含了几个应用场景，包括相册识别、摄像头识别、手机IMEI号识别、摄像头身份证识别这几个功能页面；
5. opencv 3.4.15，使用了[opencv-mobile](https://github.com/nihui/opencv-mobile)的代码，编译脚本在script文件夹里；
6. ncnn 20210720；
7. 可选择CPU版或GPU版；CPU版仅支持CPU运算，最低支持API21，且安装包体积小；GPU版支持vulkan(GPU加速)，最低支持API24，安装包体积较大；

## 更新说明

#### 2021-09-28 update 1.6.0

* opencv 3.4.15(特别说明：4.5.3也可以支持，如果换成cv4，minSdkVersion必须改为24)
* ncnn 20210720
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

5.
下载opencv-mobile-3.4.15-android.7z，[下载地址](https://gitee.com/benjaminwan/ocr-lite-android-ncnn/attach_files/843219/download/opencv-mobile-3.4.15-android.7z)
解压后目录结构为

```
OcrLiteAndroidNcnn/OcrLibrary/src/sdk
    └── native
        ├── jni
        └── staticlibs
```

6. ncnn预编译库版本为 20210720 2c4ae09，[下载地址](https://github.com/Tencent/ncnn/releases/tag/20210720)

* 目标是编译cpu版和gpu版，所以需要下载 "不带vulkan的"和"带vulkan的"两种库。
* 但是每种库又分为静态库和动态库，所以一共就有四种库。
* 下载ncnn-20210124-android.zip或ncnn-20210124-android-shared.zip，二选一，解压到OcrLiteAndroidNcnn/OcrLibrary/src/main/ncnn
*

下载ncnn-20210124-android-vulkan.zip或ncnn-20210124-android-vulkan-shared.zip，二选一，解压到OcrLiteAndroidNcnn/OcrLibrary/src/main/ncnn-vulkan

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

* **注意：解压后还必须修改每个abi目录下的lib/cmake/ncnn/ncnn.cmake，注释掉此行```# INTERFACE_COMPILE_OPTIONS "-fno-rtti;-fno-exceptions"```
  一共有4个文件需要修改，否则会造成编译错误。**

### 编译Release包

* mac/linux使用命令编译```./gradlew assembleRelease```
* win使用命令编译```gradlew.bat assembleRelease```
* 输出apk文件在app/build/outputs/apk

### AndroidStudio调试启动

* 先在左侧边栏中找到"Build Variants"选项卡
* 在选项卡里，有app和OcrLibrary两项，且"Active Build Variant"可以选择CpuDebug/CpuRelease/GpuDebug/GpuRelease
* 选中需要的"Active Build Variant"，注意app和OcrLibrary必须选择相同选项，等待刷新，然后直接用工具栏的运行或调试按钮启动。
* 选中cpu版时: minSdkVersion=21，最终编译出来的apk大约21MB
* 选中gpu版时:minSdkVersion=24，因为sdk24(Android N/7.0)启用了新的打包和签名方式，再加上vulkan支持增加了不少体积，最终apk大约58MB

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
OcrLiteAndroidNcnn
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

