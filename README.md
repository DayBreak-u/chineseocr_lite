# chineseocr_lite 的 onnx 推理, 部署简单

## 原始项目分支（torch推理，ncnn推理等）：[master](https://github.com/ouyanghuiyu/chineseocr_lite/tree/master)

## 环境
- python3.6

- linux/macos/windows


## 20200820 更新
  dbnet + crnn_lite(新版本) ncnn 实现

## web服务启动
``` Bash
cd chineseocr_lite## 进入chineseocr目录
python backend/main.py 
```

## 在线体验(lite版本<模型还在训练中>，支持竖排，空格识别，文字行方向旋转)： http://ocr.timeline229.com:8089/

## 识别结果展示

<p align="center"><img src="test_imgs/res.jpg"\></p>
<p align="center"><img src="test_imgs/res_2.jpg"\></p>


## 参考
1. TrWebOCR https://github.com/alisen39/TrWebOCR         

## QQ群
994929053
