#ifndef __OCR_UTILS_H__
#define __OCR_UTILS_H__

#include <opencv2/core.hpp>
#include "OcrStruct.h"

/*#define __ENABLE_CONSOLE__ true
#define Logger(format, ...) {\
  if(__ENABLE_CONSOLE__) printf(format,##__VA_ARGS__); \
}*/

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

std::vector<int> getAngleIndexes(std::vector<Angle> &angles);

void saveImg(cv::Mat &img, const char *imgPath);

std::string getSrcImgFilePath(const char *path, const char *imgName);

std::string getResultTxtFilePath(const char *path, const char *imgName);

std::string getResultImgFilePath(const char *path, const char *imgName);

std::string getDebugImgFilePath(const char *path, const char *imgName, int i, const char *tag);

#endif //__OCR_UTILS_H__
