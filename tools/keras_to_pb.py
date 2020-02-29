#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Sep  4 00:00:43 2019
keras to pd for opencv dnn 
@author: chineseocr
"""


import tensorflow as tf
import os 
from keras import backend as K
from tensorflow.python.framework import graph_util,graph_io
def keras_to_pb(kerasmodel,outputDir,modelName='model.pd',outName = "output_"):
    if not os.path.exists(outputDir):
        os.makedirs(outputDir)
    out_nodes = []
    for i in range(len(kerasmodel.outputs)):
        out_nodes.append(outName + str(i + 1))
        tf.identity(kerasmodel.outputs[i],outName + str(i + 1))
    sess = K.get_session()
    init_graph = sess.graph.as_graph_def()
    main_graph = graph_util.convert_variables_to_constants(sess,init_graph,out_nodes)
    graph_io.write_graph(main_graph,outputDir,name = modelName,as_text = False)
    
    
def pd_to_pbtxt(pdPath):
    with tf.gfile.FastGFile(pdPath,'rb') as f:
       graph_def = tf.GraphDef()
       graph_def.ParseFromString(f.read())
       
    for i in reversed(range(len(graph_def.node))):
        if graph_def.node[i].op == 'Const':
            del graph_def.node[i]
        for attr in ['T', 'data_format', 'Tshape', 'N', 'Tidx', 'Tdim',
                     'use_cudnn_on_gpu', 'Index', 'Tperm', 'is_training',
                     'Tpaddings']:
            if attr in graph_def.node[i].attr:
                del graph_def.node[i].attr[attr]
                
    path,filename = os.path.split(pdPath)
    filename = filename.replace('.pb','.pbtxt')
    tf.train.write_graph(graph_def,path, filename, as_text=True) 
    

def remove_node(txt,name):
    index = txt.find('node {\n  '+ name )
    ind = index
    punc = []
    flag=False
    if index>=0:
        for i in range(ind,len(txt)):
                if txt[i]=='{':
                    punc.append('{')
                elif txt[i]=='}':
                        if '{' in punc:
                            punc.pop(-1)
                        if len(punc)==0:
                            flag=True
                            break
                        
                            
                   
    if flag:
        txt=txt.replace(txt[index:i+1],'')
        
    return txt
                   
    
    
if __name__=='__main__':
    ## demo vgg16 to pb 
    
    """
    Open model.pbtxt and remove nodes with names strided_slice,flatten/Shape, flatten/strided_slice, flatten/Prod, flatten/stack.
    Replace the node
    node {
      name: "flatten/Reshape"
      op: "Reshape"
      input: "block5_pool/MaxPool"
      input: "flatten/stack"
    }
    
    on
    
    node {
      name: "flatten/Reshape"
      op: "Flatten"
      input: "block5_pool/MaxPool"
    }
    
    """
    def pbtxt_adjust(pbtxt):
        with open(pbtxt) as f:
            txt = f.read()
            
        nodename = 'node {\n  name: "flatten/Reshape"'
        ## replace 
        index = txt.find(nodename)
        if index>0:
            for ind in range(index,len(txt)):
                if txt[ind]=='}':
                   break
            replacestr = txt[index:ind+1]
            txt=txt.replace(replacestr,replacestr.replace('op: "Reshape"','op: "Flatten"'))
             
        
        ## del node 
        delnamelist = [ 'name: "flatten/Shape"',
                       'name: "flatten/strided_slice"',
                       'name: "flatten/Prod"',
                       'name: "flatten/stack"'
                       ]
        
        for delname in delnamelist:
            txt = remove_node(txt,delname)
           
        with open(pbtxt,'w') as f:
            f.write(txt)
    
    
    from keras.applications.vgg16 import VGG16
    import cv2
    import numpy as np
    vgg = VGG16(weights=None)
    name = vgg.name
    modelName = name+'.pb'
    outputDir=os.path.join('/tmp/',name)
    keras_to_pb(vgg,outputDir,modelName=modelName,outName = "output_")
    pb = os.path.join(outputDir,modelName)
    pd_to_pbtxt(pb)
    pbtxt = os.path.join(outputDir,name+'.pbtxt')
    pbtxt_adjust(pbtxt)
    
    dnn = cv2.dnn.readNetFromTensorflow(pb,pbtxt)
    inputBlob = np.zeros((1,3,224,224))
    dnn.setInput(inputBlob)
    pred = dnn.forward()
    print('dnn:',pred[0][:10])
    print('vgg:',vgg.predict(np.zeros((1,224,224,3)))[0][:10])
    
    
    