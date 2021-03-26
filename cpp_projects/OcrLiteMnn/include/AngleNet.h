#ifndef __OCR_ANGLENET_H__
#define __OCR_ANGLENET_H__

#include "OcrStruct.h"
#include <opencv2/opencv.hpp>
#include <Interpreter.hpp>

class AngleNet {
public:
    AngleNet();

    ~AngleNet();

    void setNumThread(int numOfThread);

    void initModel(const std::string &pathStr);

    std::vector<Angle> getAngles(std::vector<cv::Mat> &partImgs, const char *path,
                                 const char *imgName, bool doAngle, bool mostAngle);

private:
    bool isOutputAngleImg = false;

    std::shared_ptr<MNN::Interpreter> net;
    MNN::Session* session;
    int numThread = 4;

    const float meanValues[3] = {127.5, 127.5, 127.5};
    const float normValues[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};
    const int dstWidth = 192;
    const int dstHeight = 32;

    Angle getAngle(cv::Mat &src);
};


#endif //__OCR_ANGLENET_H__
