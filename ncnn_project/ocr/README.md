

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
# 2020.03.16更新
- psenet ncnn核扩展实现，有效解决粘连文本检测问题

# ncnn检测识别展示(x86 cpu 单进程)
### 未实现核扩展
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/ncnn_project/ocr/res_imgs/res_1.jpg"/>
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/ncnn_project/ocr/res_imgs/res_2.jpg"/>

### 实现核扩展
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/ncnn_project/ocr/res_imgs/res_3.jpg"/>


