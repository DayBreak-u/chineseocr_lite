from PIL import  Image
import numpy as np
import cv2

import onnxruntime as rt
import time

class AngleNetHandle:
    def __init__(self, model_path,size_h = 32, size_w = 192):

        self.sess = rt.InferenceSession(model_path)
        self.size_h = size_h
        self.size_w = size_w

    def predict_rbg(self, im):
        """
        预测
        """
        scale = im.size[1] * 1.0 / self.size_h
        w = im.size[0] / scale
        w = int(w)
        img = im.resize((w, self.size_h), Image.BILINEAR)

        if w < self.size_w:
            imgnew = Image.new('RGB', (self.size_w, self.size_h), (255))
            imgnew.paste(img, (0, 0, w, self.size_h))
        else :
            imgnew = img.crop((0, 0, self.size_w,   self.size_h))

        img = np.array(imgnew, dtype=np.float32)

        img -= 127.5
        img /= 127.5
        image = img.transpose(2, 0, 1)
        transformed_image = np.expand_dims(image, axis=0)


        preds = self.sess.run(["out"], {"input": transformed_image.astype(np.float32)})

        pred = np.argmax(preds[0])

        return pred

    def predict_rbgs(self, imgs):
        nlen = len(imgs)
        res_sum = sum([self.predict_rbg(im) for im in imgs])
        return  res_sum < nlen//2


if __name__ == "__main__":
    crnn_handle = AngleNetHandle(model_path="../models/angle_net.onnx")
    import glob
    imgs = glob.glob("/Users/yanghuiyu/Desktop/myself/OCR/mbv3_crnn/test_imgs/*p*g")
    for im_path in imgs:
        im = Image.open(im_path).convert("RGB")

        print(im_path , crnn_handle.predict_rbg(im))