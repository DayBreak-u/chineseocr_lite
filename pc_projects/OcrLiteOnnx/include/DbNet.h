#ifndef __OCR_DBNET_H__
#define __OCR_DBNET_H__

#include "OcrStruct.h"
#include "onnx/onnxruntime_cxx_api.h"
#include <opencv/cv.hpp>

using namespace cv;
using namespace std;
using namespace Ort;

class DbNet {
public:
    ~DbNet();

    bool initModel(string &pathStr, Env &env, SessionOptions &sessionOptions);

    vector<TextBox> getTextBoxes(Mat &src, ScaleParam &s, float boxScoreThresh,
                                 float boxThresh, float minArea, float unClipRatio);

private:
    unique_ptr<Session> session;
    vector<const char *> inputNames;
    vector<const char *> outputNames;

    const float meanValues[3] = {0.485 * 255, 0.456 * 255, 0.406 * 255};
    const float normValues[3] = {1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0};
};


#endif //__OCR_DBNET_H__
