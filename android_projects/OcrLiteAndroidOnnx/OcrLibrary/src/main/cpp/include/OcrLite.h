#ifndef __OCR_LITE_H__
#define __OCR_LITE_H__

#include "opencv2/core.hpp"
#include "onnx/onnxruntime_cxx_api.h"
#include "OcrStruct.h"
#include "DbNet.h"
#include "AngleNet.h"
#include "CrnnNet.h"

class OcrLite {
public:
    OcrLite();

    ~OcrLite();

    void init(JNIEnv *jniEnv, jobject assetManager, int numOfThread);

    //void initLogger(bool isDebug);

    //void Logger(const char *format, ...);

    OcrResult detect(cv::Mat &src, cv::Rect &originRect, ScaleParam &scale,
                     float boxScoreThresh, float boxThresh,
                     float unClipRatio, bool doAngle, bool mostAngle);

private:
    bool isLOG = true;
    DbNet dbNet;
    AngleNet angleNet;
    CrnnNet crnnNet;
};


#endif