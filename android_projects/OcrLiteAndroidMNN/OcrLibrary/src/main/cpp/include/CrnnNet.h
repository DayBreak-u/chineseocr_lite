#ifndef __OCR_CRNNNET_H__
#define __OCR_CRNNNET_H__

#include "OcrStruct.h"
#include <opencv/cv.hpp>
#include <Interpreter.hpp>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

class CrnnNet {
public:

    CrnnNet();

    ~CrnnNet();

    void setNumThread(int numOfThread);

    bool initModel(AAssetManager *mgr,const char* crnnpath);

    std::vector<TextLine> getTextLines(std::vector<cv::Mat> &partImg);

private:
    std::shared_ptr<MNN::Interpreter> net;
    MNN::Session* session;
    int numThread = 4;

    const float meanValues[3] = {127.5, 127.5, 127.5};
    const float normValues[3] = {1.0 / 127.5, 1.0 / 127.5, 1.0 / 127.5};
    const int dstHeight = 32;
    const int crnnCols = 5531;

    std::vector<std::string> keys;

    TextLine scoreToTextLine(const float *srcData, int h, int w);

    TextLine getTextLine(cv::Mat &src);
};


#endif //__OCR_CRNNNET_H__
