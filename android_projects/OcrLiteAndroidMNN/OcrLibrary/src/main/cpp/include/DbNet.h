#ifndef __OCR_DBNET_H__
#define __OCR_DBNET_H__

#include "OcrStruct.h"
#include <opencv/cv.hpp>
#include <Interpreter.hpp>

class DbNet {
public:
    DbNet();

    ~DbNet();

    void setNumThread(int numOfThread);

    bool initModel(const char* dbnetpath);

    std::vector<TextBox> getTextBoxes(cv::Mat &src, ScaleParam &s, float boxScoreThresh,
                                      float boxThresh, float unClipRatio);

private:
    std::shared_ptr<MNN::Interpreter> net;
    MNN::Session* session;
    int numThread = 4;

    const float meanValues[3] = {0.485 * 255, 0.456 * 255, 0.406 * 255};
    const float normValues[3] = {1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0};
};


#endif //__OCR_DBNET_H__
