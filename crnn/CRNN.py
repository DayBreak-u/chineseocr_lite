from PIL import  Image
import numpy as np
import cv2
from .keys import alphabetChinese as alphabet
# from keys import alphabetChinese as alphabet

import onnxruntime as rt
# from util import strLabelConverter, resizeNormalize
from .util import strLabelConverter, resizeNormalize

converter = strLabelConverter(''.join(alphabet))


class CRNNHandle:
    def __init__(self, model_path):

        self.sess = rt.InferenceSession(model_path)

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

        image = image.transpose(2, 0, 1)
        transformed_image = np.expand_dims(image, axis=0)

        preds = self.sess.run(["out"], {"input": transformed_image.astype(np.float32)})

        preds = preds[0]


        length  = preds.shape[0]
        preds = preds.reshape(length,-1)
        # preds = 1/(1+np.exp(-1 * preds))

        # temp_fist = preds[:,0]
        # temp_fist[temp_fist>0.001] = 0.001
        #
        # preds[:,0 ] =temp_fist


        preds = np.argmax(preds,axis=1)

        preds = preds.reshape(-1)

        sim_pred = converter.decode(preds, length, raw=False)

        return sim_pred


if __name__ == "__main__":
    im = Image.open("471594277244_.pic.jpg")
    crnn_handle = CRNNHandle(model_path="../models/crnn_lite_lstm.onnx")
    print(crnn_handle.predict(im))