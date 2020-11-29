# OcrLiteOnnx

#### Demo下载(win、mac、linux)
[Gitee下载](https://gitee.com/benjaminwan/ocr-lite-onnx/releases)

#### 介绍
ChineseOcr Lite Onnx，超轻量级中文OCR PC Demo，支持onnxruntime推理(DBNet+AngleNet+CRNN)

**对应chineseocr lite的onnx分支**

这个项目使用onnxruntime框架进行推理

详情请查看 [https://github.com/ouyanghuiyu/chineseocr_lite](https://github.com/ouyanghuiyu/chineseocr_lite)

采用onnxruntime框架[https://github.com/microsoft/onnxruntime](https://github.com/microsoft/onnxruntime)

整合了如下算法：
DBNet(图像分割)+AngleNet(文字方向检测)+CRNN(文字识别)

[模型下载地址](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/models)
```
OcrLiteOnnx/models
├── angle_net.onnx
├── crnn_lite_lstm.onnx
├── dbnet.onnx
└── keys.txt
```

#### 编译环境
1. cmake 3.18.4
2. onnx动态库版本为1.5.2，[下载地址](https://github.com/microsoft/onnxruntime/releases/tag/v1.5.2)，根据需要编译的平台，把文件解压到项目根目录。解压后目录结构为
```
OcrLiteOnnx/onnx
├── ONNXConfig.cmake
├── include
│   └── onnx
│       ├── cpu_provider_factory.h
│       ├── cuda_provider_factory.h
│       ├── onnxruntime_c_api.h
│       ├── onnxruntime_cxx_api.h
│       ├── onnxruntime_cxx_inline.h
│       └── onnxruntime_session_options_config_keys.h
├── linux
│   ├── libonnxruntime.so -> libonnxruntime.so.1.5.2
│   └── libonnxruntime.so.1.5.2
├── macos
│   ├── libonnxruntime.1.5.2.dylib
│   └── libonnxruntime.dylib -> libonnxruntime.1.5.2.dylib
└── windows
    ├── onnxruntime.dll
    ├── onnxruntime.lib
    └── onnxruntime.pdb
```
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

#### 关于设置线程数
onnxruntime设置线程数分为2个部分:
1. execution within nodes，这个线程数是通过openmp的环境变量设置
```set OMP_NUM_THREADS=<CPU逻辑核心数量>```
2. execution of the graph，这个线程数可以通过代码设置
```sessionOptions.SetInterOpNumThreads(numThread);```
测试脚本通过检查当前的CPU逻辑核心数量，设置环境变量来配置第一个线程数，通过命令行传入第三个参数numThread来配置第二个线程数

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
2. ```OCR_LITE_LIB=ON```： 启用(ON)或禁用(OFF) 编译为jni lib

#### Windows静态编译opencv3(非必须步骤)
* 不想自己编译的话，可以从顶部地址下载编译好的库文件。
* 编译环境vs2017/vs2019，cmake，git……等请参考前文配置，或自行查找资料。
* 因为只是用opencv做一些图像处理和变换，所以不需要它自带的推理模块，video模块等等，可以去掉这些模块以减小程序大小。
* 静态编译opencv，让编译出来的可执行程序不用再依赖外部的dll。

##### 同步opencv源码
```
下载安装cgit(通过gitclone缓存加速)
cgit clone https://github.com/opencv/opencv.git
```

##### 检出opencv3.4.11
```
cd opencv
git checkout 3.4.11
```

##### 选择opencv编译脚本
根据自己的编译环境，选择“scripts”文件夹里任一脚本，并复制到opencv文件夹：
```
build-opencv-win-vs-full.bat：windows下使用visutal studio编译opencv完整版
build-opencv-win-vs-lite.bat：windows下使用visutal studio编译opencv精简版
build-opencv-win-nmake-full.bat：windows下使用命令行编译opencv完整版
build-opencv-win-nmake-lite.bat：windows下使用命令行编译opencv精简版
```

##### 编译opencv@Windows by visutal studio
* 用cmd执行bat后，会创建4个文件夹，根据你的开发环境，进入对应的文件夹，用Visual Studio打开OpenCV.sln
* 先在工具栏选择你要的Debug或者Release
* 然后在右边的“解决方案资源管理器”里找到“ALL_BUILD”->右键->执行“生成”
* 编译成功后，同样在右边找到“INSTALL”->右键->执行“生成”
* 生成的sdk包在install文件夹里

##### 编译opencv@Windows by nmake
* 跟上述的步骤没什么不同，二选一即可
* 开始菜单找到如下命令工具，转到opencv目录执行bat脚本，会生成对应x86或x64的编译结果：
```
"x64 Native Tools Command Prompt for VS 2019" : vs2019编译64位应用程序
"x86 Native Tools Command Prompt for VS 2019" : vs2019编译32位应用程序
"适用于 VS2017 的 x64 本机工具" : vs2017编译64位应用程序
"x86 Native Tools Command Prompt for VS 2017" : vs2017编译64位应用程序
```
nmake直接会生成sdk包，在build-xxx的install文件夹里

##### 修复opencv静态库
1. (完整版不需要此步骤)精简版因为许多模块去除了，要把头文件的引用也注释，否则会造成编译错误
```
修改install/x64(或x86)/include/opencv/cv.h，注释如下行
//#include "opencv2/photo/photo_c.h"
//#include "opencv2/video/tracking_c.h"
//#include "opencv2/objdetect/objdetect_c.h"

修改install/x64(或x86)/include/opencv/cv.hpp，注释如下行
//#include "opencv2/photo.hpp"
//#include "opencv2/video.hpp"
//#include "opencv2/features2d.hpp"
//#include "opencv2/calib3d.hpp"
//#include "opencv2/objdetect.hpp"
```
2. 修改staticlib的OpenCVConfig.cmake文件
```
修改install\x64(或x86)\vc15(或vc16)\staticlib\OpenCVConfig.cmake

把此行
set(OpenCV_LIB_COMPONENTS opencv_core;opencv_highgui;opencv_imgcodecs;opencv_imgproc;opencv_world)

修改成这样
set(OpenCV_LIB_COMPONENTS opencv_world)
```
3. 修改总体的OpenCVConfig.cmake
```
修改install\OpenCVConfig.cmake
添加在if(CMAKE_VERSION VERSION_GREATER 2.6)前添加一行
set(OpenCV_STATIC ON)
```

#### Windows静态编译onnxruntime
* 不想自己编译的话，可以从顶部地址下载编译好的库文件。
* 同步源代码：略……
* 在scripts文件夹里找到需要的脚本，并复制到onnxruntime源代码目录
```
build-onnxruntime-win-x64.bat：编译64位版本
build-onnxruntime-win-x86.bat：编译32位版本
```
脚本里的```--skip_submodule_sync```参数，用于跳过每次编译前的同步子项目源代码步骤，第一次执行编译请去掉此参数，让编译脚本完整同步完所有源代码
编译成功后，在builid文件夹里找到相关的lib，并复制到onnx目录，最终目录结构如下
```
OcrLiteOnnx/onnx
├── ONNXConfig.cmake
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
└── windows
    ├── flatbuffers.lib
    ├── libprotobuf-lite.lib
    ├── onnx.lib
    ├── onnx_proto.lib
    ├── onnxruntime_common.lib
    ├── onnxruntime_flatbuffers.lib
    ├── onnxruntime_framework.lib
    ├── onnxruntime_graph.lib
    ├── onnxruntime_mlas.lib
    ├── onnxruntime_optimizer.lib
    ├── onnxruntime_providers.lib
    ├── onnxruntime_session.lib
    ├── onnxruntime_util.lib
    └── re2.lib

```
