# ChOcrLiteAndroidMNN


## 介绍
Chineseocr Lite Android MNN Demo，超轻量级中文OCR Android Demo，支持mnn推理 (DBNet+AngleNet+CRNN)

**代码和模型均源自chineseocr lite的onnx分支**

详情请查看 [https://github.com/ouyanghuiyu/chineseocr_lite](https://github.com/ouyanghuiyu/chineseocr_lite)

MNN 框架 [https://github.com/alibaba/MNN](https://github.com/alibaba/MNN)


## 总体说明

1.依赖MNN版本为1.1.7
2.MNN依赖方式为动态库，位于目录 `OcrLiteAndroidNcnn/OcrLibrary/src/mnn`,库文件较小直接在git中管理
3.依赖opencv 3.4.10 
4.模型已经转换为mnn格式，转换工具版本也是1.1.7版本，位于目录：

```
OcrLiteAndroidNcnn/OcrLibrary/src/main
    └── assets
        ├── angle_net.mnn
        ├── crnn_lite_lstm.mnn
        └── dbnet.mnn
```

## 编译说明
1. AndroidStudio 2020.3.1或以上
2. NDK下载，在SDK Tools中下载，版本选最新版；
3. cmake 3.4.1或以上，最好是3.18.1，在SDK Tools中下载；
4. 下载opencv-3.4.10-android-sdk-static-lite.7z，[下载地址](https://gitee.com/benjaminwan/ocr-lite-android-onnx/attach_files/574782/download/opencv-3.4.10-android-sdk-static-lite.7z)

解压后目录结构为
```
OcrLiteAndroidNcnn/OcrLibrary/src/sdk
    └── native
        ├── 3rdparty
        ├── jni
        └── staticlibs
```

## 常见问题

### 输入参数说明

请参考[OcrLiteMnn项目](https://github.com/DayBreak-u/chineseocr_lite/tree/onnx/cpp_projects/OcrLiteMnn)


## 关于作者

 Android MNN demo 集成 [thomaszheng](https://github.com/thomaszheng)
 
## 版权声明

- OCR模型版权归[DayBreak](https://github.com/DayBreak-u)所有；
- 其它工程代码版权归本仓库所有者所有；