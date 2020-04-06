FROM python:3.6
ENV LANG C.UTF-8

COPY . /chineseocr_lite

WORKDIR /chineseocr_lite

RUN PIP_INSTALL="pip install -i https://mirrors.aliyun.com/pypi/simple/" && \
    $PIP_INSTALL \
    numpy \
    scipy \
    pandas \
    cloudpickle \
    scikit-image>=0.14.2 \
    scikit-learn \
    matplotlib \
    Cython \
    tqdm \
    future \
    protobuf \
    enum34 \
    pyyaml \
    typing \
    torch \
    torchvision \
    && \
    $PIP_INSTALL -r requirements.txt

CMD python app.py 8080
