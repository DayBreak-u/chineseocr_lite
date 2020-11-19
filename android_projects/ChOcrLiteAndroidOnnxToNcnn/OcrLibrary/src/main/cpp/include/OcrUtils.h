#ifndef __OCR_UTILS_H__
#define __OCR_UTILS_H__

#include <opencv2/core.hpp>
#include "OcrStruct.h"
#include <android/log.h>

using namespace cv;
using namespace std;

#define TAG "OcrLite"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

double getCurrentTime();

ScaleParam getScaleParam(Mat &src, const float scale);

ScaleParam getScaleParam(Mat &src, const int targetSize);

RotatedRect getPartRect(vector<Point> &box, float scaleWidth, float scaleHeight);

int getThickness(Mat &boxImg);

void drawTextBox(Mat &boxImg, RotatedRect &rect, int thickness);

void drawTextBox(Mat &boxImg, const vector<Point> &box, int thickness);

Mat matRotateClockWise180(Mat src);

Mat matRotateClockWise90(Mat src);

Mat GetRotateCropImage(const Mat &src, vector<Point> box);

Mat adjustTargetImg(Mat &src, int dstWidth, int dstHeight);

int getMiniBoxes(vector<Point> &inVec,
                 vector<Point> &minBoxVec,
                 float &minEdgeSize, float &allEdgeSize
);

float boxScoreFast(Mat &mapmat, vector<Point> &_box);

void unClip(vector<Point> &minBoxVec, float allEdgeSize,
            vector<Point> &outVec, float unClipRatio);

vector<int> getAngleIndexes(vector<Angle> &angles);

#endif //__OCR_UTILS_H__
