#ifndef __OCR_ANGLENET_H__
#define __OCR_ANGLENET_H__

#include "OcrStruct.h"
#include "onnxruntime_cxx_api.h"
#include <opencv/cv.hpp>

class AngleNet {
public:
    AngleNet();

    ~AngleNet();

    void setNumThread(int numOfThread);

    bool initModel(std::string &pathStr);

    std::vector<Angle> getAngles(std::vector<cv::Mat> &partImgs, const char *path,
                                 const char *imgName, bool doAngle, bool mostAngle);

private:
    bool isOutputAngleImg = false;

    Ort::Session *session;
    Ort::Env env = Ort::Env(ORT_LOGGING_LEVEL_ERROR, "AngleNet");
    Ort::SessionOptions sessionOptions = Ort::SessionOptions();
    int numThread = 0;

    //std::vector<const char *> inputNames;
    //std::vector<const char *> outputNames;
    const char *inputNames[1] = {"input"};
    const char *outputNames[1] = {"out"};

    const float meanValues[3] = {127.5, 127.5, 127.5};
    const float normValues[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};
    const int angleCols = 2;
    const int dstWidth = 192;
    const int dstHeight = 32;

    Angle getAngle(cv::Mat &src);
};


#endif //__OCR_ANGLENET_H__
