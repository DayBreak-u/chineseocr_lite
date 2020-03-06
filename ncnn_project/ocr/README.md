

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
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/ncnn_project/ocr/res_imgs/res_1.jpg"/>
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/ncnn_project/ocr/res_imgs/res_2.jpg"/>

因为ncnn模型都是dense的 lstm的没转成功，效果差的不少，以后继续优化
