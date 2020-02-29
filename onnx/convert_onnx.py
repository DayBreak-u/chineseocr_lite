import  sys
import  mxnet as mx
sys.path.insert(0,"..")
from crnn import LiteCrnn
from utils0nnx import  load_model
import  torch
from crnn.keys import alphabetChinese as alphabet
nh = 256
net   =LiteCrnn(32, 1, len(alphabet) + 1, nh, n_rnn=2, leakyRelu=False, lstmFlag=True)
net = load_model(net, '../models/crnn_lite_lstm_dw.pth')
net.eval()

print('Finished loading model!')
print(net)
device = torch.device("cpu")
net = net.to(device)

##################export###############
output_onnx = 'crnn_lite_lstm_dw.onnx'
print("==> Exporting model to ONNX format at '{}'".format(output_onnx))
input_names = ["input"]
# output_names = ["hm" , "wh"  , "reg"]
output_names = ["out" ]
inputs = torch.randn(1, 1, 32, 640).to(device)
torch_out = torch.onnx._export(net, inputs, output_onnx, export_params=True, verbose=False,
                               input_names=input_names, output_names=output_names)


import  os
os.system("python -m onnxsim {0} {0}".format(output_onnx))