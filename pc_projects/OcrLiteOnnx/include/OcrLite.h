#ifndef __OCR_LITE_H__
#define __OCR_LITE_H__

#include "opencv2/core.hpp"
#include "onnx/onnxruntime_cxx_api.h"
#include "OcrStruct.h"

class OcrLite {
public:
    OcrLite(int numOfThread);

    ~OcrLite();

    void initLogger(bool isConsole, bool isPartImg, bool isAngleImg, bool isDebugImg, bool isResultImg);

    void enableResultTxt(const char *path, const char *imgName);

    bool initModels(const char *path);

    void Logger(const char *format, ...);

    OcrResult detect(const char *path, const char *imgName,
                     int padding, float imgScale,
                     float boxScoreThresh, float boxThresh, float minArea,
                     float unClipRatio, bool doAngle, bool mostAngle);

    OcrResult detect(const char *path, const char *imgName,
                     int padding, int imgResize,
                     float boxScoreThresh, float boxThresh, float minArea,
                     float unClipRatio, bool doAngle, bool mostAngle);

private:
    bool isOutputConsole = false;
    bool isOutputPartImg = false;
    bool isOutputAngleImg = false;
    bool isOutputDebugImg = false;
    bool isOutputResultTxt = false;
    bool isOutputResultImg = false;
    FILE *resultTxt;

    Ort::Env env = Ort::Env(ORT_LOGGING_LEVEL_ERROR, "OcrLite");
    Ort::SessionOptions sessionOptions;
    std::unique_ptr<Ort::Session> dbNet, angleNet, crnnNet;
    std::vector<const char *> dbNetInputNames, angleNetInputNames, crnnNetInputNames;
    std::vector<const char *> dbNetOutputNames, angleNetOutputNames, crnnNetOutputNames;

    int numThread = 0;

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
                                      float boxScoreThresh, float boxThresh, float minArea, float unClipRatio);

    Angle getAngle(cv::Mat &src);

    TextLine getTextLine(cv::Mat &src);

    TextLine scoreToTextLine(const float *srcData, int h, int w);

    void drawTextBoxes(cv::Mat &boxImg, std::vector<TextBox> &textBoxes, int thickness);

    std::vector<cv::Mat>
    getPartImages(cv::Mat &src, std::vector<TextBox> &textBoxes, const char *path, const char *imgName);

    std::vector<Angle>
    getAngles(std::vector<cv::Mat> &partImgs, const char *path, const char *imgName, bool doAngle, bool mostAngle);

    std::vector<TextLine> getTextLines(std::vector<cv::Mat> &partImg, const char *path, const char *imgName);

    OcrResult detect(const char *path, const char *imgName,
                     cv::Mat &src, cv::Rect &originRect, ScaleParam &scale,
                     float boxScoreThresh = 0.6f, float boxThresh = 0.3f, float minArea = 3.f,
                     float unClipRatio = 2.0f, bool doAngle = true, bool mostAngle = true);
};

#endif //__OCR_LITE_H__
