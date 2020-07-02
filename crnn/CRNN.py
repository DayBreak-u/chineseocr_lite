import torch
from torchvision import transforms
import os
import cv2
import time
import numpy as np
from .keys import alphabetChinese as alphabet
from torch.autograd import Variable

from .util import strLabelConverter, resizeNormalize

converter = strLabelConverter(''.join(alphabet))


class CRNNHandle:
    def __init__(self, model_path, net, gpu_id=None):
        """
       初始化pytorch模型
       :param model_path: 模型地址(可以是模型的参数或者参数和计算图一起保存的文件)
       :param net: 网络计算图，如果在model_path中指定的是参数的保存路径，则需要给出网络的计算图

       :param gpu_id: 在哪一块gpu上运行
       """
        
        if gpu_id is not None and isinstance(gpu_id, int) and torch.cuda.is_available():
            self.device = torch.device("cuda:{}".format(gpu_id))
        else:
            self.device = torch.device("cpu")
        self.net = torch.load(model_path, map_location=self.device)
        print('device:', self.device)
        
        if net is not None:
            # 如果网络计算图和参数是分开保存的，就执行参数加载
            net = net.to(self.device)
            
            try:
                sk = {}
                for k in self.net:
                    sk[k.replace("module.", "")] = self.net[k]
                    # sk[k[7:]] = self.net[k]
                
                net.load_state_dict(sk)
            except Exception as e:
                print(e)
                net.load_state_dict(self.net)
            
            self.net = net
            print('load model')
        self.net.eval()
    
    def predict(self, im):
        """
        预测
        """
        image = im.convert('L')
        scale = image.size[1] * 1.0 / 32
        w = image.size[0] / scale
        w = int(w)
        transformer = resizeNormalize((w, 32))
        
        image = transformer(image)
        image = image.to(self.device)
        image = image.view(1, *image.size())
        image = Variable(image)
        preds = self.net(image)
        
        _, preds = preds.max(2)
        
        preds = preds.transpose(1, 0).contiguous().view(-1)
        preds_size = Variable(torch.IntTensor([preds.size(0)]))
        sim_pred = converter.decode(preds.data, preds_size.data, raw=False)
        return sim_pred
