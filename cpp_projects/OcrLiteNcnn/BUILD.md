# 编译说明

### 依赖的第三方库下载

1. 下载opencv，[下载地址](https://github.com/RapidAI/OpenCVBuilder/releases)

* OpenCV静态库：opencv-(版本号)-平台.7z，
* 把压缩包解压到项目根目录，windows平台需要注意目录层次，解压后目录结构如下
* windows平台请选择vs2017版本(也可以用于vs2019编译)
* 程序版本v1.6.0需要opencv 4.5.4
```
OcrLiteNcnn/opencv-static
├── OpenCVWrapperConfig.cmake
├── linux
├── macos
├── windows-x64
└── windows-x86
```

2. 下载ncnn，[下载地址](https://github.com/benjaminwan/NcnnBuilder/releases)

* ncnn静态库含vulkan：ncnn-(版本号)-vulkan-平台.7z
* ncnn静态库：ncnn-(版本号)-平台.7z
* 注意：本demo从v1.5.1开始，去除crnn的lstm外循环改用框架的实现，需要配合ncnn>=20210124，请注意选择ncnn版本。
* 程序版本v1.6.0需要ncnn 20210720
* 把压缩包解压到项目根目录，windows平台需要注意目录层次，解压后目录结构如下

```
OcrLiteNcnn/ncnn-static
├── NcnnWrapperConfig.cmake
├── linux
├── macos
├── windows-x64
└── windows-x86

OcrLiteNcnn/ncnn-vulkan-static
├── NcnnWrapperConfig.cmake
├── linux
├── macos
├── windows-x64
└── windows-x86
```

3. Vulkan SDK，[下载地址](https://vulkan.lunarg.com/sdk/home)

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
3. Linux Ubuntu 1804 x64

**注意：以下说明仅适用于本机编译。如果需要交叉编译为arm等其它平台(参考android)，则需要先交叉编译所有第三方依赖库(ncnn、opencv)，然后再把依赖库整合替换到本项目里。**

### Windows编译说明

#### Windows nmake编译

1. 安装VS2017或VS2019，安装时，至少选中'使用C++的桌面开发'
2. cmake>=3.12请自行下载&配置，[下载地址](https://cmake.org/download/)
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
2. 自行下载安装HomeBrew，cmake >=3.17[下载地址](https://cmake.org/download/)
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

1. Ubuntu18.04 LTS 其它发行版(请自行编译依赖库opencv和ncnn，或自行适配官方发布的动态库)
2. ```sudo apt-get install build-essential```
3. g++>=5，cmake>=3.17[下载地址](https://cmake.org/download/)
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

1. ```OCR_OUTPUT="BIN"或"JNI"或"CLIB"```： BIN时编译为可执行文件，JNI时默认编译为java jni 动态库，CLIB时便以为 C动态库；
2. ```OCR_VULKAN="CPU"或"GPU"```： CPU时则选择不带vulkan的版本编译，GPU时选择ncnn(带vulkan)静态库进行编译；