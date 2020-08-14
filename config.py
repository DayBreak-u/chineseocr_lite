import os



filt_path = os.path.abspath(__file__)
father_path = os.path.abspath(os.path.dirname(filt_path) + os.path.sep + ".")


dbnet_max_size = 6000
crnn_lite = True
# 每个ip每天的最大访问次数
max_post_time = 20
model_path = os.path.join(father_path, "models/dbnet.onnx")

# crnn相关
crnn_model_path = os.path.join(father_path, "models/crnn_lstm.onnx")
if_rgb = False
if crnn_lite:
    crnn_model_path = os.path.join(father_path, "models/crnn_lite_lstm.onnx")
    is_rgb = True


from crnn.keys import alphabetChinese as alphabet

TIMEOUT = 30
#IP 白名单
white_ips = []

version = 'api/v1'
