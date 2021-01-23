# OcrLiteOnnxVB

#### 介绍
* 本项目为Windows平台VB.Net WinForm范例。
* 代码是从OcrLiteOnnxCs项目翻译过来，理论上是一样的，但某些计算值可能因为语言本身而稍有差异
* 依赖的包：Emgu.CV、MicroSoft.ML.OnnxRuntime、clipper_library

#### 编译环境
1. Windows 10 x64
2. Visual Studio 2017或以上

##### 编译说明
1. Vs2017打开OcrLiteOnnxVB.sln。
2. 解决方案资源管理器->OcrLiteLib->右键->管理NuGet程序包->浏览->搜索->安装
* 注意：Emgu.CV要选作者是“Emgu Corporation”
* Emgu.CV 4.4.0.4099
* Emgu.CV.runtime.windows 4.4.0.4099
* MicroSoft.ML.OnnxRuntime 1.5.2
* clipper_library 6.2.1
3. 解决方案资源管理器->OcrLiteOnnxForm->右键->管理NuGet程序包->浏览->搜索->安装
* 注意：Emgu.CV要选作者是“Emgu Corporation”
* Emgu.CV 4.4.0.4099
* Emgu.CV.Bitmap 4.4.0.4099
4. 确保：OcrLiteOnnxForm设为启动项目
5. 确保：OcrLiteOnnxForm->右键->属性->生成->平台目标:x64
6. 确保：OcrLiteLib->右键->属性->生成->平台目标:x64
7. 生成解决方案
8. 把models文件夹复制到```\OcrLiteOnnxVB\OcrLiteOnnxForm\bin\Debug(或Release)```
* [模型下载地址](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/models)
* [keys.txt下载地址](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/cpp_projects/OcrLiteOnnx/models)
```
Debug\models
├── angle_net.onnx
├── crnn_lite_lstm.onnx
├── dbnet.onnx
└── keys.txt
```
9. 运行

##### 其它
* 修改线程数，必须“重新初始化”才能生效
* “models”按钮修改模型路径，必须“重新初始化”才能生效
* 输入参数说明请参考[OcrLiteOnnx项目](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/cpp_projects/OcrLiteOnnx)