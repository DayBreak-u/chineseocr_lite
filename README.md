## 本项目基于[chineseocr](https://github.com/chineseocr/chineseocr) 与[psenet](https://github.com/WenmuZhou/PSENet.pytorch)  实现中文自然场景文字检测及识别

# 环境
pytorch  1.2.0
linux/macos
## PSENET 编译
``` Bash
cd psenet/pse
rm -rf pse.so 
make 
```
  
# 实现功能
- [x]  提供轻量的backone检测模型psenet（8.5M）,crnn_lstm_lite(9.5M) 和行文本方向分类网络（1.5M）
- [x]  任意方向文字检测，识别时判断行文本方向 
- [x]  crnn\crnn_lite lstm\dense识别（ocr-dense和ocr-lstm是搬运[chineseocr](https://github.com/chineseocr/chineseocr)的）   
- [x]  支持竖排文本识别  
- [x]  ncnn 实现 (支持lstm)
- [ ]  mnn  实现 



# 2020.03.14更新
- nihui大佬实现的[crnn_lstm推理](https://github.com/ouyanghuiyu/chineseocr_lite/pull/41) 具体操作详解: [详细记录超轻量中文OCR LSTM模型ncnn实现](https://zhuanlan.zhihu.com/p/113338890?utm_source=qq&utm_medium=social&utm_oi=645149500650557440)
- 升级crnn_lite_lstm_dw.pth模型crnn_lite_lstm_dw_v2.pth , 精度更高
- 提供竖排文字样例以及字体库（旋转90度的字体）



## 字体样式：
  <img width="300" height="200" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/vertical_text_fonts/imgs/test.jpg"/>

## 生成的竖排文本样例：
  <img width="256" height="32" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/vertical_text_fonts/imgs/00156360.jpg"/>
  <img width="256" height="32" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/vertical_text_fonts/imgs/00000027.jpg"/>
  <img width="256" height="32" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/vertical_text_fonts/imgs/00156365.jpg"/>
  <img width="256" height="32" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/vertical_text_fonts/imgs/00187940.jpg"/>
  


 


## web服务启动
``` Bash
cd chineseocr_lite## 进入chineseocr目录
python app.py 8080 ##8080端口号，可以设置任意端口
```

## 访问服务
http://127.0.0.1:8080/ocr


## 识别结果展示

<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/test_imgs/5_res.jpg"/>
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/test_imgs/4_res.jpg"/>
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/test_imgs/1_res.jpg"/>
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/test_imgs/2_res.jpg"/>
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/test_imgs/3_res.jpg"/>


## ncnn检测识别展示(x86 cpu 单进程)
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/ncnn_project/ocr/res_imgs/res_1.jpg"/>
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/ncnn_project/ocr/res_imgs/res_2.jpg"/>



## 参考
1. ncnn  https://github.com/Tencent/ncnn         
2. crnn  https://github.com/meijieru/crnn.pytorch.git              
3. chineseocr  https://github.com/chineseocr/chineseocr      
4. Psenet https://github.com/WenmuZhou/PSENet.pytorch  
5. 语言模型实现 https://github.com/lukhy/masr
