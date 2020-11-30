#include "AngleNet.h"
#include "OcrUtils.h"
#include <numeric>

AngleNet::~AngleNet() {
    session.release();
}

bool AngleNet::initModel(AAssetManager *mgr, Ort::Env &ortEnv, Ort::SessionOptions &sessionOptions) {
    int dbModelDataLength = 0;
    void *dbModelData = getModelDataFromAssets(mgr, "angle_net.onnx", dbModelDataLength);
    session = std::make_unique<Ort::Session>(ortEnv, dbModelData, dbModelDataLength,
                                             sessionOptions);

    inputNames = getInputNames(session);
    outputNames = getOutputNames(session);

    return true;
}

Angle scoreToAngle(const float *srcData, int w) {
    int angleIndex = 0;
    float maxValue = -1000.0f;
    for (int i = 0; i < w; i++) {
        if (i == 0)maxValue = srcData[i];
        else if (srcData[i] > maxValue) {
            angleIndex = i;
            maxValue = srcData[i];
        }
    }
    return {angleIndex, maxValue};
}

Angle AngleNet::getAngle(cv::Mat &src) {

    std::vector<float> inputTensorValues = substractMeanNormalize(src, meanValues, normValues);

    std::array<int64_t, 4> inputShape{1, src.channels(), src.rows, src.cols};

    auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(),
                                                             inputTensorValues.size(), inputShape.data(),
                                                             inputShape.size());
    assert(inputTensor.IsTensor());

    auto outputTensor = session->Run(Ort::RunOptions{nullptr}, inputNames.data(), &inputTensor,
                                     inputNames.size(),
                                     outputNames.data(), outputNames.size());

    assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());

    size_t count = outputTensor.front().GetTensorTypeAndShapeInfo().GetElementCount();
    size_t rows = count / angleCols;
    float *floatArray = outputTensor.front().GetTensorMutableData<float>();

    cv::Mat score(rows, angleCols, CV_32FC1);
    memcpy(score.data, floatArray, rows * angleCols * sizeof(float));

    return scoreToAngle((float *) score.data, angleCols);
}

std::vector<Angle> AngleNet::getAngles(std::vector<cv::Mat> &partImgs,
                                       bool doAngle, bool mostAngle) {
    int size = partImgs.size();
    std::vector<Angle> angles(size);
    if (doAngle) {
#ifdef __OPENMP__
#pragma omp parallel for
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
            angles.emplace_back(Angle{-1, 0.f});
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