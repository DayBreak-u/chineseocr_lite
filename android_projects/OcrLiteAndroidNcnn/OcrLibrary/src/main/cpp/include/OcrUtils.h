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

#define __ENABLE_CONSOLE__ false
#define Logger(format, ...) {\
  if(__ENABLE_CONSOLE__) LOGI(format,##__VA_ARGS__); \
}

double getCurrentTime();

ScaleParam getScaleParam(cv::Mat &src, const float scale);

ScaleParam getScaleParam(cv::Mat &src, const int targetSize);

std::vector<cv::Point2f> getBox(const cv::RotatedRect &rect);

int getThickness(cv::Mat &boxImg);

void drawTextBox(cv::Mat &boxImg, cv::RotatedRect &rect, int thickness);

void drawTextBox(cv::Mat &boxImg, const std::vector<cv::Point> &box, int thickness);

void drawTextBoxes(cv::Mat &boxImg, std::vector<TextBox> &textBoxes, int thickness);

cv::Mat matRotateClockWise180(cv::Mat src);

cv::Mat matRotateClockWise90(cv::Mat src);

cv::Mat getRotateCropImage(const cv::Mat &src, std::vector<cv::Point> box);

cv::Mat adjustTargetImg(cv::Mat &src, int dstWidth, int dstHeight);

std::vector<cv::Point> getMinBoxes(const std::vector<cv::Point> &inVec, float &minSideLen, float &allEdgeSize);

float boxScoreFast(const cv::Mat &inMat, const std::vector<cv::Point> &inBox);

std::vector<cv::Point> unClip(const std::vector<cv::Point> &inBox, float perimeter, float unClipRatio);

std::vector<int> getAngleIndexes(std::vector<Angle> &angles);

#endif //__OCR_UTILS_H__
