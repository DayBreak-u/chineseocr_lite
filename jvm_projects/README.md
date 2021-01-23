# Jvm版OcrLite范例

#### 介绍
本目录存放Jvm相关demo项目
1. OcrLiteOnnxJvm项目是通过jni调用OcrLiteOnnx动态运行库的范例，内含java和kotlin调用范例，附调试和编译相关说明
* 因测试设备的限制，onnxruntime在macOS不支持AMD显卡，没有跨平台的GPU支持方案，所以目前Onnxruntime版仅支持cpu计算。
* 如果想在windows或linux下启用GPU加速，除了装好显卡驱动，cuda sdk，以及gpu版onnxruntime以外，还必须修改代码，请自行到onnxruntime官方仓库寻找参考范例。

-----
  
2. OcrLiteNcnnJvm项目是通过jni调用OcrLiteNcnn动态运行库的范例，内含java和kotlin调用范例，附调试和编译相关说明
* ncnn版支持Vulkan GPU加速，并支持跨平台，实测在macOS和windows下工作良好，ncnn版分为两个版本，一个版本仅支持CPU计算，另一个版本带vulkan支持。
* 带vulkan支持的ncnn版本，需要先安装Vulkan SDK，Windows和LInux还需要安装显卡驱动，且显卡驱动要有vulkan支持。
* Intel核芯显卡，AMD显卡，NVIDIA显卡大部分都支持Vulkan。
* 关于GPU加速，Intel核芯显卡和AMD低端显卡都能获得非常不错的加速效果，Intel核芯显卡dbNet提速约一倍以上，AMD低端显卡(RX560D￥2xx)dbNet提速5~10倍左右，因测试设备的限制，没有条件测试NVIDIA显卡。
* [详细的测试结果](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/cpp_projects)

