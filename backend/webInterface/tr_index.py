

import tornado.web
import tornado.gen
import tornado.template
import os

BASE_PATH = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


class Index(tornado.web.RequestHandler):

    @tornado.gen.coroutine
    def get(self, *args, **kwargs):
        self.render(os.path.join(BASE_PATH, 'dist/chineseocr_lite_fontend/index.html'))
