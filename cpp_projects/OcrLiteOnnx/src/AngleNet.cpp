#include "AngleNet.h"
#include "OcrUtils.h"
#include <numeric>

AngleNet::AngleNet() {}

AngleNet::~AngleNet() {
    delete session;
    free(inputName);
    free(outputName);
}

void AngleNet::setNumThread(int numOfThread) {
    numThread = numOfThread;
    //===session options===
    // Sets the number of threads used to parallelize the execution within nodes
    // A value of 0 means ORT will pick a default
    //sessionOptions.SetIntraOpNumThreads(numThread);
    //set OMP_NUM_THREADS=16

    // Sets the number of threads used to parallelize the execution of the graph (across nodes)
    // If sequential execution is enabled this value is ignored
    // A value of 0 means ORT will pick a default
    sessionOptions.SetInterOpNumThreads(numThread);

    // Sets graph optimization level
    // ORT_DISABLE_ALL -> To disable all optimizations
    // ORT_ENABLE_BASIC -> To enable basic optimizations (Such as redundant node removals)
    // ORT_ENABLE_EXTENDED -> To enable extended optimizations (Includes level 1 + more complex optimizations like node fusions)
    // ORT_ENABLE_ALL -> To Enable All possible opitmizations
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
}

void AngleNet::initModel(const std::string &pathStr) {
#ifdef _WIN32
    std::wstring anglePath = strToWstr(pathStr);
    session = new Ort::Session(env, anglePath.c_str(), sessionOptions);
#else
    session = new Ort::Session(env, pathStr.c_str(), sessionOptions);
#endif
    getInputName(session,inputName);
    getOutputName(session,outputName);
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

    std::array<int64_t, 4> inputShape{1, src.channels(), src.rows, src.cols};

    auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(),
                                                             inputTensorValues.size(), inputShape.data(),
                                                             inputShape.size());
    assert(inputTensor.IsTensor());

    auto outputTensor = session->Run(Ort::RunOptions{nullptr}, &inputName, &inputTensor, 1, &outputName, 1);

    assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());

    std::vector<int64_t> outputShape = outputTensor[0].GetTensorTypeAndShapeInfo().GetShape();

    int64_t outputCount = std::accumulate(outputShape.begin(), outputShape.end(), 1,
                                          std::multiplies<int64_t>());

    float *floatArray = outputTensor.front().GetTensorMutableData<float>();
    std::vector<float> outputData(floatArray, floatArray + outputCount);
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