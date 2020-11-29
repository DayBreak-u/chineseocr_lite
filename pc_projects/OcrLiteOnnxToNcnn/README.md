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
│   └── ncnn
│       ├── allocator.h
│       ├── benchmark.h
│       ├── blob.h
│       ├── c_api.h
│       ├── command.h
│       ├── cpu.h
│       ├── datareader.h
│       ├── gpu.h
│       ├── layer.h
│       ├── layer_shader_type.h
│       ├── layer_shader_type_enum.h
│       ├── layer_type.h
│       ├── layer_type_enum.h
│       ├── mat.h
│       ├── modelbin.h
│       ├── net.h
│       ├── opencv.h
│       ├── option.h
│       ├── paramdict.h
│       ├── pipeline.h
│       ├── pipelinecache.h
│       ├── platform.h
│       └── simplestl.h
├── linux
│   └── libncnn.a
├── macos
│   └── libncnn.a
├── ncnnConfig.cmake
└── windows
    └── ncnn.lib
```
**注意：如果你要使用此处下载的预编译库，请选择VS2019来编译。如果你打算自己编译，则可以选择低版本VS**
3. opencv 3
4. jdk1.8(可选)，编译为供java调用的动态运行库时，需要安装，编译为可执行文件无需安装java。

##### Windows命令行编译说明
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
6.  运行```build.cmd```并输入选项，编译为命令行可执行文件
7.  编译完成后运行```run-test-win.cmd```进行测试
8.  如果提示缺少"VCRUNTIME140_1.dll"，下载安装适用于 Visual Studio 2015、2017 和 2019 的 Microsoft Visual C++ 可再发行软件包，
[下载地址](https://support.microsoft.com/zh-cn/help/2977003/the-latest-supported-visual-c-downloads)
9. 编译动态运行库(可选，可用于java调用)
* 下载jdk-8u221-windows-x64.exe，安装选项默认(确保“源代码”项选中)，安装完成后，打开“系统”属性->高级->环境变量
* 新建“系统变量”，变量名```JAVA_HOME``` ，变量值```C:\Program Files\Java\jdk1.8.0_221``
* 新建“系统变量”，变量名```CLASSPATH``` ，变量值```.;%JAVA_HOME%\lib\dt.jar;%JAVA_HOME%\lib\tools.jar;``
* 编辑“系统变量”Path，Win7在变量值头部添加```%JAVA_HOME%\bin;``` ，win10直接添加一行```%JAVA_HOME%\bin```
* 开始菜单打开"x64 Native Tools Command Prompt for VS 2019"或"适用于 VS2017 的 x64 本机工具"，并转到OcrLiteOnnx根目录
* 运行```build.cmd```并输入选项，选择编译为动态运行库

##### Windows Visual Studio编译说明
* VS2015/VS2017/VS2019，cmake，opencv……等安装配置参考上述步骤。
* 运行generate-vs-project.cmd，输入数字选择要生成的visual studio项目解决方案版本。
* 根据你的编译环境，进入build-xxxx-x86或x64文件夹，打开OcrLiteOnnxToNcnn.sln。
* 在顶部工具栏选择Debug或Release，在右边的"解决方案"窗口，右键选中"ALL_BUILD"->生成。

##### Mac编译说明
1.  macOS Catalina 10.15.x
2.  自行下载安装HomeBrew
3.  下载opencv：
```
brew install opencv@3
brew ln opencv3 --force
```
* 编辑用户目录下的隐藏文件```.zshrc``` ，添加```export OpenCV_DIR="/usr/local/Cellar/opencv@3/3.4.10_4"```
4.  libomp: ```brew install libomp```
5.  编译：```./build.sh```并输入选项，编译为命令行可执行文件
6.  测试：```./run-test.sh```
7. 编译动态运行库(可选，可用于java调用)
* 下载jdk-8u221-macosx-x64.dmg，安装。
* 编辑用户目录下的隐藏文件```.zshrc``` ，添加```export JAVA_HOME=$(/usr/libexec/java_home)```
* 运行```build.sh```并输入选项，编译为动态运行库

##### Linux编译说明
1.  Deepin 20 或其它发行版
2.  安装build-essential:略……
3.  下载opencv：各发行版不大一样，略……
4.  编译：```./build.sh```并输入选项，编译为命令行可执行文件
5.  测试：```./run-test.sh```
6. 编译动态运行库(可选，可用于java调用)
* 下载安装jdk，略……
* 运行```build.sh```并输入选项，编译为动态运行库

#### 测试结果说明
1.  *-part-x.jpg为分割后的图片
2.  *-debug-x.jpg为分割后的图片进行方向识别后，校准方向后的图片
3.  *-result.jpg为图像分割画框的结果
4.  *-result.txt为识别的最终结果
5.  最终结果包含：
* getTextBoxesTime(图像分割耗时)、
* TextBoxScore(文字框置信度)、
* TextBoxPos(文字框坐标)、
* Angle(方向索引、方向置信度)、
* AngleTime(方向识别耗时)、
* textLine(识别到的文字)
* textScores(各个文字的置信度)、
* getTextLineTime(文字识别耗时)、
* TextBoxTime(方向识别+文字识别耗时)
* FullDetectTime（整张图片总耗时）

#### 输入参数说明
* 请参考main.h中的命令行参数说明。
* 每个参数有一个短参数名和一个长参数名，用短的或长的均可。
1. ```-d或--models```：模型所在文件夹路径，可以相对路径也可以绝对路径。
2. ```-i或--image```：目标图片路径，可以相对路径也可以绝对路径。
3. ```-t或--numThread```：线程数量。
4. ```-p或--padding```：图像预处理，用于提升识别率，文字框没有正确框住所有文字时，增加此值。
5. ```-s或--imgResize```：按图像长边进行总体缩放，放大增加识别耗时但精度更高，缩小减小耗时但精度降低。
6. ```-b或--boxScoreThresh```：文字框置信度门限，文字框没有正确框住所有文字时，减小此值。
7. ```-o或--boxThresh```：请自行试验。
8. ```-m或--minArea```：请自行试验。
9. ```-u或--unClipRatio```：单个文字框大小倍率，越大时单个文字框越大。
10. ```-a或--noAngle```：启用(1)/禁用(0) 文字方向检测，只有图片倒置的情况下(旋转90~270度的图片)，才需要启用文字方向检测。
11. ```-A或--mostAngle```：启用(1)/禁用(0) 角度投票(整张图片以最大可能文字方向来识别)，当禁用文字方向检测时，此项也不起作用。
12. ```-?或--help```：打印命令行帮助。

#### 编译参数说明
build.sh目前有2个编译参数
1. ```OCR_LITE_OPENMP=ON```：启用(ON)或禁用(OFF) AngleNet和CrnnNet阶段使用OpenMP并行运算。
* 测试硬件：NUC8I7HVK冥王峡谷(i7-8809G) 32GRAM 2TSSD
* 测试条件：不计算模型加载、框架初始化、加载图片的时间，线程数设置为当前CPU逻辑核心数量，跑100次计算平均耗时。
* 经过测试，dbNet阶段使用OpenMP不能减少耗时
* 所以此选项仅影响angelNet和crnnNet阶段的代码
* 因为ncnn的crnn阶段因算子不支持而采用了外循环，所以在外循环使用OpenMP后，效果显著
* 为了对比测试，选取了一张640x640的普通图片和一张分辨率为810*12119长图进行测试。
* 第一个表格为普通图的测试结果，第二个图为长图的测试结果
* 结论：启用OpenMP能快10%~30%(取决于图片文字数量)

| 平台    | 系统版本  | 单线程 | OpenMP | 耗时倍率 |
| ------- | ------- | ----: | ----: | ----: |
| macOS   | 10.15.7 | 254ms | 227ms | 1.12 |
| Windows | 10 x64  | 334ms | 308ms | 1.08 |

| 平台    | 系统版本  | 单线程 | OpenMP | 耗时倍率 |
| ------- | ------- | ----: | ----: | ----: |
| macOS   | 10.15.7 | 8427ms | 6055ms | 1.39 |
| Windows | 10 x64  | 12379ms | 9621ms | 1.28 |



2. ```OCR_LITE_LIB=ON```： 启用(ON)或禁用(OFF) 编译为jni lib

