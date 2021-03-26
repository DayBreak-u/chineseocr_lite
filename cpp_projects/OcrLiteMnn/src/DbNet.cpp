
#include "DbNet.h"
#include "OcrUtils.h"
#include <Interpreter.hpp>
#include <Tensor.hpp>

DbNet::DbNet() {}

DbNet::~DbNet() {
}

void DbNet::setNumThread(int numOfThread) {
    numThread = numOfThread;
}

void DbNet::initModel(const std::string &pathStr) {
    net.reset(MNN::Interpreter::createFromFile(pathStr.c_str()));
    MNN::ScheduleConfig config;
    config.type  = MNN_FORWARD_CPU;
    config.numThread = numThread;
    session = net->createSession(config);
}

std::vector<TextBox> findRsBoxes(const cv::Mat &fMapMat, const cv::Mat &norfMapMat, ScaleParam &s,
                                 const float boxScoreThresh, const float unClipRatio) {
    float minArea = 3;
    std::vector<TextBox> rsBoxes;
    rsBoxes.clear();
    std::vector<std::vector<cv::Point>> contours;
    findContours(norfMapMat, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); ++i) {
        float minSideLen, perimeter;
        std::vector<cv::Point> minBox = getMinBoxes(contours[i], minSideLen, perimeter);
        if (minSideLen < minArea)
            continue;
        float score = boxScoreFast(fMapMat, contours[i]);
        if (score < boxScoreThresh)
            continue;
        //---use clipper start---
        std::vector<cv::Point> clipBox = unClip(minBox, perimeter, unClipRatio);
        std::vector<cv::Point> clipMinBox = getMinBoxes(clipBox, minSideLen, perimeter);
        //---use clipper end---

        if (minSideLen < minArea + 2)
            continue;

        for (int j = 0; j < clipMinBox.size(); ++j) {
            clipMinBox[j].x = (clipMinBox[j].x / s.ratioWidth);
            clipMinBox[j].x = (std::min)((std::max)(clipMinBox[j].x, 0), s.srcWidth);

            clipMinBox[j].y = (clipMinBox[j].y / s.ratioHeight);
            clipMinBox[j].y = (std::min)((std::max)(clipMinBox[j].y, 0), s.srcHeight);
        }

        rsBoxes.emplace_back(TextBox{clipMinBox, score});
    }
    reverse(rsBoxes.begin(), rsBoxes.end());
    return rsBoxes;
}

std::vector<TextBox>
DbNet::getTextBoxes(cv::Mat &src, ScaleParam &s, float boxScoreThresh, float boxThresh, float unClipRatio) {
    cv::Mat srcResize;
    resize(src, srcResize, cv::Size(s.dstWidth, s.dstHeight)); // w*h
    
    std::vector<float> inputTensorValues = substractMeanNormalize(srcResize, meanValues, normValues);

    auto input = net->getSessionInput(session, NULL);
    auto output = net->getSessionOutput(session, NULL);
    auto shape = input->shape();
    shape[0]   = 1;
    shape[2]   = srcResize.rows;  
    shape[3]   = srcResize.cols;   
    net->resizeTensor(input, shape);
    net->resizeSession(session);
    auto shapein = input->shape();
    std::vector<int> v = {1,(int)srcResize.channels(),srcResize.rows,srcResize.cols};
    auto nchwTensor = MNN::Tensor::create(v,halide_type_of<float>(),&inputTensorValues[0],MNN::Tensor::CAFFE);
    input->copyFromHostTensor(nchwTensor);
    delete nchwTensor;
    net->runSession(session);

    std::shared_ptr<MNN::Tensor> outputUser(new MNN::Tensor(output, output->getDimensionType())); //nchw
    output->copyToHostTensor(outputUser.get());
    auto values = outputUser->host<float>();
    //-----Data preparation-----
    cv::Mat fMapMat(srcResize.rows, srcResize.cols, CV_32FC1); //  h* w  rows* cols
    ::memcpy(fMapMat.data, values, outputUser->stride(1) * sizeof(float));  // 只取第一个channel的数据
    //-----boxThresh-----
    cv::Mat norfMapMat;
    norfMapMat = fMapMat > boxThresh;
    return findRsBoxes(fMapMat, norfMapMat, s, boxScoreThresh, unClipRatio);
}
