import os
from flask import Flask, jsonify
from flask_restful import Api, Resource, reqparse
from config import version, TIMEOUT
from utils import app_url
from apphelper.image import base64_to_PIL
import time
import numpy as np
import uuid
from model import text_predict

app = Flask(__name__)
api = Api(app)

filelock = 'file.lock'
if os.path.exists(filelock):
    os.remove(filelock)


class ImageIdentify(Resource):
    # http://ip:port/api/v1/ocr
    def post(self, **kwargs):
        parser = reqparse.RequestParser()
        parser.add_argument('imgString', type=str, location='form')
        args = parser.parse_args()
        
        result = None
        t = time.time()
        uidJob = uuid.uuid1().__str__()
        imgString = args['imgString'].encode().split(b';base64,')[-1]
        b64_image = base64_to_PIL(imgString)
        if b64_image:
            b64_image = np.array(b64_image)
        
        # 避免识别过程中重复上传识别
        while time.time() - t <= TIMEOUT:
            if os.path.exists(filelock):
                continue
            else:
                with open(filelock, 'w') as f:
                    f.write(uidJob)
                
                # 图片识别
                result = text_predict(b64_image)
                os.remove(filelock)
                break
        return jsonify({'text': [i['text'] for i in result]})


api.add_resource(ImageIdentify, app_url(version, '/ocr'))

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=False)
