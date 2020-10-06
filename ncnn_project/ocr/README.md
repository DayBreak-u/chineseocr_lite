#说明


CMakeSettings.json  是vs2019 使用的cmake设置。 可以直接用vs2019打开而不需要转成vs工程。
vs2019可以开发跨平台项目，特别是以CMake方式进行项目管理时。
# 编译
``` Bash
cd ocr 
mkdir build &&  cd build 
cmake  ..
make 
```
# 识别
``` Bash
./Text_recognition ../../../test_imgs/1.jpg 640
```

# ncnn检测识别展示(x86 cpu 单进程)

<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/onnx/ncnn_project/ocr/res_imgs/result.jpg"/>


