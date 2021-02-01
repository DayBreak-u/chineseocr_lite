# OcrLiteNcnn

### Project下载

* 有整合好源码和依赖库的完整工程项目，文件比较大，可到Q群共享内下载，找以Project开头的压缩包文件
* 如果想自己折腾，则请继续阅读本说明

### Demo下载(win、mac、linux)

编译好的demo文件可以到Q群共享内下载 或者[Gitee下载](https://gitee.com/benjaminwan/ocr-lite-ncnn/releases)

### 介绍

ChineseOcr Lite Ncnn，超轻量级中文OCR PC Demo，支持ncnn推理

**对应chineseocr lite的onnx分支**

这个项目使用ncnn框架进行推理，模型是从onnx模型转成了ncnn模型格式。

详情请查看 [https://github.com/ouyanghuiyu/chineseocr_lite](https://github.com/ouyanghuiyu/chineseocr_lite)

采用ncnn神经网络前向计算框架[https://github.com/Tencent/ncnn](https://github.com/Tencent/ncnn)

### 模型下载

[模型下载地址](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/models_ncnn)
下载后解压到项目根目录

```
OcrLiteNcnn/models
    ├── angle_op.bin
    ├── angle_op.param
    ├── crnn_lite_op.bin
    ├── crnn_lite_op.param
    ├── dbnet_op.bin
    ├── dbnet_op.param
    └── keys.txt
```

### 依赖的第三方库下载

1. 下载opencv和ncnn，[下载地址](https://gitee.com/benjaminwan/ocr-lite-ncnn/releases/v1.0)

* OpenCv动态库：opencv-(版本号)-sharedLib.7z
* OpenCv静态库：opencv-(版本号)-staticLib.7z
* ncnn静态库含vulkan：ncnn-(版本号)-vulkan-staticLib.7z
* ncnn静态库：ncnn-(版本号)-staticLib.7z
* 把压缩包解压到项目根目录，解压后目录结构
* 注意：本demo从v1.5.1开始，去除crnn的lstm外循环改用框架的实现，需要配合ncnn>=20210124，请注意选择ncnn版本。

```
OcrLiteNcnn
    ├── ncnn-static
    ├── ncnn-vulkan-static
    ├── opencv-shared
    ├── opencv-static
```

2. Vulkan SDK，[下载地址](https://vulkan.lunarg.com/sdk/home)

* 如果想编译ncnn带vulkan支持的版本，则必须先安装Vulkan SDK。
* 一般下载最新版即可，当前最新版1.2.162.0
* Windows：直接双击安装。
* macOS：加载dmg后，终端执行```./install_vulkan.py```
* Linux: 解压tar.gz文件后，把scripts文件夹里的install-vulkan-linux.sh复制到解压后的文件夹，并打开终端执行

```
chmod a+x install-vulkan-linux.sh
./install-vulkan-linux.sh
```

### 编译环境

1. Windows 10 x64
2. macOS 10.15
3. Linux Ubuntu 1604 x64

**注意：以下说明仅适用于本机编译。如果需要交叉编译为arm等其它平台(参考android)，则需要先交叉编译所有第三方依赖库(ncnn、opencv)，然后再把依赖库整合替换到本项目里。**

### Windows编译说明

#### Windows nmake编译

1. 安装VS2017或VS2019，安装时，至少选中'使用C++的桌面开发'
2. cmake请自行下载&配置，[下载地址](https://cmake.org/download/)
3. 开始菜单打开"x64 Native Tools Command Prompt for VS 2019"或"适用于 VS2017 的 x64 本机工具"，并转到本项目根目录
4. 运行```build.bat```并按照提示输入选项，最后选择'编译成可执行文件'
5. 编译完成后运行```run-test.bat```进行测试(注意修改脚本内的目标图片路径)
6. 编译JNI动态运行库(可选，可用于java调用)

* 下载jdk-8u221-windows-x64.exe，安装选项默认(确保“源代码”项选中)，安装完成后，打开“系统”属性->高级->环境变量
* 新建“系统变量”，变量名```JAVA_HOME``` ，变量值```C:\Program Files\Java\jdk1.8.0_221``
* 新建“系统变量”，变量名```CLASSPATH``` ，变量值```.;%JAVA_HOME%\lib\dt.jar;%JAVA_HOME%\lib\tools.jar;``
* 编辑“系统变量”Path，Win7在变量值头部添加```%JAVA_HOME%\bin;``` ，win10直接添加一行```%JAVA_HOME%\bin```
* 开始菜单打开"x64 Native Tools Command Prompt for VS 2019"或"适用于 VS2017 的 x64 本机工具"，并转到本项目根目录
* 运行```build.bat```并按照提示输入选项，最后选择'编译成JNI动态库'

#### Windows Visual Studio编译说明

1. VS2017/VS2019，cmake……等安装配置参考上述步骤。
2. 运行generate-vs-project.bat，输入数字选择要生成的visual studio项目解决方案版本。
3. 根据你的编译环境，进入build-xxxx-x86或x64文件夹，打开OcrLiteNcnn.sln。
4. 在顶部工具栏选择Release，在右边的"解决方案"窗口，右键选中"ALL_BUILD"->生成。要选择Debug，则您必须自行编译Debug版的opencv或ncnn。

#### Windows部署说明

1. 编译选项选择第三方库为动态库时，部署的时候记得把dll复制到可执行文件目录。
2. 部署时如果提示缺少"VCRUNTIME140_1.dll"，下载安装适用于 Visual Studio 2015、2017 和 2019 的 Microsoft Visual C++ 可再发行软件包，
   [下载地址](https://support.microsoft.com/zh-cn/help/2977003/the-latest-supported-visual-c-downloads)

### Mac编译说明

1. macOS Catalina 10.15.x，安装Xcode 12.1，并安装Xcode Command Line Tools, 终端运行```xcode-select –install```
2. 自行下载安装HomeBrew，cmake >=3.1[下载地址](https://cmake.org/download/)
3. libomp: ```brew install libomp```
4. 终端打开项目根目录，```./build.sh```并按照提示输入选项，最后选择'编译成可执行文件'
5. 测试：```./run-test.sh```(注意修改脚本内的目标图片路径)
6. 编译JNI动态运行库(可选，可用于java调用)

* 下载jdk-8u221-macosx-x64.dmg，安装。
* 编辑用户目录下的隐藏文件```.zshrc``` ，添加```export JAVA_HOME=$(/usr/libexec/java_home)```
* 运行```build.sh```并按照提示输入选项，最后选择'编译成JNI动态库'

#### macOS部署说明

opencv或onnxruntime使用动态库时，参考下列方法：

* 把动态库所在路径加入DYLD_LIBRARY_PATH搜索路径
* 把动态库复制或链接到到/usr/lib

### Linux编译说明

1. Ubuntu16.04 LTS 或其它发行版
2. ```sudo apt-get install build-essential```
3. g++>=5，cmake>=3.1[下载地址](https://cmake.org/download/)
4. 终端打开项目根目录，```./build.sh```并按照提示输入选项，最后选择'编译成可执行文件'
5. 测试：```./run-test.sh```(注意修改脚本内的目标图片路径)
6. 编译JNI动态运行库(可选，可用于java调用)

* 下载jdk-8u221并安装配置
* 运行```build.sh```并按照提示输入选项，最后选择'编译成JNI动态库'
* **注意：编译JNI时，g++版本要求>=6**

#### Linux部署说明

opencv或onnxruntime使用动态库时，参考下列方法：

* 把动态库所在路径加入LD_LIBRARY_PATH搜索路径
* 把动态库复制或链接到到/usr/lib

### 编译参数说明

build.sh编译参数

1. ```OCR_OPENMP=ON```：启用(ON)或禁用(OFF) AngleNet和CrnnNet阶段使用OpenMP并行运算。
2. ```OCR_LIB=ON```： 启用(ON)或禁用(OFF) ON时编译为jni lib，OFF时编译为可执行文件
3. ```OCR_STATIC=ON```： 启用(ON)或禁用(OFF) ON时选择opencv静态库进行编译，OFF时则选择动态库编译
4. ```OCR_VULKAN=ON```： 启用(ON)或禁用(OFF) ON时选择ncnn(带vulkan)静态库进行编译，OFF时则选择不带vulkan的版本编译

### 输入参数说明

* 请参考main.h中的命令行参数说明。
* 每个参数有一个短参数名和一个长参数名，用短的或长的均可。

1. ```-d或--models```：模型所在文件夹路径，可以相对路径也可以绝对路径。
2. ```-1或--det```:dbNet模型文件名(不含扩展名)
3. ```-2或--cls```:angleNet模型文件名(不含扩展名)
4. ```-3或--rec```:crnnNet模型文件名(不含扩展名)
5. ```-4或--keys```:keys.txt文件名(含扩展名)
6. ```-i或--image```：目标图片路径，可以相对路径也可以绝对路径。
7. ```-t或--numThread```：线程数量。
8. ```-p或--padding```：图像预处理，在图片外周添加白边，用于提升识别率，文字框没有正确框住所有文字时，增加此值。
9. ```-s或--maxSideLen```
   ：按图片最长边的长度，此值为0代表不缩放，例：1024，如果图片长边大于1024则把图像整体缩小到1024再进行图像分割计算，如果图片长边小于1024则不缩放，如果图片长边小于32，则缩放到32。
10. ```-b或--boxScoreThresh```：文字框置信度门限，文字框没有正确框住所有文字时，减小此值。
11. ```-o或--boxThresh```：请自行试验。
12. ```-u或--unClipRatio```：单个文字框大小倍率，越大时单个文字框越大。此项与图片的大小相关，越大的图片此值应该越大。
13. ```-a或--doAngle```：启用(1)/禁用(0) 文字方向检测，只有图片倒置的情况下(旋转90~270度的图片)，才需要启用文字方向检测。
14. ```-A或--mostAngle```：启用(1)/禁用(0) 角度投票(整张图片以最大可能文字方向来识别)，当禁用文字方向检测时，此项也不起作用。
15. ```-h或--help```：打印命令行帮助。
16. ```-G或--GPU```：尝试使用gpu进行计算，-1(使用CPU)/0(使用GPU0)/1(使用GPU1)/...，GPU选择失败时，则使用CPU进行计算。

### 编译脚本说明

* scripts文件夹内有一些脚本，用于自行编译ncnn，另有编译opencv的脚本，请到OcrLiteOnnx项目内寻找。

### 关于内存泄漏与valgrind

* 项目根目录的valgrind-memcheck.sh用来检查内存泄漏(需要debug编译)。
* valgrind-memcheck.txt是demo在linux平台的检查报告。
* 报告中的"possibly lost"均发生在第三方库，possibly lost可能不一定是泄露，暂时不管。