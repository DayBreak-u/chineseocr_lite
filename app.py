# -*- coding: utf-8 -*-
"""
@author: lywen
"""
import os
import json
import time
import web
import numpy as np
import uuid
from PIL import Image
from model import text_predict, crnn_handle

web.config.debug = True

filelock = 'file.lock'
if os.path.exists(filelock):
    os.remove(filelock)

render = web.template.render('templates', base='base')

from config import *
from apphelper.image import union_rbox, adjust_box_to_origin, base64_to_PIL
from application import trainTicket, idcard

# from main import TextOcrModel


billList = ['通用OCR', '火车票', '身份证']


class OCR:
    """通用OCR识别"""
    
    def GET(self):
        post = {}
        post['postName'] = 'ocr'  ##请求地址
        post['height'] = 1000
        post['H'] = 1000
        post['width'] = 600
        post['W'] = 600
        post['billList'] = billList
        return render.ocr(post)
    
    def POST(self):
        t = time.time()
        data = web.data()
        uidJob = uuid.uuid1().__str__()
        
        data = json.loads(data)
        # print(data)
        billModel = data.get('billModel', '')
        # textAngle = data.get('textAngle',False)##文字检测
        textLine = data.get('textLine', False)  # 只进行单行识别
        
        imgString = data['imgString'].encode().split(b';base64,')[-1]
        img = base64_to_PIL(imgString)
        if img is not None:
            img = np.array(img)
        
        H, W = img.shape[:2]
        
        while time.time() - t <= TIMEOUT:
            if os.path.exists(filelock):
                continue
            else:
                with open(filelock, 'w') as f:
                    f.write(uidJob)
                
                if textLine:
                    # 单行识别
                    partImg = Image.fromarray(img)
                    text = crnn_handle.predict(partImg)
                    res = [{'text': text, 'name': '0', 'box': [0, 0, W, 0, W, H, 0, H]}]
                    os.remove(filelock)
                    break
                
                else:
                    # detectAngle = textAngle
                    result = text_predict(img)
                    
                    if billModel == '' or billModel == '通用OCR':
                        # result = union_rbox(result,0.2)
                        res = [{'text': x['text'],
                                'name': str(i),
                                'box': {'cx': x['cx'],
                                        'cy': x['cy'],
                                        'w': x['w'],
                                        'h': x['h'],
                                        'angle': x['degree']
                            
                                        }
                                } for i, x in enumerate(result)]
                        # res = adjust_box_to_origin(img,angle, res)##修正box
                    
                    elif billModel == '火车票':
                        res = trainTicket.trainTicket(result)
                        res = res.res
                        res = [{'text': res[key], 'name': key, 'box': {}} for key in res]
                    
                    elif billModel == '身份证':
                        
                        res = idcard.idcard(result)
                        res = res.res
                        res = [{'text': res[key], 'name': key, 'box': {}} for key in res]
                    
                    os.remove(filelock)
                    break
        
        timeTake = time.time() - t
        
        return json.dumps({'res': res, 'timeTake': round(timeTake, 4)}, ensure_ascii=False)


urls = ('/ocr', 'OCR',)

if __name__ == "__main__":
    app = web.application(urls, globals())
    app.run()
