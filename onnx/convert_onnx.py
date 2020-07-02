import sys
import mxnet as mx

sys.path.insert(0, "..")
from crnn import LiteCrnn
from onnx.utils0nnx import load_model
import torch
from angle_class import shufflenet_v2_x0_5
from crnn.keys import alphabetChinese as alphabet

nh = 256
net = shufflenet_v2_x0_5(num_classes=4, pretrained=False)

net = load_model(net, '../models/shufflenetv2_05.pth')
net.eval()

print('Finished loading model!')
print(net)
device = torch.device("cpu")
net = net.to(device)

##################export###############
output_onnx = 'shufflenetv2_05.onnx'
print("==> Exporting model to ONNX format at '{}'".format(output_onnx))
input_names = ["input"]
# output_names = ["hm" , "wh"  , "reg"]
output_names = ["out"]
inputs = torch.randn(1, 3, 48, 196).to(device)
torch_out = torch.onnx._export(net, inputs, output_onnx, export_params=True, verbose=False,
                               input_names=input_names, output_names=output_names)

import os

os.system("python -m onnxsim {0} {0}".format(output_onnx))
