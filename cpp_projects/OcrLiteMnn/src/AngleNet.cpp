#include "AngleNet.h"
#include "OcrUtils.h"
#include <numeric>
#include <Interpreter.hpp>
#include <Tensor.hpp>

AngleNet::AngleNet() {}

AngleNet::~AngleNet() {
}

void AngleNet::setNumThread(int numOfThread) {
    numThread = numOfThread;
    //===session options===
    // Sets the number of threads used to parallelize the execution within nodes
    // A value of 0 means ORT will pick a default
    //sessionOptions.SetIntraOpNumThreads(numThread);
    //set OMP_NUM_THREADS=16
}

void AngleNet::initModel(const std::string &pathStr) {

    net.reset(MNN::Interpreter::createFromFile(pathStr.c_str()));
    MNN::ScheduleConfig config;
    config.type  = MNN_FORWARD_CPU;
    config.numThread = numThread;
    session = net->createSession(config);
}

Angle scoreToAngle(const std::vector<float> &outputData) {
    int maxIndex = 0;
    float maxScore = -1000.0f;
    for (int i = 0; i < outputData.size(); i++) {
        if (i == 0)maxScore = outputData[i];
        else if (outputData[i] > maxScore) {
            maxScore = outputData[i];
            maxIndex = i;
        }
    }
    return {maxIndex, maxScore};
}

Angle AngleNet::getAngle(cv::Mat &src) {
    std::vector<float> inputTensorValues = substractMeanNormalize(src, meanValues, normValues);
    std::vector<int>inputShape = {1, src.channels(), src.rows, src.cols};
    auto input = net->getSessionInput(session, NULL);
    auto output = net->getSessionOutput(session, NULL);
    auto shape = input->shape();
    shape[0]   = 1;
    shape[2]   = src.rows;  
    shape[3]   = src.cols;   
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
    return scoreToAngle(outputData);
}

std::vector<Angle> AngleNet::getAngles(std::vector<cv::Mat> &partImgs, const char *path,
                                       const char *imgName, bool doAngle, bool mostAngle) {
    int size = partImgs.size();
    std::vector<Angle> angles(size);
    if (doAngle) {
        for (int i = 0; i < size; ++i) {
            double startAngle = getCurrentTime();
            auto angleImg = adjustTargetImg(partImgs[i], dstWidth, dstHeight);
            Angle angle = getAngle(angleImg);
            double endAngle = getCurrentTime();
            angle.time = endAngle - startAngle;

            angles[i] = angle;

            //OutPut AngleImg
            if (isOutputAngleImg) {
                std::string angleImgFile = getDebugImgFilePath(path, imgName, i, "-angle-");
                saveImg(angleImg, angleImgFile.c_str());
            }
        }
    } else {
        for (int i = 0; i < size; ++i) {
            angles[i] = Angle{-1, 0.f};
        }
    }
    //Most Possible AngleIndex
    if (doAngle && mostAngle) {
        auto angleIndexes = getAngleIndexes(angles);
        double sum = std::accumulate(angleIndexes.begin(), angleIndexes.end(), 0.0);
        double halfPercent = angles.size() / 2.0f;
        int mostAngleIndex;
        if (sum < halfPercent) {//all angle set to 0
            mostAngleIndex = 0;
        } else {//all angle set to 1
            mostAngleIndex = 1;
        }
        printf("Set All Angle to mostAngleIndex(%d)\n", mostAngleIndex);
        for (int i = 0; i < angles.size(); ++i) {
            Angle angle = angles[i];
            angle.index = mostAngleIndex;
            angles.at(i) = angle;
        }
    }

    return angles;
}