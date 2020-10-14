#pragma once

#include <opencv2/core.hpp>

struct ScaleParam {
    int srcWidth;
    int srcHeight;
    int dstWidth;
    int dstHeight;
    float scaleWidth;
    float scaleHeight;

    ScaleParam(
            int srcWidth,
            int srcHeight,
            int targetWidth,
            int targetHeight,
            float scaleWidth,
            float scaleHeight
    ) : srcWidth(srcWidth),
        srcHeight(srcHeight),
        dstWidth(targetWidth),
        dstHeight(targetHeight),
        scaleWidth(scaleWidth),
        scaleHeight(scaleHeight) {};
};

ScaleParam getScaleParam(cv::Mat &src, const float scale);

ScaleParam getScaleParam(cv::Mat &src, const int targetSize);

cv::RotatedRect getPartRect(std::vector<cv::Point> &box, float scaleWidth, float scaleHeight);

void drawTextBox(cv::Mat &boxImg, cv::RotatedRect &rect);

cv::Mat matRotateClockWise180(cv::Mat src);

cv::Mat matRotateClockWise90(cv::Mat src);

int getMiniBoxes(std::vector<cv::Point> &invec,
                 std::vector<cv::Point> &minboxvec,
                 float &minedgesize, float &alledgesize
);

float boxScoreFast(cv::Mat &mapmat, std::vector<cv::Point> &_box);

int unClip(std::vector<cv::Point> &minboxvec, float alledgesize,
           std::vector<cv::Point> &outvec, float unclip_ratio);

