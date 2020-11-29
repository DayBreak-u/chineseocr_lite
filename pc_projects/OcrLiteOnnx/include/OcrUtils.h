#ifndef __OCR_UTILS_H__
#define __OCR_UTILS_H__

#include <opencv2/core.hpp>
#include "OcrStruct.h"
#include "onnx/onnxruntime_cxx_api.h"
#include <numeric>

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

std::wstring strToWstr(std::string str);

ScaleParam getScaleParam(cv::Mat &src, const float scale);

ScaleParam getScaleParam(cv::Mat &src, const int targetSize);

cv::RotatedRect getPartRect(std::vector<cv::Point> &box, float scaleWidth, float scaleHeight);

std::vector<cv::Point2f> getBox(cv::RotatedRect &rect);

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

void unClip(std::vector<cv::Point> &minBoxVec, float allEdgeSize, std::vector<cv::Point> &outVec, float unClipRatio);

std::vector<float> substractMeanNormalize(cv::Mat &src, const float *meanVals, const float *normVals);

std::vector<int> getAngleIndexes(std::vector<Angle> &angles);

std::vector<const char *> getInputNames(std::unique_ptr<Ort::Session> &session);

std::vector<const char *> getOutputNames(std::unique_ptr<Ort::Session> &session);

int getMostProbabilityAngleIndex(std::vector<int> &input, double mean, double stdev);

void saveImg(cv::Mat &img, const char *imgPath);

std::string getSrcImgFilePath(const char *path, const char *imgName);

std::string getResultTxtFilePath(const char *path, const char *imgName);

std::string getResultImgFilePath(const char *path, const char *imgName);

std::string getDebugImgFilePath(const char *path, const char *imgName, int i, const char *tag);

#endif //__OCR_UTILS_H__
