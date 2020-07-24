import os
from flask import Flask
from flask_restful import Api, Resource, reqparse
from config import version
from utils import app_url
from apphelper.image import base64_to_PIL
import numpy as np
from model import text_predict

app = Flask(__name__)
api = Api(app)


class ImageIdentify(Resource):
    # http://ip:port/api/v1/ocr
    def post(self, **kwargs):
        parser = reqparse.RequestParser()
        parser.add_argument('ImgString', type=str,
                            required=True, location='form')
        args = parser.parse_args()
        rst = None
        imgString = args['ImgString'].encode().split(b';base64,')[-1]

        try:
            b64_image = base64_to_PIL(imgString)
            if b64_image is not None:
                b64_image = np.array(b64_image)
                result = text_predict(b64_image)
                text = ' '.join([i['text'] for i in result])
                rst = {
                    'code': "success",
                    'text': text
                }
        except ValueError:
            rst = {
                "code": "FAILURE",
                "errMsg": "ocr访问出错",
                "id": "1501"
            }
        return rst


api.add_resource(ImageIdentify, app_url(version, '/ocr'))

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=False)
