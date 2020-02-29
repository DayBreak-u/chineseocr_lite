# -*- coding: utf-8 -*-
"""
参考：https://www.cnblogs.com/shouhuxianjian/p/10567201.html
"""
import os
import io
import argparse
import configparser
import numpy
import numpy as np
from keras import backend as K
from keras.layers import (Conv2D, Input, ZeroPadding2D, Add,
                          UpSampling2D, MaxPooling2D, Concatenate)
from keras.layers.advanced_activations import LeakyReLU
from keras.layers.normalization import BatchNormalization
from keras.models import Model
from keras.regularizers import l2
from keras.utils.vis_utils import plot_model as plot



def parser():
    parser = argparse.ArgumentParser(description="keras yolo3 to darknet")
    parser.add_argument('-cfg_path', help='models/text.cfg')
    parser.add_argument('-weights_path', help='models/text.h5')
    parser.add_argument('-output_path', help='models/text.weights')
    return parser.parse_args()


class DarkNetParser(object):
    def __init__(self, cfg_path, weights_path):

        self.block_gen = self._get_block(cfg_path)
        self.input_layer = Input(shape=(None, None, 3))
        self.out_index = []
        self.prev_layer = self.input_layer
        self.all_layers = []
        self.count = [0,0]
        self.weights_path = weights_path
    

    
    def _get_block(self,cfg_path):

        block = {}
        with open(cfg_path,'r', encoding='utf-8') as fr:
            for line in fr:
                line = line.strip()
                if '[' in line and ']' in line:        
                    if block:
                        yield block
                    block = {}
                    block['type'] = line.strip(' []')
                elif not line or '#' in line:
                    continue
                else:
                    key,val = line.strip().replace(' ','').split('=')
                    key,val = key.strip(), val.strip()
                    block[key] = val

            yield block


    def conv(self, block):
        '''在读取darknet的yolov3.weights文件时，顺序是
          1 - bias；
          2 - 如果有bn，则接着读取三个scale，mean，var
          3 - 读取权重
        '''
        # Darknet serializes convolutional weights as:
        # [bias/beta, [gamma, mean, variance], conv_weights]
        self.count[0] += 1
        # read conv block
        filters = int(block['filters'])
        size = int(block['size'])
        stride = int(block['stride'])
        pad = int(block['pad'])
        activation = block['activation']
        
        padding = 'same' if pad == 1 and stride == 1 else 'valid'
        batch_normalize = 'batch_normalize' in block
        
        prev_layer_shape = K.int_shape(self.prev_layer)
        weights_shape = (size, size, prev_layer_shape[-1], filters)
        darknet_w_shape = (filters, weights_shape[2], size, size)
        weights_size = np.product(weights_shape)

        print('+',self.count[0],'conv2d', 
              'bn' if batch_normalize else ' ',
              activation,
              weights_shape)

        
 
        # 如果有bn，则接着读取滤波器个scale，mean，var

        # 读取权重
        
        # DarkNet conv_weights are serialized Caffe-style:
        # (out_dim, in_dim, height, width)
        # We would like to set these to Tensorflow order:
        # (height, width, in_dim, out_dim)


        act_fn = None
        if activation == 'leaky':
            pass
        elif activation != 'linear':
            raise

        if stride > 1:
            self.prev_layer = ZeroPadding2D(((1,0),(1,0)))(self.prev_layer)

        conv_layer = (Conv2D(
                filters, (size, size),
                strides=(stride, stride),
                kernel_regularizer=l2(self.weight_decay),
                use_bias=not batch_normalize,
                             # weights=conv_weights,
                activation=act_fn,
                padding=padding))(self.prev_layer)

        if batch_normalize:
            #conv_layer = BatchNormalization(weights=bn_weight_list)(conv_layer)
             conv_layer = BatchNormalization()(conv_layer)
        self.prev_layer = conv_layer

        if activation == 'linear':
            self.all_layers.append(self.prev_layer)
        elif activation == 'leaky':
            act_layer = LeakyReLU(alpha=0.1)(self.prev_layer)
            self.prev_layer = act_layer
            self.all_layers.append(act_layer)


    def bn(self,filters,conv_bias):
        '''bn有4个参数，分别是bias，scale，mean，var，
          其中bias已经读取完毕，这里读取剩下三个，scale,mean,var '''
        bn_weights = self.weight_loader.parser_buffer(
                              shape=(3,filters),
                              dtype='float32',
                              buffer_size=(filters*3)*4)
        # scale, bias, mean,var
        bn_weight_list = [bn_weights[0],
                          conv_bias,
                          bn_weights[1],
                          bn_weights[2] ]
        return bn_weight_list
       
    def maxpool(self,block):
        size = int(block['size'])
        stride = int(block['stride'])
        maxpool_layer = MaxPooling2D(pool_size=(size,size),
                        strides=(stride,stride),
                        padding='same')(self.prev_layer)
        self.all_layers.append(maxpool_layer)
        self.prev_layer = maxpool_layer

    def shortcut(self,block):
        index = int(block['from'])
        activation = block['activation']
        assert activation == 'linear', 'Only linear activation supported.'
        shortcut_layer = Add()([self.all_layers[index],self.prev_layer])
        self.all_layers.append(shortcut_layer)
        self.prev_layer = shortcut_layer

    def route(self,block):
        layers_ids = block['layers']
        ids = [int(i) for i in layers_ids.split(',')]
        layers = [self.all_layers[i] for i in ids]
        if len(layers) > 1:
            print('Concatenating route layers:', layers)
            concatenate_layer = Concatenate()(layers)
            self.all_layers.append(concatenate_layer)
            self.prev_layer = concatenate_layer
        else:
            skip_layer = layers[0]
            self.all_layers.append(skip_layer)
            self.prev_layer = skip_layer

    def upsample(self,block):
        stride = int(block['stride'])
        assert stride == 2, 'Only stride=2 supported.'
        upsample_layer = UpSampling2D(stride)(self.prev_layer)
        self.all_layers.append(upsample_layer)
        self.prev_layer = self.all_layers[-1]
    
    def yolo(self,block):
        self.out_index.append(len(self.all_layers)-1)
        self.all_layers.append(None)
        self.prev_layer = self.all_layers[-1]

    def net(self, block):
        self.weight_decay = block['decay']


    def create_and_save(self):
        if len(self.out_index) == 0:
            self.out_index.append( len(self.all_layers)-1 )

        output_layers = [self.all_layers[i] for i in self.out_index]
        model = Model(inputs=self.input_layer,
                      outputs=output_layers)
        self.model = model
        self.model.load_weights(self.weights_path)




