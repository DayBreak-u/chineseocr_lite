### 介绍

ChineseOcr Lite Mnn，超轻量级中文OCR PC Demo，使用MNN推理

这个项目使用MNN框架进行推理，使用  [https://github.com/ouyanghuiyu/chineseocr_lite](https://github.com/ouyanghuiyu/chineseocr_lite) 项目中提供的模型，迁移了项目中cpp_projects中的[OcrLiteOnnx](https://github.com/DayBreak-u/chineseocr_lite/tree/onnx/cpp_projects/OcrLiteOnnx)项目，采用阿里MNN框架[https://github.com/alibaba/MNN](https://github.com/alibaba/MNN) 作为推理引擎实现。


## 快速使用

### 一、依赖的第三方库下载

下载opencv和MNN 

* OpenCv动态库、OpenCv静态库 [下载地址](https://gitee.com/benjaminwan/ocr-lite-onnx/releases/v1.0)
* MNN 动态库：mnn-1.1.4-shared.zip [下载地址](https://github.com/thomaszheng/OcrLiteMnn/releases/tag/v0.0.1)
* MNN 静态库：mnn-1.1.4-static.zip
* 可以选择只下载两者的动态库或两者的静态库(要么都是静态库要么都是动态库)，或者4种全部下载
* 把压缩包解压到项目根目录，解压后目录结构

```
OcrLiteMnn
    ├── mnn-shared
    ├── mnn-static
    ├── opencv-shared
    ├── opencv-static
```

### 二、下载模型

```
OcrLiteMnn
    ├── models
          ├── angle_net.mnn
          ├── crnn_lite_lstm.mnn
          ├── dbnet.mnn
```

### 三、编译

macOS 10.15

**注意：以下说明仅适用于本机编译。如果需要交叉编译为arm等其它平台(参考android)，则需要先交叉编译所有第三方依赖库(mnn、opencv)，然后再把依赖库整合替换到本项目里。**

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


### 四、编译参数说明

build.sh编译参数：

2. ```OCR_LIB=ON```： 启用(ON)或禁用(OFF) ON时编译为jni lib，OFF时编译为可执行文件
3. ```OCR_STATIC=ON```： 启用(ON)或禁用(OFF) ON时选择opencv和onnxruntime的静态库进行编译，OFF时则选择动态库编译

### 五、输入参数说明

* 请参考main.h中的命令行参数说明。
* 每个参数有一个短参数名和一个长参数名，用短的或长的均可。

1. ```-d或--models```：模型所在文件夹路径，可以相对路径也可以绝对路径。
2. ```-1或--det```:dbNet模型文件名(含扩展名)
3. ```-2或--cls```:angleNet模型文件名(含扩展名)
4. ```-3或--rec```:crnnNet模型文件名(含扩展名)
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

### 六、关于内存泄漏与valgrind

* 项目根目录的valgrind-memcheck.sh用来检查内存泄漏(需要debug编译)。
* valgrind-memcheck.txt是demo在linux平台的检查报告。
* 报告中的"possibly lost"均发生在第三方库，possibly lost可能不一定是泄露，暂时不管。