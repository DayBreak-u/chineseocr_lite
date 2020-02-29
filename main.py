#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Aug  4 01:01:37 2019
main 
@author: chineseocr
"""
from text.detector.detectors import TextDetector
from apphelper.image import rotate_cut_img,sort_box
import numpy as np
from PIL import Image

class TextOcrModel(object):
    def __init__(self,ocrModel,textModel,angleModel):
        self.ocrModel = ocrModel
        self.textModel = textModel
        self.angleModel = angleModel
    
    def detect_angle(self,img):
        """
        detect text angle in [0,90,180,270]
        @@img:np.array
        """
        angle = self.angleModel(img)
        if angle==90:
            im = Image.fromarray(img).transpose(Image.ROTATE_90)
            img = np.array(im)
        elif angle==180:
            im = Image.fromarray(img).transpose(Image.ROTATE_180)
            img = np.array(im)
        elif angle==270:
            im = Image.fromarray(img).transpose(Image.ROTATE_270)
            img = np.array(im)
        
        return img,angle
    
    def detect_box(self,img,scale=600,maxScale=900):
        """
        detect text angle in [0,90,180,270]
        @@img:np.array
        """
        boxes,scores = self.textModel(img,scale,maxScale)
        return boxes,scores
    
    def box_cluster(self,img,boxes,scores,**args):
        
        MAX_HORIZONTAL_GAP= args.get('MAX_HORIZONTAL_GAP',100)
        MIN_V_OVERLAPS    = args.get('MIN_V_OVERLAPS',0.6)
        MIN_SIZE_SIM      = args.get('MIN_SIZE_SIM',0.6)
        textdetector = TextDetector(MAX_HORIZONTAL_GAP,MIN_V_OVERLAPS,MIN_SIZE_SIM)
        
        shape = img.shape[:2]
        TEXT_PROPOSALS_MIN_SCORE     = args.get('TEXT_PROPOSALS_MIN_SCORE',0.7)
        TEXT_PROPOSALS_NMS_THRESH    = args.get('TEXT_PROPOSALS_NMS_THRESH',0.3)
        TEXT_LINE_NMS_THRESH         = args.get('TEXT_LINE_NMS_THRESH',0.3)
        LINE_MIN_SCORE               = args.get('LINE_MIN_SCORE',0.8)
        
        boxes,scores = textdetector.detect(boxes,
                                scores[:, np.newaxis],
                                shape,
                                TEXT_PROPOSALS_MIN_SCORE,
                                TEXT_PROPOSALS_NMS_THRESH,
                                TEXT_LINE_NMS_THRESH,
                                LINE_MIN_SCORE
                                )
        return boxes,scores
    
    
    def ocr_batch(self,img,boxes,leftAdjustAlph=0.0,rightAdjustAlph=0.0):
        """
        batch for ocr
        """
        im = Image.fromarray(img)
        newBoxes = []
        for index,box in enumerate(boxes):
            partImg,box = rotate_cut_img(im,box,leftAdjustAlph,rightAdjustAlph)
            box['img'] = partImg.convert('L')
            newBoxes.append(box)
            
        res = self.ocrModel(newBoxes)
        return res
    
    
    def model(self,img,**args):
        detectAngle        = args.get('detectAngle',False)
        if detectAngle:
            img,angle      = self.detect_angle(img)
        else:
            angle          = 0
        scale              = args.get('scale',608)
        maxScale           = args.get('maxScale',608)
        boxes,scores       = self.detect_box(img,scale,maxScale)##文字检测
        boxes,scores       = self.box_cluster(img,boxes,scores,**args)
        boxes              = sort_box(boxes)
        leftAdjustAlph     = args.get('leftAdjustAlph',0)
        rightAdjustAlph    = args.get('rightAdjustAlph',0)
       
        res                = self.ocr_batch(img,boxes,leftAdjustAlph,rightAdjustAlph)
        return res,angle
        
    
