from config import *
from crnn import FullCrnn, LiteCrnn, CRNNHandle
from psenet import PSENet, PSENetHandel
from angle_class import AangleClassHandle, shufflenet_v2_x0_5
from utils import draw_bbox, crop_rect, sorted_boxes, get_rotate_crop_image
from PIL import Image
import numpy as np
import cv2
import copy
from dbnet.dbnet_infer import DBNET

if det_model_type == "pse_mobilenetv2":
    text_detect_net = PSENet(backbone=det_model_type.split("_")[-1], pretrained=False, result_num=6, scale=pse_scale)
    text_handle = PSENetHandel(model_path, text_detect_net, pse_scale, gpu_id=GPU_ID)
elif det_model_type == "dbnet":
    text_handle = DBNET(model_path, short_size=dbnet_short_size)

crnn_net = None

if crnn_type == "full_lstm" or crnn_type == "full_dense":
    crnn_net = FullCrnn(32, 1, len(alphabet) + 1, nh, n_rnn=2, leakyRelu=False, lstmFlag=LSTMFLAG)
elif crnn_type == "lite_lstm" or crnn_type == "lite_dense":
    crnn_net = LiteCrnn(32, 1, len(alphabet) + 1, nh, n_rnn=2, leakyRelu=False, lstmFlag=LSTMFLAG)

assert crnn_type is not None
crnn_handle = CRNNHandle(crnn_model_path, crnn_net, gpu_id=GPU_ID)

crnn_vertical_handle = None
if crnn_vertical_model_path is not None:
    crnn_vertical_net = LiteCrnn(32, 1, len(alphabet) + 1, nh, n_rnn=2, leakyRelu=False, lstmFlag=True)
    crnn_vertical_handle = CRNNHandle(crnn_vertical_model_path, crnn_vertical_net, gpu_id=GPU_ID)

assert angle_type in ["shufflenetv2_05"]

if angle_type == "shufflenetv2_05":
    angle_net = shufflenet_v2_x0_5(num_classes=len(lable_map_dict), pretrained=False)

angle_handle = AangleClassHandle(angle_model_path, angle_net, gpu_id=GPU_ID)


def crnnRec(im, rects_re, leftAdjust=False, rightAdjust=False, alph=0.2, f=1.0):
    """
    crnn模型，ocr识别
    @@model,
    @@converter,
    @@im:Array
    @@text_recs:text box
    @@ifIm:是否输出box对应的img

    """
    results = []
    im = Image.fromarray(im)
    for index, rect in enumerate(rects_re):
        
        degree, w, h, cx, cy = rect
        
        # partImg, newW, newH = rotate_cut_img(im,  90  + degree  , cx, cy, w, h, leftAdjust, rightAdjust, alph)
        partImg = crop_rect(im, ((cx, cy), (h, w), degree))
        newW, newH = partImg.size
        partImg_array = np.uint8(partImg)
        
        #
        if newH > 1.5 * newW:
            partImg_array = np.rot90(partImg_array, 1)
        
        # partImg = Image.fromarray(partImg_array).convert("RGB")
        
        # partImg.save("./debug_im/{}.jpg".format(index))
        
        angel_index = angle_handle.predict(partImg_array)
        
        angel_class = lable_map_dict[angel_index]
        # print(angel_class)
        rotate_angle = rotae_map_dict[angel_class]
        
        if rotate_angle != 0:
            partImg_array = np.rot90(partImg_array, rotate_angle // 90)
        
        partImg = Image.fromarray(partImg_array).convert("RGB")
        #
        # partImg.save("./debug_im/{}.jpg".format(index))
        
        partImg_ = partImg.convert('L')
        
        try:
            
            if crnn_vertical_handle is not None and angel_class in ["shudao", "shuzhen"]:
                
                simPred = crnn_vertical_handle.predict(partImg_)
            else:
                simPred = crnn_handle.predict(partImg_)  ##识别的文本
        except:
            continue
        
        if simPred.strip() != u'':
            results.append({'cx': cx * f, 'cy': cy * f, 'text': simPred, 'w': newW * f, 'h': newH * f,
                            'degree': degree})
    return results


def crnnRecWithBox(im, boxes_list):
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
    for index, box in enumerate(boxes_list):
        # rect = cv2.minAreaRect(box)
        # degree, w, h, cx, cy = rect
        # box = sorted_boxes(box)
        tmp_box = copy.deepcopy(box)
        partImg_array = get_rotate_crop_image(im, tmp_box.astype(np.float32))
        
        # partImg = Image.fromarray(partImg_array).convert("RGB")
        
        # partImg.save("./debug_im/{}.jpg".format(index))
        
        angel_index = angle_handle.predict(partImg_array)
        
        angel_class = lable_map_dict[angel_index]
        # print(angel_class)
        rotate_angle = rotae_map_dict[angel_class]
        
        if rotate_angle != 0:
            partImg_array = np.rot90(partImg_array, rotate_angle // 90)
        
        partImg = Image.fromarray(partImg_array).convert("RGB")
        #
        # partImg.save("./debug_im/{}.jpg".format(index))
        
        partImg_ = partImg.convert('L')
        newW, newH = partImg.size
        try:
            
            if crnn_vertical_handle is not None and angel_class in ["shudao", "shuzhen"]:
                
                simPred = crnn_vertical_handle.predict(partImg_)
            else:
                simPred = crnn_handle.predict(partImg_)  ##识别的文本
        except:
            continue
        
        if simPred.strip() != u'':
            results.append({'cx': 0, 'cy': 0, 'text': simPred, 'w': newW, 'h': newH,
                            'degree': 0})
            # results.append({ 'text': simPred, })
    return results


def text_predict(img):
    # img = cv2.imread(imgpath)
    if det_model_type == "pse_mobilenetv2":
        preds, boxes_list, rects_re, t = text_handle.predict(img, long_size=pse_long_size)
    else:
        boxes_list, score_list = text_handle.process(img)
    
    # img2 = draw_bbox(img, boxes_list, color=(0, 255, 0))
    # cv2.imwrite("debug_im/draw.jpg", img2)
    
    # result = crnnRec(np.array(img), rects_re)
    result = crnnRecWithBox(np.array(img), boxes_list)
    
    return result


if __name__ == "__main__":
    pass