class WeightSaver(object):

    def __init__(self,textModel,output_path):
        self.model = textModel
        self.sess = K.get_session()
        #self.sess.run(K.tf.global_variables_initializer())
        self.layers = {weight.name:weight for weight in self.model.weights}
        self.fhandle = open(output_path,'wb')
        self._write_head()

    def _write_head(self):
        numpy_data = numpy.ndarray(shape=(3,),
                          dtype='int32',
                          buffer=np.array([0,2,0],dtype='int32') )
        self.save(numpy_data)
        numpy_data = numpy.ndarray(shape=(1,),
                          dtype='int64',
                          buffer=np.array([320000],dtype='int64'))
        self.save(numpy_data)
 
    def get_bn_layername(self,num):
        layer_name = 'batch_normalization_{num}'.format(num=num)
        bias = self.layers['{0}/beta:0'.format(layer_name)]
        scale = self.layers['{0}/gamma:0'.format(layer_name)]
        mean = self.layers['{0}/moving_mean:0'.format(layer_name)]
        var = self.layers['{0}/moving_variance:0'.format(layer_name)]
       
        bias_np = self.get_numpy(bias)
        scale_np = self.get_numpy(scale)
        mean_np = self.get_numpy(mean)
        var_np = self.get_numpy(var)
        return bias_np,scale_np,mean_np,var_np

    def get_convbias_layername(self,num):
        layer_name = 'conv2d_{num}'.format(num=num)
        bias = self.layers['{0}/bias:0'.format(layer_name)]
      
        bias_np = self.get_numpy(bias)
        return bias_np
 
    def get_conv_layername(self,num):
        layer_name = 'conv2d_{num}'.format(num=num)
        conv = self.layers['{0}/kernel:0'.format(layer_name)]
       
        conv_np = self.get_numpy(conv)
        return conv_np

  
    def get_numpy(self,layer_name):
        numpy_data = self.sess.run(layer_name)
        return numpy_data

    def save(self,numpy_data):
        bytes_data = numpy_data.tobytes()
        self.fhandle.write(bytes_data)
        self.fhandle.flush()

    def close(self):
        self.fhandle.close()

