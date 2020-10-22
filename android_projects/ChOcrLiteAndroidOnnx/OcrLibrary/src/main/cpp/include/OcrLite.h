#ifndef __OCR_LITE_H__
#define __OCR_LITE_H__

#include "opencv2/core.hpp"
#include "onnx/onnxruntime_cxx_api.h"
#include "OcrStruct.h"

using namespace std;

class OcrLite {
public:
    OcrLite(JNIEnv *jniEnv, jobject assetManager, int numThread);

    ~OcrLite();

    OcrResult detect(cv::Mat &src, cv::Rect &originRect, ScaleParam &scale,
                     float boxScoreThresh, float boxThresh, float minArea,
                     float scaleWidth, float scaleHeight);

private:
    Ort::Env ortEnv = Ort::Env(ORT_LOGGING_LEVEL_ERROR, "OcrLite");
    Ort::SessionOptions sessionOptions;
    std::unique_ptr<Ort::Session> dbNet, angleNet, crnnNet;
    std::vector<const char *> dbNetInputNames, angleNetInputNames, crnnNetInputNames;
    std::vector<const char *> dbNetOutputNames, angleNetOutputNames, crnnNetOutputNames;

    const float meanValsDBNet[3] = {0.485 * 255, 0.456 * 255, 0.406 * 255};
    const float normValsDBNet[3] = {1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0};

    const int angleDstWidth = 192;
    const int angleDstHeight = 32;
    const float meanValsAngle[3] = {0.485 * 255, 0.456 * 255, 0.406 * 255};
    const float normValsAngle[3] = {1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0};
    const int angleCols = 2;

    const int crnnDstHeight = 32;
    const float meanValsCrnn[3] = {127.5, 127.5, 127.5};
    const float normValsCrnn[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};
    const int crnnCols = 5531;

    std::vector<std::string> keys;

    std::vector<TextBox> getTextBoxes(cv::Mat &src, ScaleParam &s,
                                      float boxScoreThresh, float boxThresh, float minArea);

    Angle getAngle(cv::Mat &src);

    TextLine getTextLine(cv::Mat &src);

    TextLine scoreToTextLine(const float *srcData, int h, int w);

};


#endif