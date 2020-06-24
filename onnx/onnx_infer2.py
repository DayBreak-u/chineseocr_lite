"""
This code uses the onnx model to detect faces from live video or cameras.
"""
import os
import sys

sys.path.insert(0, "..")
from pse import decode
import time

import cv2
import numpy as np
import onnx

from utils.utils import show_img, draw_bbox

# onnx runtime
import onnxruntime

onnx_path = "psenetlite_mobilenetv2.onnx"
session = onnxruntime.InferenceSession(onnx_path)
session.get_modelmeta()
input_name = session.get_inputs()[0].name
output_name = session.get_outputs()[0].name

# img_path = '/home/yanghuiyu/datas/OCR_data/test_1000/image/TB1y4pSLXXXXXbEXXXXunYpLFXX.jpg'
img_path = './微信截图_20180801152018.png'
img_path = './2.jpg'

# confidences, boxes = predictor.run(image)
root = "../test_images/input/*.*p*g"
import glob

imgs = glob.glob(root)
for img_path in imgs:
    
    orig_image = cv2.imread(img_path)
    h, w, _ = orig_image.shape
    
    image = orig_image.copy()
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    # if h > w :
    #     image = np.concatenate((image , np.zeros([h,h-w,3])),axis=1)
    # elif w > h :
    #     image = np.concatenate((image, np.zeros([w - h,  w,3])), axis=0)
    
    print(image.shape)
    
    # cv2.imwrite("temp.jpg",image)
    
    image = cv2.resize(image, (480, 480))
    image = image / 255.0
    # image = cv2.resize(image, (640, 480))
    
    # mean = np.array([0.40789654, 0.44719302, 0.47026115],
    #                 dtype=np.float32).reshape(1, 1, 3)
    # std = np.array([0.28863828, 0.27408164, 0.27809835],
    #                dtype=np.float32).reshape(1, 1, 3)
    
    # print(image)
    
    mean = np.array([0.485, 0.456, 0.406])
    std = np.array([0.229, 0.224, 0.225])
    image = (image - mean) / std
    image = np.transpose(image, [2, 0, 1])
    image = np.expand_dims(image, axis=0)
    image = image.astype(np.float32)
    
    time_time = time.time()
    preds = session.run([output_name], {input_name: image})
    # print(preds[0][0])
    preds, boxes_list = decode(preds[0][0], 1, no_sigmode=True)
    scale = (preds.shape[1] / w, preds.shape[0] / h)
    if len(boxes_list):
        boxes_list = boxes_list / scale
    
    print(time.time() - time_time)
    img = draw_bbox(img_path, boxes_list, color=(0, 0, 255))
    cv2.imwrite(img_path.replace("input", "output"), img)
