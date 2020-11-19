#include "DbNet.h"
#include "OcrUtils.h"

DbNet::~DbNet() {
    session.release();
}

bool DbNet::initModel(string &pathStr, Env &env, SessionOptions &sessionOptions) {
#ifdef _WIN32
    wstring dbPath = strToWstr(pathStr + "/dbnet.onnx");
    session = makeUnique<Session>(env, dbPath.c_str(), sessionOptions);
#else
    session = makeUnique<Session>(env, (pathStr + "/dbnet.onnx").c_str(), sessionOptions);
#endif
    inputNames = getInputNames(session);
    outputNames = getOutputNames(session);
    return true;
}

vector<TextBox>
DbNet::getTextBoxes(Mat &src, ScaleParam &s,
                    float boxScoreThresh, float boxThresh, float minArea, float unClipRatio) {
    Mat srcResize;
    resize(src, srcResize, Size(s.dstWidth, s.dstHeight));
    vector<float> inputTensorValues = substractMeanNormalize(srcResize, meanValues, normValues);

    array<int64_t, 4> inputShape{1, srcResize.channels(), srcResize.rows, srcResize.cols};

    auto memoryInfo = MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    Value inputTensor = Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(),
                                                   inputTensorValues.size(), inputShape.data(),
                                                   inputShape.size());
    assert(inputTensor.IsTensor());

    auto outputTensor = session->Run(RunOptions{nullptr}, inputNames.data(), &inputTensor,
                                     inputNames.size(),
                                     outputNames.data(), outputNames.size());

    assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());

    float *floatArray = outputTensor.front().GetTensorMutableData<float>();

    Mat fMapMat(srcResize.rows, srcResize.cols, CV_32FC1);
    memcpy(fMapMat.data, floatArray, srcResize.rows * srcResize.cols * sizeof(float));

    vector<TextBox> rsBoxes;
    Mat norfMapMat;
    norfMapMat = fMapMat > boxThresh;
    rsBoxes.clear();
    vector<vector<Point>> contours;
    findContours(norfMapMat, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); ++i) {
        vector<Point> minBox;
        float minEdgeSize, allEdgeSize;
        getMiniBoxes(contours[i], minBox, minEdgeSize, allEdgeSize);

        if (minEdgeSize < minArea)
            continue;
        float score = boxScoreFast(fMapMat, contours[i]);

        if (score < boxScoreThresh)
            continue;
        //---use clipper start---
        vector<Point> newbox;
        unClip(minBox, allEdgeSize, newbox, unClipRatio);

        getMiniBoxes(newbox, minBox, minEdgeSize, allEdgeSize);

        if (minEdgeSize < minArea + 2)
            continue;
        //---use clipper end---

        for (int j = 0; j < minBox.size(); ++j) {
            minBox[j].x = (minBox[j].x / s.scaleWidth);
            minBox[j].x = (min)((max)(minBox[j].x, 0), s.srcWidth);

            minBox[j].y = (minBox[j].y / s.scaleHeight);
            minBox[j].y = (min)((max)(minBox[j].y, 0), s.srcHeight);
        }

        rsBoxes.emplace_back(TextBox(minBox, score));
    }
    reverse(rsBoxes.begin(), rsBoxes.end());
    return rsBoxes;
}
