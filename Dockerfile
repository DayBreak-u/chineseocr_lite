FROM python:3.6
ENV LANG C.UTF-8

COPY . /chineseocr_lite

WORKDIR /chineseocr_lite

RUN PIP_INSTALL="pip install -i https://pypi.tuna.tsinghua.edu.cn/simple/" && \
    $PIP_INSTALL \
    torch==1.2.0 \
    torchvision==0.4.0 \
    && \
    $PIP_INSTALL -r requirements.txt

CMD python app.py 8080
