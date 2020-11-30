#ifndef __OCR_DBNET_H__
#define __OCR_DBNET_H__

#include "OcrStruct.h"
#include "onnx/onnxruntime_cxx_api.h"
#include <opencv/cv.hpp>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

class DbNet {
public:
    ~DbNet();

    bool initModel(AAssetManager *mgr, Ort::Env &ortEnv, Ort::SessionOptions &sessionOptions);

    std::vector<TextBox> getTextBoxes(cv::Mat &src, ScaleParam &s, float boxScoreThresh,
                                      float boxThresh, float minArea, float unClipRatio);

private:
    std::unique_ptr<Ort::Session> session;
    std::vector<const char *> inputNames;
    std::vector<const char *> outputNames;

    const float meanValues[3] = {0.485 * 255, 0.456 * 255, 0.406 * 255};
    const float normValues[3] = {1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0};

};


#endif //__OCR_DBNET_H__
