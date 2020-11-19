#ifndef __OCR_ANGLENET_H__
#define __OCR_ANGLENET_H__

#include "OcrStruct.h"
#include "onnx/onnxruntime_cxx_api.h"
#include <opencv/cv.hpp>

using namespace cv;
using namespace std;
using namespace Ort;

class AngleNet {
public:

    ~AngleNet();

    bool initModel(string &pathStr, Env &env, SessionOptions &sessionOptions);

    vector<Angle> getAngles(vector<Mat> &partImgs, const char *path,
                            const char *imgName, bool doAngle, bool mostAngle);

private:
    bool isOutputAngleImg = false;

    unique_ptr<Session> session;
    vector<const char *> inputNames;
    vector<const char *> outputNames;

    const float meanValues[3] = {127.5, 127.5, 127.5};
    const float normValues[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};
    const int angleCols = 2;
    const int dstWidth = 192;
    const int dstHeight = 32;

    Angle getAngle(Mat &src);
};


#endif //__OCR_ANGLENET_H__
