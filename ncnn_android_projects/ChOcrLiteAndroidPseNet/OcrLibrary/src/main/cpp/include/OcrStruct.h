#ifndef __OCR_LITE_OCR_STRUCT_H__
#define __OCR_LITE_OCR_STRUCT_H__

#include "opencv2/core.hpp"
#include "ncnn/net.h"
#include <vector>

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

struct TextBox {
    std::vector <cv::Point> box;
    float score;

    TextBox(std::vector <cv::Point> box,
            float score) : box(box), score(score) {};
};

struct Angle {
    int index;
    float score;

    Angle(int index,
          float score
    ) : index(index),
        score(score) {};
};

struct TextLine {
    std::string line;
    std::vector<float> scores;

    TextLine(std::string line,
             std::vector<float> scores) : line(line), scores(scores) {};
};

#endif //__OCR_LITE_OCR_STRUCT_H__
