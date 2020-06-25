## 本项目基于[chineseocr](https://github.com/chineseocr/chineseocr) 与[psenet](https://github.com/WenmuZhou/PSENet.pytorch)  实现中文自然场景文字检测及识别

# 环境
- pytorch  1.2.0 

- python3

- linux/macos/windows
---
- windows环境配置参考热心网友的文章[Python构建快速高效的中文文字识别OCR](https://blog.csdn.net/lly1122334/article/details/104752851) 👍
- windows环境运行需要手动编译psenet/pse目录下的pse.cpp为DLL，为了方便新增支持python3.5和python3.6的pyd文件，在windows平台运行
的时候需要根据自己环境的python版本更改对应的pyd文件为pse.pyd即可

- Docker 环境

  1. 可以直接在项目根目录下面运行`docker build -t my/chineseocr .` 构建运行环境的镜像，也可以

     使用其他已经构建好的镜像`docker run -dit -p 8080:8080 -v /mnt/d/data/:/data --name chineseocr vitzy/chineseocr_lite`。

  2. 可通过`docker attach --sig-proxy=false <container id>` 或者`docker exec -it <your container name or id> /bin/bash`进入容器，然后`git clone https://github.com/ouyanghuiyu/chineseocr_lite`拉取本项目代码到`/data`
  3. cd 到`chineseocr_lite`下进行安装：`pip3 install -i https://pypi.tuna.tsinghua.edu.cn/simple -r requirements.txt`
  4. 启动 web `python3 app.py 8080`， 在浏览器中打开` http://127.0.0.1:8080/ocr`。
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
- [x]  ncnn 实现 (支持lstm) nihui大佬实现的[crnn_lstm推理](https://github.com/ouyanghuiyu/chineseocr_lite/pull/41) 具体操作详解: [详细记录超轻量中文OCR LSTM模型ncnn实现](https://zhuanlan.zhihu.com/p/113338890?utm_source=qq&utm_medium=social&utm_oi=645149500650557440)
- [x]  提供竖排文字样例以及字体库（旋转90度的字体）
- [ ]  mnn  实现 


# 2020.03.16更新
- psenet ncnn核扩展实现，有效解决粘连文本检测问题，详见[ncnn ocr一条龙](https://github.com/ouyanghuiyu/chineseocr_lite/tree/master/ncnn_project/ocr)
- nihui大佬实现的[crnn_lstm推理](https://github.com/ouyanghuiyu/chineseocr_lite/pull/41) 具体操作详解: [详细记录超轻量中文OCR LSTM模型ncnn实现](https://zhuanlan.zhihu.com/p/113338890?utm_source=qq&utm_medium=social&utm_oi=645149500650557440)

# 2020.03.12更新
- 升级crnn_lite_lstm_dw.pth模型crnn_lite_lstm_dw_v2.pth , 精度更高



## 竖排字体样式：
  <img width="300" height="200" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/vertical_text_fonts/imgs/test.jpg"/>

## 竖排生成的竖排文本样例：
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


## Flask-RESTful API demo
`cd chineseocr_lite && python flask_app.py`
```text
- 请求url: http://ip:port/api/v1/ocr
- 请求方式：POST
- 请求参数
    - imgString：图片转base64后的字符串， str
- 返回实例
{
    "code": "SUCCESS",
    "text": "不配图我总觉得不舒服不完整不专业"
}
```
## 识别结果展示

<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/test_imgs/5_res.jpg"/>
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/test_imgs/4_res.jpg"/>
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/test_imgs/1_res.jpg"/>
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/test_imgs/2_res.jpg"/>
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/test_imgs/3_res.jpg"/>


## ncnn检测识别展示(x86 cpu 单进程)
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/ncnn_project/ocr/res_imgs/res_3.jpg"/>
<img width="500" height="300" src="https://github.com/ouyanghuiyu/chineseocr_lite/blob/master/ncnn_project/ocr/res_imgs/res_2.jpg"/>



## 参考
1. ncnn  https://github.com/Tencent/ncnn         
2. crnn  https://github.com/meijieru/crnn.pytorch.git              
3. chineseocr  https://github.com/chineseocr/chineseocr      
4. Psenet https://github.com/WenmuZhou/PSENet.pytorch  
5. 语言模型实现 https://github.com/lukhy/masr
