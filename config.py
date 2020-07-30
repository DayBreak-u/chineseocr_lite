import os

filt_path = os.path.abspath(__file__)
father_path = os.path.abspath(os.path.dirname(filt_path) + os.path.sep + ".")


dbnet_short_size = 960
crnn_lite = True

model_path = os.path.join(father_path, "models/dbnet.onnx")

# crnn相关
crnn_model_path = os.path.join(father_path, "models/crnn_lstm.onnx")
if crnn_lite:
    crnn_model_path = os.path.join(father_path, "models/crnn_lite_lstm.onnx")


from crnn.keys import alphabetChinese as alphabet

TIMEOUT = 30

version = 'api/v1'
