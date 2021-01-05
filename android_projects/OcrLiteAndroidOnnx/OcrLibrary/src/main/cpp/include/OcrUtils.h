#ifndef __OCR_UTILS_H__
#define __OCR_UTILS_H__

#include <opencv2/core.hpp>
#include "OcrStruct.h"
#include "onnx/onnxruntime_cxx_api.h"
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

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

template<typename T, typename... Ts>
static std::unique_ptr<T> makeUnique(Ts &&... params) {
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

template<typename T>
static double getMean(std::vector<T> &input) {
    auto sum = accumulate(input.begin(), input.end(), 0.0);
    return sum / input.size();
}

template<typename T>
static double getStdev(std::vector<T> &input, double mean) {
    if (input.size() <= 1) return 0;
    double accum = 0.0;
    for_each(input.begin(), input.end(), [&](const double d) {
        accum += (d - mean) * (d - mean);
    });
    double stdev = sqrt(accum / (input.size() - 1));
    return stdev;
}

double getCurrentTime();

ScaleParam getScaleParam(cv::Mat &src, const float scale);

ScaleParam getScaleParam(cv::Mat &src, const int targetSize);

cv::RotatedRect getPartRect(std::vector<cv::Point> &box, float scaleWidth, float scaleHeight);

int getThickness(cv::Mat &boxImg);

void drawTextBox(cv::Mat &boxImg, cv::RotatedRect &rect, int thickness);

void drawTextBox(cv::Mat &boxImg, const std::vector<cv::Point> &box, int thickness);

void drawTextBoxes(cv::Mat &boxImg, std::vector<TextBox> &textBoxes, int thickness);

cv::Mat matRotateClockWise180(cv::Mat src);

cv::Mat matRotateClockWise90(cv::Mat src);

cv::Mat GetRotateCropImage(const cv::Mat &src, std::vector<cv::Point> box);

cv::Mat adjustTargetImg(cv::Mat &src, int dstWidth, int dstHeight);

int getMiniBoxes(std::vector<cv::Point> &inVec,
                 std::vector<cv::Point> &minBoxVec,
                 float &minEdgeSize, float &allEdgeSize
);

float boxScoreFast(cv::Mat &mapmat, std::vector<cv::Point> &_box);

void unClip(std::vector<cv::Point> &minBoxVec, float allEdgeSize, std::vector<cv::Point> &outVec,
            float unClipRatio);

std::vector<float>
substractMeanNormalize(cv::Mat &src, const float *meanVals, const float *normVals);

std::vector<int> getAngleIndexes(std::vector<Angle> &angles);

std::vector<const char *> getInputNames(Ort::Session *session);

std::vector<const char *> getOutputNames(Ort::Session *session);

void *getModelDataFromAssets(AAssetManager *mgr, const char *modelName, int &size);

#endif //__OCR_UTILS_H__
