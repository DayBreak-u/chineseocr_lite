#ifndef __OCR_LITE_H__
#define __OCR_LITE_H__

#include "opencv2/core.hpp"
#include "ncnn/net.h"
#include "OcrStruct.h"

using namespace std;

class OcrLite {
public:
    OcrLite(JNIEnv *jniEnv, jobject assetManager, int numOfThread);

    ~OcrLite();

    OcrResult detect(cv::Mat &src, cv::Rect &originRect, ScaleParam &scale,
                     float boxScoreThresh, float boxThresh, float minArea,
                     float scaleWidth, float scaleHeight);

private:
    ncnn::Net dbNet, angleNet, crnnNet, crnnVerticalNet;
    int numThread = 4;

    const float meanValsDBNet[3] = {0.485 * 255, 0.456 * 255, 0.406 * 255};
    const float normValsDBNet[3] = {1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0};

    int angleDstWidth = 196;
    int angleDstHeight = 32;
    const float meanValsAngle[3] = {0.485 * 255, 0.456 * 255, 0.406 * 255};
    const float normValsAngle[3] = {1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0};

    int crnnDstHeight = 32;
    const float meanValsCrnn[1] = {127.5};
    const float normValsCrnn[1] = {1.0 / 127.5};

    std::vector<std::string> keys;

    std::vector<TextBox> getTextBoxes(cv::Mat &src, ScaleParam &s,
                                      float boxScoreThresh, float boxThresh, float minArea);

    Angle getAngle(cv::Mat &src);

    ncnn::Extractor getExtractor(int angleIndex);
    TextLine getTextLine(cv::Mat &src, int angleIndex);

    TextLine scoreToTextLine(const float *srcData, int h, int w);

};


#endif