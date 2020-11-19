#ifndef __OCR_ANGLENET_H__
#define __OCR_ANGLENET_H__

#include "OcrStruct.h"
#include "ncnn/net.h"
#include <opencv/cv.hpp>

using namespace cv;
using namespace std;

class AngleNet {
public:

    ~AngleNet();

    void setNumOfThreads(int numOfThread);

    bool initModel(AAssetManager *mgr);

    vector<Angle> getAngles(vector<cv::Mat> &partImgs, bool doAngle, bool mostAngle);

private:
    int numThread;
    ncnn::Net net;
    const float meanValues[3] = {127.5, 127.5, 127.5};
    const float normValues[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};

    const int dstWidth = 192;
    const int dstHeight = 32;

    Angle getAngle(cv::Mat &src);
};


#endif //__OCR_ANGLENET_H__
