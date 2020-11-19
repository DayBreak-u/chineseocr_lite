#ifndef __OCR_UTILS_H__
#define __OCR_UTILS_H__

#include <opencv2/core.hpp>
#include "OcrStruct.h"
#include "onnx/onnxruntime_cxx_api.h"
#include <numeric>

using namespace cv;
using namespace std;

template<typename T, typename... Ts>
static unique_ptr<T> makeUnique(Ts &&... params) {
    return unique_ptr<T>(new T(forward<Ts>(params)...));
}

template<typename T>
static double getMean(vector<T> &input) {
    auto sum = accumulate(input.begin(), input.end(), 0.0);
    return sum / input.size();
}

template<typename T>
static double getStdev(vector<T> &input, double mean) {
    if (input.size() <= 1) return 0;
    double accum = 0.0;
    for_each(input.begin(), input.end(), [&](const double d) {
        accum += (d - mean) * (d - mean);
    });
    double stdev = sqrt(accum / (input.size() - 1));
    return stdev;
}

double getCurrentTime();

wstring strToWstr(string str);

ScaleParam getScaleParam(Mat &src, const float scale);

ScaleParam getScaleParam(Mat &src, const int targetSize);

RotatedRect getPartRect(vector<Point> &box, float scaleWidth, float scaleHeight);

vector<Point2f> getBox(RotatedRect &rect);

int getThickness(Mat &boxImg);

void drawTextBox(Mat &boxImg, RotatedRect &rect, int thickness);

void drawTextBox(Mat &boxImg, const vector<Point> &box, int thickness);

void drawTextBoxes(Mat &boxImg, vector<TextBox> &textBoxes, int thickness);

Mat matRotateClockWise180(Mat src);

Mat matRotateClockWise90(Mat src);

Mat GetRotateCropImage(const Mat &src, vector<Point> box);

Mat adjustTargetImg(Mat &src, int dstWidth, int dstHeight);

int getMiniBoxes(vector<Point> &inVec,
                 vector<Point> &minBoxVec,
                 float &minEdgeSize, float &allEdgeSize
);

float boxScoreFast(Mat &mapmat, vector<Point> &_box);

void unClip(vector<Point> &minBoxVec, float allEdgeSize, vector<Point> &outVec, float unClipRatio);

vector<float> substractMeanNormalize(Mat &src, const float *meanVals, const float *normVals);

vector<int> getAngleIndexes(vector<Angle> &angles);

vector<const char *> getInputNames(unique_ptr<Ort::Session> &session);

vector<const char *> getOutputNames(unique_ptr<Ort::Session> &session);

int getMostProbabilityAngleIndex(vector<int> &input, double mean, double stdev);

void saveImg(Mat &img, const char *imgPath);

string getSrcImgFilePath(const char *path, const char *imgName);

string getResultTxtFilePath(const char *path, const char *imgName);

string getResultImgFilePath(const char *path, const char *imgName);

string getDebugImgFilePath(const char *path, const char *imgName, int i, const char *tag);

#endif //__OCR_UTILS_H__
