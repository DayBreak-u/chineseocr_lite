#ifndef __OCR_CRNNNET_H__
#define __OCR_CRNNNET_H__

#include "OcrStruct.h"
#include "onnx/onnxruntime_cxx_api.h"
#include <opencv/cv.hpp>

using namespace cv;
using namespace std;
using namespace Ort;

class CrnnNet {
public:

    ~CrnnNet();

    bool initModel(string &pathStr, Env &env, SessionOptions &sessionOptions);

    vector<TextLine> getTextLines(vector<Mat> &partImg, const char *path, const char *imgName);

private:
    bool isOutputDebugImg = false;
    unique_ptr<Session> session;
    vector<const char *> inputNames;
    vector<const char *> outputNames;

    const float meanValues[3] = {127.5, 127.5, 127.5};
    const float normValues[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};
    const int dstHeight = 32;
    const int crnnCols = 5531;

    vector<string> keys;

    TextLine scoreToTextLine(const float *srcData, int h, int w);

    TextLine getTextLine(Mat &src);
};


#endif //__OCR_CRNNNET_H__
