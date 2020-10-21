# OcrLiteOnnxToNcnn

#### Demo下载(win、mac、linux)
[Gitee下载](https://gitee.com/benjaminwan/ocr-lite-onnx-to-ncnn/releases)

#### 介绍
ChineseOcr Lite Onnx To Ncnn，超轻量级中文OCR PC Demo，支持ncnn推理(DBNet+AngleNet+CRNN)

**对应chineseocr lite的onnx分支**

这个项目使用ncnn框架进行推理，模型是从onnx模型转成了ncnn模型格式。

详情请查看 [https://github.com/ouyanghuiyu/chineseocr_lite](https://github.com/ouyanghuiyu/chineseocr_lite)

采用ncnn神经网络前向计算框架[https://github.com/Tencent/ncnn](https://github.com/Tencent/ncnn)

整合了如下算法：
DBNet(图像分割)+AngleNet(文字方向检测)+CRNN(文字识别)

[模型下载地址](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/ncnn_project/models)
```
OcrLiteOnnxToNcnn/models
├── angle_op.bin
├── angle_op.param
├── crnn_lite_op.bin
├── crnn_lite_op.param
├── dbnet_op.bin
├── dbnet_op.param
└── keys.txt
```

#### 编译环境
1. cmake 3.18.4
2. ncnn预编译库版本为 20200916 b766c8c，文件名:ncnn-pc-lib-20200916.zip，从顶部下载地址下载后，解压到项目根目录
```
OcrLiteOnnxToNcnn/ncnn
├── include
│   └── ncnn
│       ├── allocator.h
│       ├── benchmark.h
│       ├── blob.h
│       ├── c_api.h
│       ├── command.h
│       ├── cpu.h
│       ├── datareader.h
│       ├── gpu.h
│       ├── layer.h
│       ├── layer_shader_type.h
│       ├── layer_shader_type_enum.h
│       ├── layer_type.h
│       ├── layer_type_enum.h
│       ├── mat.h
│       ├── modelbin.h
│       ├── net.h
│       ├── opencv.h
│       ├── option.h
│       ├── paramdict.h
│       ├── pipeline.h
│       ├── pipelinecache.h
│       ├── platform.h
│       └── simplestl.h
├── linux
│   ├── cmake
│   │   └── ncnn
│   │       ├── ncnn-release.cmake
│   │       ├── ncnn.cmake
│   │       └── ncnnConfig.cmake
│   └── libncnn.a
├── macos
│   ├── cmake
│   │   └── ncnn
│   │       ├── ncnn-release.cmake
│   │       ├── ncnn.cmake
│   │       └── ncnnConfig.cmake
│   └── libncnn.a
└── windows
    ├── cmake
    │   └── ncnn
    │       ├── ncnn-release.cmake
    │       ├── ncnn.cmake
    │       └── ncnnConfig.cmake
    └── ncnn.lib
```
**注意：如果你要使用此处下载的预编译库，请选择VS2019来编译。如果你打算自己编译，则可以选择低版本VS**

3. opencv动态库版本3.4.x以上

##### Windows编译说明
1.  Windows10 x64 /VS2019或VS2017
2.  cmake请自行下载&配置
3.  下载opencv-3.4.11-vc14_vc15.exe，[下载地址](https://github.com/opencv/opencv/releases/tag/3.4.11)，把文件解压到项目根目录。解压后目录结构为
```
OcrLiteOnnx/opencv
│  LICENSE.txt
│  LICENSE_FFMPEG.txt
│  README.md.txt
├─build              
└─sources
```
4.  VS2017/2019安装时，至少选中"使用C++的桌面开发"
5.  开始菜单打开"x64 Native Tools Command Prompt for VS 2019"或"适用于 VS2017 的 x64 本机工具"，并转到OcrLiteOnnx根目录
6.  运行```build-win.cmd```
7.  编译完成后运行```run-test-win.cmd```进行测试
8.  如果提示缺少"VCRUNTIME140_1.dll"，下载安装适用于 Visual Studio 2015、2017 和 2019 的 Microsoft Visual C++ 可再发行软件包，
[下载地址](https://support.microsoft.com/zh-cn/help/2977003/the-latest-supported-visual-c-downloads)

##### Mac编译说明
1.  macOS Catalina 10.15.x
2.  自行下载安装HomeBrew
3.  下载opencv：
```
brew install opencv@3
brew ln opencv3 --force
```
4.  libomp: ```brew install libomp```
5.  编译：```./build.sh```
6.  测试：```./run-test.sh```

##### Linux编译说明
1.  Deepin 20 或其它发行版
2.  安装build-essential:略……
3.  下载opencv：各发行版不大一样，略……
4.  编译：```./build.sh```
5.  测试：```./run-test.sh```

#### 测试结果说明
1.  *-part-x.jpg为分割后的图片
2.  *-debug-x.jpg为分割后的图片进行方向识别后，校准方向后的图片
3.  *-result.jpg为图像分割画框的结果
4.  *-result.txt为识别的最终结果
5.  最终结果包含：
getTextBoxesTime(图像分割耗时)、
TextBoxScore(文字框置信度)、
TextBoxPos(文字框坐标)、
Angle(方向索引、方向置信度)、
getAngleTime(方向识别耗时)、
textLine(识别到的文字)
textScores(各个文字的置信度)、
getTextLineTime(文字识别耗时)、
TextBoxTime(方向识别+文字识别耗时)
FullDetectTime（整张图片总耗时）