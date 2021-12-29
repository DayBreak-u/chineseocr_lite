#include "DbNet.h"
#include "OcrUtils.h"

DbNet::DbNet() {}

DbNet::~DbNet() {
}

void DbNet::setNumThread(int numOfThread) {
    numThread = numOfThread;
}

bool DbNet::initModel(const char* dbnetpath) {
    net.reset(MNN::Interpreter::createFromFile(dbnetpath));
    MNN::ScheduleConfig config;
    config.type  = MNN_FORWARD_CPU;
    config.numThread = numThread;
    session = net->createSession(config);

    return true;
}

std::vector<TextBox>
DbNet::getTextBoxes(cv::Mat &src, ScaleParam &s, float boxScoreThresh, float boxThresh,
                    float unClipRatio) {
    float minArea = 3.0f;
    cv::Mat srcResize;
    resize(src, srcResize, cv::Size(s.dstWidth, s.dstHeight));
    std::vector<float> inputTensorValues = substractMeanNormalize(srcResize, meanValues,
                                                                  normValues);

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


    std::vector<TextBox> rsBoxes;
    cv::Mat norfMapMat;
    norfMapMat = fMapMat > boxThresh;
    rsBoxes.clear();
    std::vector<std::vector<cv::Point>> contours;
    findContours(norfMapMat, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); ++i) {
        std::vector<cv::Point> minBox;
        float minEdgeSize, allEdgeSize;
        getMiniBoxes(contours[i], minBox, minEdgeSize, allEdgeSize);

        if (minEdgeSize < minArea)
            continue;
        float score = boxScoreFast(fMapMat, contours[i]);

        if (score < boxScoreThresh)
            continue;
        //---use clipper start---
        std::vector<cv::Point> newbox;
        unClip(minBox, allEdgeSize, newbox, unClipRatio);

        getMiniBoxes(newbox, minBox, minEdgeSize, allEdgeSize);

        if (minEdgeSize < minArea + 2)
            continue;
        //---use clipper end---

        for (int j = 0; j < minBox.size(); ++j) {
            minBox[j].x = (minBox[j].x / s.ratioWidth);
            minBox[j].x = (std::min)((std::max)(minBox[j].x, 0), s.srcWidth);

            minBox[j].y = (minBox[j].y / s.ratioHeight);
            minBox[j].y = (std::min)((std::max)(minBox[j].y, 0), s.srcHeight);
        }

        rsBoxes.emplace_back(TextBox{minBox, score});
    }
    reverse(rsBoxes.begin(), rsBoxes.end());
    return rsBoxes;
}
