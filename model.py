from config import *
from crnn import CRNNHandle

from utils import draw_bbox, crop_rect, sorted_boxes, get_rotate_crop_image
from PIL import Image
import numpy as np
import cv2
import copy
from dbnet.dbnet_infer import DBNET


text_handle = DBNET(model_path, short_size=dbnet_short_size)


crnn_handle = CRNNHandle(crnn_model_path)


def crnnRecWithBox(im, boxes_list,score_list):
    """
    crnn模型，ocr识别
    @@model,
    @@converter,
    @@im:Array
    @@text_recs:text box
    @@ifIm:是否输出box对应的img

    """
    results = []
    boxes_list = sorted_boxes(np.array(boxes_list))
    for index, (box ,score) in enumerate(zip(boxes_list,score_list)):

        tmp_box = copy.deepcopy(box)
        partImg_array = get_rotate_crop_image(im, tmp_box.astype(np.float32))
        

        partImg = Image.fromarray(partImg_array).convert("RGB")


        partImg_ = partImg.convert('L')

        try:

            simPred = crnn_handle.predict(partImg_)  ##识别的文本
        except:
            continue
        
        if simPred.strip() != u'':
            results.append([tmp_box,simPred,score])

    return results


def text_predict(img):
    boxes_list, score_list = text_handle.process(np.asarray(img).astype(np.uint8))
    result = crnnRecWithBox(np.array(img), boxes_list,score_list)
    
    return result


if __name__ == "__main__":
    pass
