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
    std::vector<cv::Point> boxPoint;
    float score;

    TextBox(std::vector<cv::Point> boxPoint,
            float score) : boxPoint(boxPoint), score(score) {};
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
    std::string text;
    std::vector<float> charScores;
    double time;

    TextLine(std::string line,
             std::vector<float> scores) : text(line), charScores(scores) {};
};

struct TextBlock {
    std::vector<cv::Point> boxPoint;
    float boxScore;
    int angleIndex;
    float angleScore;
    double angleTime;
    std::string text;
    std::vector<float> charScores;
    double crnnTime;
    double blockTime;

    TextBlock(
            std::vector<cv::Point> boxPoint, float boxScore,
            int angleIndex, float angleScore, double angleTime,
            std::string text, std::vector<float> charScores, double crnnTime,
            double blockTime
    ) : boxPoint(boxPoint), boxScore(boxScore), angleIndex(angleIndex), angleScore(angleScore),
        angleTime(angleTime),
        text(text), charScores(charScores), crnnTime(crnnTime), blockTime(blockTime) {};
};

struct OcrResult {
    double dbNetTime;
    std::vector<TextBlock> textBlocks;
    cv::Mat boxImg;
    double detectTime;
    std::string strRes;

    OcrResult(
            std::vector<TextBlock> textBlocks,
            double dbNetTime,
            cv::Mat boxImg,
            double detectTime,
            std::string strRes
    ) : textBlocks(textBlocks), dbNetTime(dbNetTime),
        boxImg(boxImg), detectTime(detectTime), strRes(strRes) {};
};

#endif //__OCR_STRUCT_H__
