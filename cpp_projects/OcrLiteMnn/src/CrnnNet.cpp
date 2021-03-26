#include "CrnnNet.h"
#include "OcrUtils.h"
#include <fstream>
#include <numeric>
#include <Interpreter.hpp>
#include <Tensor.hpp>

CrnnNet::CrnnNet() {}

CrnnNet::~CrnnNet() {
}

void CrnnNet::setNumThread(int numOfThread) {
    numThread = numOfThread;
}

void CrnnNet::initModel(const std::string &pathStr, const std::string &keysPath) {

    net.reset(MNN::Interpreter::createFromFile(pathStr.c_str()));
    MNN::ScheduleConfig config;
    config.type  = MNN_FORWARD_CPU;
    config.numThread = numThread;
    session = net->createSession(config);

    //load keys
    std::ifstream in(keysPath.c_str());
    std::string line;
    if (in) {
        while (getline(in, line)) {// line中不包括每行的换行符
            keys.push_back(line);
        }
    } else {
        printf("The keys.txt file was not found\n");
        return;
    }
    if (keys.size() != 5531) {
        fprintf(stderr, "missing keys\n");
    }
    printf("total keys size(%lu)\n", keys.size());

}

template<class ForwardIterator>
inline static size_t argmax(ForwardIterator first, ForwardIterator last) {
    return std::distance(first, std::max_element(first, last));
}

TextLine CrnnNet::scoreToTextLine(const std::vector<float> &outputData, int h, int w) {
    int keySize = keys.size();
    std::string strRes;
    std::vector<float> scores;
    int lastIndex = 0;
    int maxIndex;
    float maxValue;

    for (int i = 0; i < h; i++) {
        maxIndex = 0;
        maxValue = -1000.f;
        //do softmax
        std::vector<float> exps(w);
        for (int j = 0; j < w; j++) {
            float expSingle = exp(outputData[i * w + j]);
            exps.at(j) = expSingle;
        }
        float partition = accumulate(exps.begin(), exps.end(), 0.0);//row sum
        maxIndex = int(argmax(exps.begin(), exps.end()));
        maxValue = float(*std::max_element(exps.begin(), exps.end())) / partition;
        if (maxIndex > 0 && maxIndex < keySize && (!(i > 0 && maxIndex == lastIndex))) {
            scores.emplace_back(maxValue);
            strRes.append(keys[maxIndex - 1]);
        }
        lastIndex = maxIndex;
    }
    return {strRes, scores};
}

TextLine CrnnNet::getTextLine(const cv::Mat &src) {
    float scale = (float) dstHeight / (float) src.rows;
    int dstWidth = int((float) src.cols * scale);
    cv::Mat srcResize;
    resize(src, srcResize, cv::Size(dstWidth, dstHeight));
    std::vector<float> inputTensorValues = substractMeanNormalize(srcResize, meanValues, normValues);
    std::vector<int>inputShape = {1, srcResize.channels(), srcResize.rows, srcResize.cols};
    auto input = net->getSessionInput(session, "input");
    auto output = net->getSessionOutput(session, "out");
    auto shape = input->shape();
    shape[0]   = 1;
    shape[2]   = srcResize.rows;  
    shape[3]   = srcResize.cols;   
    net->resizeTensor(input, shape);
    net->resizeSession(session);
    auto nchwTensor = MNN::Tensor::create(inputShape,halide_type_of<float>(),&inputTensorValues[0],MNN::Tensor::CAFFE);
    input->copyFromHostTensor(nchwTensor);
    delete nchwTensor;
    net->runSession(session);
    
    std::shared_ptr<MNN::Tensor> outputUser(new MNN::Tensor(output, output->getDimensionType())); //nchw
    output->copyToHostTensor(outputUser.get());
    auto values = outputUser->host<float>();
    int size = outputUser->elementSize();

    std::vector<float> outputData(values, values + size);
    return scoreToTextLine(outputData, output->shape()[0], output->shape()[2]);
}

std::vector<TextLine> CrnnNet::getTextLines(std::vector<cv::Mat> &partImg, const char *path, const char *imgName) {
    int size = partImg.size();
    std::vector<TextLine> textLines(size);
    for (int i = 0; i < size; ++i) {
        //OutPut DebugImg
        if (isOutputDebugImg) {
            std::string debugImgFile = getDebugImgFilePath(path, imgName, i, "-debug-");
            saveImg(partImg[i], debugImgFile.c_str());
        }

        //getTextLine
        double startCrnnTime = getCurrentTime();
        TextLine textLine = getTextLine(partImg[i]);
        double endCrnnTime = getCurrentTime();
        textLine.time = endCrnnTime - startCrnnTime;
        textLines[i] = textLine;
    }
    return textLines;
}