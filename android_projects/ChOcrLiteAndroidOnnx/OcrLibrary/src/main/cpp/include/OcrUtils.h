#ifndef __OCR_UTILS_H__
#define __OCR_UTILS_H__

#include <opencv2/core.hpp>
#include "OcrStruct.h"

#include <android/log.h>

#define TAG "OcrLite"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

double getCurrentTime();

ScaleParam getScaleParam(cv::Mat &src, const float scale);

ScaleParam getScaleParam(cv::Mat &src, const int targetSize);

cv::RotatedRect getPartRect(std::vector<cv::Point> &box, float scaleWidth, float scaleHeight);

int getThickness(cv::Mat &boxImg);

void drawTextBox(cv::Mat &boxImg, cv::RotatedRect &rect, int thickness);

cv::Mat matRotateClockWise180(cv::Mat src);

cv::Mat matRotateClockWise90(cv::Mat src);

int getMiniBoxes(std::vector<cv::Point> &invec,
                 std::vector<cv::Point> &minboxvec,
                 float &minedgesize, float &alledgesize
);

float boxScoreFast(cv::Mat &mapmat, std::vector<cv::Point> &_box);

std::vector<float> substractMeanNormalize(cv::Mat &src, const float *meanVals, const float *normVals);

#endif //__OCR_UTILS_H__
