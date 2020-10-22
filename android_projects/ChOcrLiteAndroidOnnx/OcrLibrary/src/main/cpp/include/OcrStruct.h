#ifndef __OCR_STRUCT_H__
#define __OCR_STRUCT_H__

#include "opencv2/core.hpp"
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
    std::vector<cv::Point> box;
    float score;

    TextBox(std::vector<cv::Point> box,
            float score) : box(box), score(score) {};
};

struct Angle {
    int index;
    float score;
    double time;

    Angle(int index,
          float score
    ) : index(index),
        score(score) {};
};

struct TextLine {
    std::string line;
    std::vector<float> scores;
    double time;

    TextLine(std::string line,
             std::vector<float> scores) : line(line), scores(scores) {};
};

struct OcrResult {
    std::vector<TextBox> textBoxes;
    double textBoxesTime;
    std::vector<Angle> angles;
    std::vector<TextLine> lines;
    cv::Mat textBoxImg;
    std::string strRes;
    double fullTime = 0.f;

    OcrResult(
            std::vector<TextBox> textBoxes,
            double textBoxesTime,
            std::vector<Angle> angles,
            std::vector<TextLine> lines,
            cv::Mat textBoxImg,
            std::string strRes,
            double fullTime
    ) : textBoxes(textBoxes), textBoxesTime(textBoxesTime), angles(angles),
        lines(lines), textBoxImg(textBoxImg), strRes(strRes), fullTime(fullTime) {};
};

#endif //__OCR_STRUCT_H__
