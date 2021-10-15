#ifndef __OCR_ANGLENET_H__
#define __OCR_ANGLENET_H__

#include "OcrStruct.h"
#include "onnx/onnxruntime_cxx_api.h"
#include <opencv2/core.hpp>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

class AngleNet {
public:
    AngleNet();

    ~AngleNet();

    void setNumThread(int numOfThread);

    bool initModel(AAssetManager *mgr);

    std::vector<Angle> getAngles(std::vector<cv::Mat> &partImgs, bool doAngle, bool mostAngle);

private:
    Ort::Session *session;
    Ort::Env ortEnv = Ort::Env(ORT_LOGGING_LEVEL_ERROR, "AngleNet");
    Ort::SessionOptions sessionOptions = Ort::SessionOptions();
    int numThread = 0;

    //std::vector<const char *> inputNames;
    //std::vector<const char *> outputNames;
    const char *inputNames[1] = {"input"};
    const char *outputNames[1] = {"out"};

    const float meanValues[3] = {127.5, 127.5, 127.5};
    const float normValues[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};
    const int angleCols = 2;
    const int dstWidth = 192;
    const int dstHeight = 32;

    Angle getAngle(cv::Mat &src);
};


#endif //__OCR_ANGLENET_H__
