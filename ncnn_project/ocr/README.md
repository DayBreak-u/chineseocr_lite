

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