class KerasParser(object):

    def __init__(self, cfg_path, h5_path, output_path):
        self.block_gen = self._get_block(cfg_path)
        self.weights_saver = WeightSaver(h5_path, output_path)
        self.count_conv = 0
        self.count_bn = 0

    def _get_block(self,cfg_path):

        block = {}
        with open(cfg_path,'r', encoding='utf-8') as fr:
            for line in fr:
                line = line.strip()
                if '[' in line and ']' in line:
                    if block:
                        yield block
                    block = {}
                    block['type'] = line.strip(' []')
                elif not line or '#' in line:
                    continue
                else:
                    key,val = line.strip().replace(' ','').split('=')
                    key,val = key.strip(), val.strip()
                    block[key] = val

            yield block

    def close(self):
        self.weights_saver.close()

    def conv(self, block):
        self.count_conv += 1
        batch_normalize = 'batch_normalize' in block
        print('handing.. ',self.count_conv)

        # 如果bn存在，则先处理bn，顺序为bias，scale，mean，var
        if batch_normalize:
            bias,scale,mean,var = self.bn()
            self.weights_saver.save(bias)
            
            scale = scale.reshape(1,-1)
            mean = mean.reshape(1,-1)
            var = var.reshape(1,-1)
            remain = np.concatenate([scale,mean,var],axis=0)
            self.weights_saver.save(remain)

        # 否则，先处理biase
        else:
            conv_bias = self.weights_saver.get_convbias_layername(self.count_conv)
            self.weights_saver.save(conv_bias)

        # 接着处理weights
        conv_weights = self.weights_saver.get_conv_layername(self.count_conv)
        # 需要将(height, width, in_dim, out_dim)转换成(out_dim, in_dim, height, width)
        conv_weights = np.transpose(conv_weights,[3,2,0,1])
        self.weights_saver.save(conv_weights)

    def bn(self):
        self.count_bn += 1
        bias,scale,mean,var = self.weights_saver.get_bn_layername(self.count_bn) 
        return bias,scale,mean,var


    
        

def main():

    args = parser()
    print('loading weights...')

    darknet_parser = DarkNetParser(args.cfg_path,args.weights_path)

    print('creating keras model...')

    layers_fun = {'convolutional':darknet_parser.conv,
                 'net':darknet_parser.net,
                 'yolo':darknet_parser.yolo,
                 'route':darknet_parser.route,
                 'upsample':darknet_parser.upsample,
                 'maxpool':darknet_parser.maxpool,
                 'shortcut':darknet_parser.shortcut
                 }

    print('Parsing Darknet config.')
    for ind,block in enumerate(darknet_parser.block_gen):
        type = block['type']
        layers_fun[type](block)

    darknet_parser.create_and_save()   
    
    keras_loader = KerasParser(args.cfg_path,darknet_parser.model, args.output_path)
    for block in keras_loader.block_gen:
        if 'convolutional' in block['type']:
            keras_loader.conv(block)
    keras_loader.close()
    


if __name__ == '__main__':
    
    main()
