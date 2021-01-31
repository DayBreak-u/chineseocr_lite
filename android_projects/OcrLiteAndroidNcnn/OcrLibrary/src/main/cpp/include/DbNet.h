#ifndef __OCR_DBNET_H__
#define __OCR_DBNET_H__

#include "OcrStruct.h"
#include <net.h>
#include <opencv/cv.hpp>

class DbNet {
public:
    DbNet();

    ~DbNet();

    void setNumThread(int numOfThread);

    void setGpuIndex(int gpuIndex);

    bool initModel(AAssetManager *mgr);

    std::vector<TextBox> getTextBoxes(cv::Mat &src, ScaleParam &s, float boxScoreThresh,
                                      float boxThresh, float unClipRatio);

private:
    int numThread;
    ncnn::Net net;
    const float meanValues[3] = {0.485 * 255, 0.456 * 255, 0.406 * 255};
    const float normValues[3] = {1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0};
};


#endif //__OCR_DBNET_H__
