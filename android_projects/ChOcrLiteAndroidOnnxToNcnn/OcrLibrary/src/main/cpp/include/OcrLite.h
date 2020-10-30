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

    void initLogger(bool isDebug);

    void Logger(const char *format, ...);

    OcrResult detect(cv::Mat &src, cv::Rect &originRect, ScaleParam &scale,
                     float boxScoreThresh, float boxThresh, float minArea,
                     float unClipRatio, bool doAngle, bool mostAngle);

private:
    bool isLOG = true;
    ncnn::Net dbNet, angleNet, crnnNet;
    int numThread = 4;

    const float meanValsDBNet[3] = {0.485 * 255, 0.456 * 255, 0.406 * 255};
    const float normValsDBNet[3] = {1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0};

    const int angleDstWidth = 192;
    const int angleDstHeight = 32;
    const float meanValsAngle[3] = {0.485 * 255, 0.456 * 255, 0.406 * 255};
    const float normValsAngle[3] = {1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0};

    const int crnnDstHeight = 32;
    const float meanValsCrnn[3] = {127.5, 127.5, 127.5};
    const float normValsCrnn[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};

    std::vector<std::string> keys;

    std::vector<TextBox> getTextBoxes(cv::Mat &src, ScaleParam &s,
                                      float boxScoreThresh, float boxThresh, float minArea,
                                      float unClipRatio);

    Angle getAngle(cv::Mat &src);

    TextLine getTextLine(cv::Mat &src);

    void drawTextBoxes(cv::Mat &boxImg, std::vector<TextBox> &textBoxes, int thickness);

    std::vector<cv::Mat> getPartImages(cv::Mat &src, std::vector<TextBox> &textBoxes);

    std::vector<Angle> getAngles(std::vector<cv::Mat> &partImgs, bool doAngle, bool mostAngle);

    std::vector<TextLine> getTextLines(std::vector<cv::Mat> &partImg);

    TextLine scoreToTextLine(const float *srcData, int h, int w);

};


#endif