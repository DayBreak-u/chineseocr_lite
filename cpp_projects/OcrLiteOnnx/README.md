# OcrLiteOnnx

### 联系方式

* QQ①群：994929053
* QQ②群：820132154
* QQ③群：904091319

### Project下载

* 整合好源码和依赖库的完整工程项目，文件比较大，可到Q群共享内下载，以Project开头的压缩包文件为源码工程，例：Project_OcrLiteOnnx-版本号.7z
* 如果想自己折腾，则请继续阅读本说明

### Demo下载(win、mac、linux)

* 编译好的demo，可以在release中下载，或者Q群共享内下载
* 或者[Gitee下载](https://gitee.com/benjaminwan/ocr-lite-onnx/releases)
* 或者[Github下载](https://github.com/benjaminwan/OcrLiteOnnx/releases)
* 各平台可执行文件：linux-bin.7z、macos-bin.7z、windows-bin.7z
* 用于java的jni库：linux-jni.7z、macos-jni.7z、windows-jni.7z
* 用于C的动态库：linux-clib.7z、macos-clib.7z、windows-clib.7z
* 注意：linux编译平台为ubuntu18.04，如果你的linux版本无法运行demo，请自行从源码编译依赖库和完整项目。

### 介绍

ChineseOcr Lite Onnx，超轻量级中文OCR PC Demo，支持onnxruntime推理

**对应chineseocr lite的onnx分支**

这个项目使用onnxruntime框架进行推理

详情请查看 [https://github.com/ouyanghuiyu/chineseocr_lite](https://github.com/ouyanghuiyu/chineseocr_lite)

采用onnxruntime框架[https://github.com/microsoft/onnxruntime](https://github.com/microsoft/onnxruntime)

### 更新说明

#### 2021-10-25 update

* fix:win编译打包C动态库缺少lib文件
* 编译c动态库添加打包include

#### 2021-10-15 update

* opencv 4.5.4
* onnxruntime 1.9.0
* 优化编译脚本和测试脚本

### 模型下载

[模型下载地址](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/models)
下载后解压到项目根目录

```
OcrLiteOnnx/models
    ├── angle_net.onnx
    ├── crnn_lite_lstm.onnx
    ├── dbnet.onnx
    └── keys.txt
```

### [编译说明](./BUILD.md)

### 测试说明

1. 根据系统下载对应的程序包linux-bin.7z、macos-bin.7z、windows-bin.7z，并解压.
2. 把上面的模型下载，解压到第一步解压的文件夹里.
3. 终端运行run-test.sh或命令行运行run-test.bat，查看识别结果.
4. 终端运行run-benchmark.sh或命令行运行run-benchmark.bat，查看识别过程平均耗时.

### FAQ

#### macOS缺少openmp

```brew install libomp```

#### windows提示缺少"VCRUNTIME140_1.dll"

下载安装适用于 Visual Studio 2015、2017 和 2019 的 Microsoft Visual C++ 可再发行软件包
[下载地址](https://support.microsoft.com/zh-cn/help/2977003/the-latest-supported-visual-c-downloads)

#### Windows7执行错误|中文乱码

1. cmd窗口左上角-属性
2. 字体选项卡-选择除了“点阵字体”以外的TrueType字体,例如:Lucida Console、宋体
3. 重新执行bat

### 输入参数说明

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

### 关于内存泄漏与valgrind

* 项目根目录的valgrind-memcheck.sh用来检查内存泄漏(需要debug编译)。
* valgrind-memcheck.txt是demo在linux平台的检查报告。
* 报告中的"possibly lost"均发生在第三方库，possibly lost可能不一定是泄露，暂时不管。