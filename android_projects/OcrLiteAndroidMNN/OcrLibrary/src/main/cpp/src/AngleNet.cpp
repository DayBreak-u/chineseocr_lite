#include "AngleNet.h"
#include "OcrUtils.h"
#include <numeric>

AngleNet::AngleNet() {}

AngleNet::~AngleNet() {
}

void AngleNet::setNumThread(int numOfThread) {
    numThread = numOfThread;
}

bool AngleNet::initModel(const char* angelpath) {
    net.reset(MNN::Interpreter::createFromFile(angelpath));
    MNN::ScheduleConfig config;
    config.type  = MNN_FORWARD_CPU;
    config.numThread = numThread;
    session = net->createSession(config);
    return true;
}

Angle scoreToAngle(const std::vector<float> &outputData) {
    int maxIndex = 0;
    float maxScore = -1000.0f;
    for (int i = 0; i < outputData.size(); i++) {
        if (i == 0)
            maxScore = outputData[i];
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

std::vector<Angle> AngleNet::getAngles(std::vector<cv::Mat> &partImgs,
                                       bool doAngle, bool mostAngle) {
    int size = partImgs.size();
    std::vector<Angle> angles(size);
    if (doAngle) {
#ifdef __OPENMP__
#pragma omp parallel for num_threads(numThread)
#endif
        for (int i = 0; i < size; ++i) {
            double startAngle = getCurrentTime();
            auto angleImg = adjustTargetImg(partImgs[i], dstWidth, dstHeight);
            Angle angle = getAngle(angleImg);
            double endAngle = getCurrentTime();
            angle.time = endAngle - startAngle;

            angles[i] = angle;

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
        Logger("Set All Angle to mostAngleIndex(%d)", mostAngleIndex);
        for (int i = 0; i < angles.size(); ++i) {
            Angle angle = angles[i];
            angle.index = mostAngleIndex;
            angles.at(i) = angle;
        }
    }

    return angles;
}