# chineseocr_lite 的 onnx 推理, 部署简单

## 原始项目分支（torch推理，ncnn推理等）：[master](https://github.com/ouyanghuiyu/chineseocr_lite/tree/master)

## 环境
- python3.6

- linux/macos/windows

## web服务启动
``` Bash
cd chineseocr_lite## 进入chineseocr目录
python backend/main.py 
```
## 以下为从Python代码翻译为各种语言的Demo，仅供参考:
* [C++ ncnn Demo](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/pc_projects/OcrLiteOnnxToNcnn)
* [C++ onnx Demo](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/pc_projects/OcrLiteOnnx)
* [Android ncnn Demo](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/android_projects)
* [.Net Demo](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/dotnet_projects)
* [Jvm Demo](https://github.com/ouyanghuiyu/chineseocr_lite/tree/onnx/jvm_projects)

## 在线体验(lite版本<模型还在训练中>，支持竖排，空格识别，文字行方向旋转)： http://ocr.timeline229.com:8089/

## 识别结果展示

<p align="center"><img src="test_imgs/res.jpg"\></p>
<p align="center"><img src="test_imgs/res_2.jpg"\></p>


## 参考
1. TrWebOCR https://github.com/alisen39/TrWebOCR         

## QQ群
994929053
