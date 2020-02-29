"""
转换pytorch版本OCR到keras 
暂时只支持dense ocr ，lstm层不支持
"""
import os
import io
import argparse
import configparser
import numpy as np

def parser():
    parser = argparse.ArgumentParser(description="pytorch dense ocr to keras ocr")
    parser.add_argument('-weights_path',help='models/ocr-dense.pth')
    parser.add_argument('-output_path', help='models/ocr-dense-keras.h5')
    parser.add_argument('-lstm', default=False,
                        action='store_true', help='translate lstm layer')
    return parser.parse_args()

def set_cnn_weight(name,keramodel,torchmodelDict):
    """
    将torch  模型CNN层导入 keras模型CNN层
    """
    weight = None
    bias   = None 
    
    for key in torchmodelDict:
        if name in key and 'weight' in key:
            weight = torchmodelDict[key].numpy()
        if name in key and 'bias' in key: 
            bias = torchmodelDict[key].numpy()
    if weight is not None and bias is not None:
        weight = weight.transpose(2, 3, 1, 0)
        keramodel.get_layer(name).set_weights([weight,bias])
    
    
def set_bn_weight(name,keramodel,torchmodelDict):
    """
    将torch  模型BN层导入 keras模型BN层
    Keras的BN层参数顺序应该是[gamma, beta, mean, std]
    """
    gamma, beta, mean, std = None,None,None,None
    
    for key in torchmodelDict:
        if name in key and 'weight' in key:
            gamma = torchmodelDict[key].numpy()
        if name in key and 'bias' in key: 
            beta = torchmodelDict[key].numpy()
            
        if name in key and 'running_mean' in key: 
            mean = torchmodelDict[key].numpy()
            
        if name in key and 'running_var' in key: 
            std = torchmodelDict[key].numpy()
            
    keramodel.get_layer(name).set_weights([gamma, beta, mean, std])
    
def set_dense_weight(name,keramodel,torchmodelDict):
    """
    将torch  模型linear层导入 keras模型dense层
    """
    weight = None
    bias   = None 
    
    for key in torchmodelDict:
        if name in key and 'weight' in key:
            weight = torchmodelDict[key].numpy()
        if name in key and 'bias' in key: 
            bias = torchmodelDict[key].numpy()
            
    if weight is not None and bias is not None:
        weight = np.transpose(weight)
        keramodel.get_layer(name).set_weights([weight,bias])

def set_lstm_weight(name, kerasmodel, torchmodelDict):
    # RNN
    weight_ih_l0 = None
    weight_hh_l0 = None
    bias_ih_l0 = None
    bias_hh_l0 = None
    weight_ih_l0_reverse = None
    weight_hh_l0_reverse = None
    bias_ih_l0_reverse = None
    bias_hh_l0_reverse = None
    # TimeDistributed
    embedding_weight = None
    embedding_bias = None

    for key in torchmodelDict:
        if name in key:
            if key.endswith('rnn.weight_ih_l0'):
                weight_ih_l0 = torchmodelDict[key]
            elif key.endswith('rnn.weight_hh_l0'):
                weight_hh_l0 = torchmodelDict[key]
            elif key.endswith('rnn.bias_ih_l0'):
                bias_ih_l0 = torchmodelDict[key]
            elif key.endswith('rnn.bias_hh_l0'):
                bias_hh_l0 = torchmodelDict[key]
            elif key.endswith('rnn.weight_ih_l0_reverse'):
                weight_ih_l0_reverse = torchmodelDict[key]
            elif key.endswith('rnn.weight_hh_l0_reverse'):
                weight_hh_l0_reverse = torchmodelDict[key]
            elif key.endswith('rnn.bias_ih_l0_reverse'):
                bias_ih_l0_reverse = torchmodelDict[key]
            elif key.endswith('rnn.bias_hh_l0_reverse'):
                bias_hh_l0_reverse = torchmodelDict[key]
            elif key.endswith('embedding.weight'):
                embedding_weight = torchmodelDict[key]
            elif key.endswith('embedding.bias'):
                embedding_bias = torchmodelDict[key]
    rnn_weights = [
        weight_ih_l0.transpose(1, 0),
        weight_hh_l0.transpose(1, 0),
        (bias_ih_l0 + bias_hh_l0),
        weight_ih_l0_reverse.transpose(1, 0),
        weight_hh_l0_reverse.transpose(1, 0),
        (bias_ih_l0_reverse + bias_hh_l0_reverse)
    ]
    linear_weights = [
        embedding_weight.transpose(1, 0).numpy(),
        embedding_bias.numpy(),
    ]
    if name == 'rnn.0':
        kerasmodel.get_layer('bidirectional_1').set_weights(rnn_weights)
        kerasmodel.get_layer('time_distributed_1').set_weights(linear_weights)
    else:
        kerasmodel.get_layer('bidirectional_2').set_weights(rnn_weights)
        kerasmodel.get_layer('time_distributed_2').set_weights(linear_weights)

if __name__=='__main__':
    import os
    import sys
    args = parser()
    GPUID=''
    os.environ["CUDA_VISIBLE_DEVICES"] = GPUID##不调用GPU
    sys.path.append('..')
    sys.path.append('')
    import torch
    from collections import OrderedDict
    from crnn.keys import alphabetChinese
    from crnn.network_keras import keras_crnn
    ##ocrModel='models/ocr-dense.pth' #dense ocr
    ##ocrModel='models/ocr-lstm.pth'  #lstm ocr
    ocrModel = args.weights_path##torch模型权重
    output_path =args.output_path##keras 模型权重输出
    kerasModel = keras_crnn(32, 1, len(alphabetChinese)+1, 256, 1,lstmFlag=args.lstm)
    
    state_dict = torch.load(ocrModel,map_location=lambda storage, loc: storage)
    new_state_dict = OrderedDict()
    for k, v in state_dict.items():
            name = k.replace('module.','') # remove `module.`
            new_state_dict[name] = v
            
    ##模型转换
    cnn = ['cnn.conv0','cnn.conv1','cnn.conv2','cnn.conv3','cnn.conv4','cnn.conv5','cnn.conv6']
    BN =['cnn.batchnorm2','cnn.batchnorm4','cnn.batchnorm6']
    linear = ['linear']
    lstm = ['rnn.0', 'rnn.1']
    ##CNN 层
    for cn in cnn:
        set_cnn_weight(cn,kerasModel,new_state_dict)  
    ##BN 层
    for bn in BN:
        set_bn_weight(bn,kerasModel,new_state_dict)  
    if args.lstm:
        for l in lstm:
            set_lstm_weight(l,kerasModel,new_state_dict)
    else:
        ## linear 层
        for lr in linear:
            set_dense_weight(lr,kerasModel,new_state_dict) 
        
    kerasModel.save_weights(output_path)##保存keras权重
