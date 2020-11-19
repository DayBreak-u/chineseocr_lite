#ifndef __OCR_LITE_H__
#define __OCR_LITE_H__

#include "opencv2/core.hpp"
#include "onnx/onnxruntime_cxx_api.h"
#include "OcrStruct.h"
#include "DbNet.h"
#include "AngleNet.h"
#include "CrnnNet.h"

using namespace cv;
using namespace std;
using namespace Ort;

class OcrLite {
public:
    OcrLite(int numOfThread);

    ~OcrLite();

    void initLogger(bool isConsole, bool isPartImg, bool isResultImg);

    void enableResultTxt(const char *path, const char *imgName);

    void initModels(const char *path);

    void Logger(const char *format, ...);

    OcrResult detect(const char *path, const char *imgName,
                     int padding, int imgResize,
                     float boxScoreThresh, float boxThresh, float minArea,
                     float unClipRatio, bool doAngle, bool mostAngle);

private:
    bool isOutputConsole = false;
    bool isOutputPartImg = false;
    bool isOutputResultTxt = false;
    bool isOutputResultImg = false;
    FILE *resultTxt;
    int numThread = 0;
    Env env = Env(ORT_LOGGING_LEVEL_ERROR, "OcrLite");
    SessionOptions sessionOptions;
    DbNet dbnet;
    AngleNet angleNet;
    CrnnNet crnnNet;

    vector<Mat> getPartImages(Mat &src, vector<TextBox> &textBoxes,
                              const char *path, const char *imgName);

    OcrResult detect(const char *path, const char *imgName,
                     Mat &src, Rect &originRect, ScaleParam &scale,
                     float boxScoreThresh = 0.6f, float boxThresh = 0.3f, float minArea = 3.f,
                     float unClipRatio = 2.0f, bool doAngle = true, bool mostAngle = true);
};

#endif //__OCR_LITE_H__
