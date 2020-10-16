#pragma once

#include <opencv2/core.hpp>
#include "OcrStruct.h"

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

