#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <numeric>
#include "OcrUtils.h"
#include "clipper.hpp"

double getCurrentTime() {
    return (static_cast<double>(cv::getTickCount())) / cv::getTickFrequency() * 1000;//单位毫秒
}

std::wstring strToWstr(std::string str) {
    if (str.length() == 0)
        return L"";
    std::wstring wstr;
    wstr.assign(str.begin(), str.end());
    return wstr;
}

ScaleParam getScaleParam(cv::Mat &src, const float scale) {
    int srcWidth = src.cols;
    int srcHeight = src.rows;
    int dstWidth = int((float) srcWidth * scale);
    int dstHeight = int((float) srcHeight * scale);
    if (dstWidth % 32 != 0) {
        dstWidth = (dstWidth / 32 - 1) * 32;
        dstWidth = (std::max)(dstWidth, 32);
    }
    if (dstHeight % 32 != 0) {
        dstHeight = (dstHeight / 32 - 1) * 32;
        dstHeight = (std::max)(dstHeight, 32);
    }
    float scaleWidth = (float) dstWidth / (float) srcWidth;
    float scaleHeight = (float) dstHeight / (float) srcHeight;
    return {srcWidth, srcHeight, dstWidth, dstHeight, scaleWidth, scaleHeight};
}

ScaleParam getScaleParam(cv::Mat &src, const int targetSize) {
    int srcWidth, srcHeight, dstWidth, dstHeight;
    srcWidth = dstWidth = src.cols;
    srcHeight = dstHeight = src.rows;

    float ratio = 1.f;
    if (srcWidth > srcHeight) {
        ratio = float(targetSize) / float(srcWidth);
    } else {
        ratio = float(targetSize) / float(srcHeight);
    }
    dstWidth = int(float(srcWidth) * ratio);
    dstHeight = int(float(srcHeight) * ratio);
    if (dstWidth % 32 != 0) {
        dstWidth = (dstWidth / 32) * 32;
        dstWidth = (std::max)(dstWidth, 32);
    }
    if (dstHeight % 32 != 0) {
        dstHeight = (dstHeight / 32) * 32;
        dstHeight = (std::max)(dstHeight, 32);
    }
    float ratioWidth = (float) dstWidth / (float) srcWidth;
    float ratioHeight = (float) dstHeight / (float) srcHeight;
    return {srcWidth, srcHeight, dstWidth, dstHeight, ratioWidth, ratioHeight};
}

std::vector<cv::Point2f> getBox(const cv::RotatedRect &rect) {
    cv::Point2f vertices[4];
    rect.points(vertices);
    //std::vector<cv::Point2f> ret(4);
    std::vector<cv::Point2f> ret2(vertices, vertices + sizeof(vertices) / sizeof(vertices[0]));
    //memcpy(vertices, &ret[0], ret.size() * sizeof(ret[0]));
    return ret2;
}

int getThickness(cv::Mat &boxImg) {
    int minSize = boxImg.cols > boxImg.rows ? boxImg.rows : boxImg.cols;
    int thickness = minSize / 1000 + 2;
    return thickness;
}

void drawTextBox(cv::Mat &boxImg, cv::RotatedRect &rect, int thickness) {
    cv::Point2f vertices[4];
    rect.points(vertices);
    for (int i = 0; i < 4; i++)
        cv::line(boxImg, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 0, 255), thickness);
    //cv::polylines(srcmat, textpoint, true, cv::Scalar(0, 255, 0), 2);
}

void drawTextBox(cv::Mat &boxImg, const std::vector<cv::Point> &box, int thickness) {
    auto color = cv::Scalar(255, 0, 0);// R(255) G(0) B(0)
    cv::line(boxImg, box[0], box[1], color, thickness);
    cv::line(boxImg, box[1], box[2], color, thickness);
    cv::line(boxImg, box[2], box[3], color, thickness);
    cv::line(boxImg, box[3], box[0], color, thickness);
}

void drawTextBoxes(cv::Mat &boxImg, std::vector<TextBox> &textBoxes, int thickness) {
    for (int i = 0; i < textBoxes.size(); ++i) {
        drawTextBox(boxImg, textBoxes[i].boxPoint, thickness);
    }
}

cv::Mat matRotateClockWise180(cv::Mat src) {
    flip(src, src, 0);
    flip(src, src, 1);
    return src;
}

cv::Mat matRotateClockWise90(cv::Mat src) {
    transpose(src, src);
    flip(src, src, 1);
    return src;
}

cv::Mat getRotateCropImage(const cv::Mat &src, std::vector<cv::Point> box) {
    cv::Mat image;
    src.copyTo(image);
    std::vector<cv::Point> points = box;

    int collectX[4] = {box[0].x, box[1].x, box[2].x, box[3].x};
    int collectY[4] = {box[0].y, box[1].y, box[2].y, box[3].y};
    int left = int(*std::min_element(collectX, collectX + 4));
    int right = int(*std::max_element(collectX, collectX + 4));
    int top = int(*std::min_element(collectY, collectY + 4));
    int bottom = int(*std::max_element(collectY, collectY + 4));

    cv::Mat imgCrop;
    image(cv::Rect(left, top, right - left, bottom - top)).copyTo(imgCrop);

    for (int i = 0; i < points.size(); i++) {
        points[i].x -= left;
        points[i].y -= top;
    }

    int imgCropWidth = int(sqrt(pow(points[0].x - points[1].x, 2) +
                                pow(points[0].y - points[1].y, 2)));
    int imgCropHeight = int(sqrt(pow(points[0].x - points[3].x, 2) +
                                 pow(points[0].y - points[3].y, 2)));

    cv::Point2f ptsDst[4];
    ptsDst[0] = cv::Point2f(0., 0.);
    ptsDst[1] = cv::Point2f(imgCropWidth, 0.);
    ptsDst[2] = cv::Point2f(imgCropWidth, imgCropHeight);
    ptsDst[3] = cv::Point2f(0.f, imgCropHeight);

    cv::Point2f ptsSrc[4];
    ptsSrc[0] = cv::Point2f(points[0].x, points[0].y);
    ptsSrc[1] = cv::Point2f(points[1].x, points[1].y);
    ptsSrc[2] = cv::Point2f(points[2].x, points[2].y);
    ptsSrc[3] = cv::Point2f(points[3].x, points[3].y);

    cv::Mat M = cv::getPerspectiveTransform(ptsSrc, ptsDst);

    cv::Mat partImg;
    cv::warpPerspective(imgCrop, partImg, M,
                        cv::Size(imgCropWidth, imgCropHeight),
                        cv::BORDER_REPLICATE);

    if (float(partImg.rows) >= float(partImg.cols) * 1.5) {
        cv::Mat srcCopy = cv::Mat(partImg.rows, partImg.cols, partImg.depth());
        cv::transpose(partImg, srcCopy);
        cv::flip(srcCopy, srcCopy, 0);
        return srcCopy;
    } else {
        return partImg;
    }
}

cv::Mat adjustTargetImg(cv::Mat &src, int dstWidth, int dstHeight) {
    cv::Mat srcResize;
    float scale = (float) dstHeight / (float) src.rows;
    int angleWidth = int((float) src.cols * scale);
    cv::resize(src, srcResize, cv::Size(angleWidth, dstHeight));
    cv::Mat srcFit = cv::Mat(dstHeight, dstWidth, CV_8UC3, cv::Scalar(255, 255, 255));
    if (angleWidth < dstWidth) {
        cv::Rect rect(0, 0, srcResize.cols, srcResize.rows);
        srcResize.copyTo(srcFit(rect));
    } else {
        cv::Rect rect(0, 0, dstWidth, dstHeight);
        srcResize(rect).copyTo(srcFit);
    }
    return srcFit;
}

bool cvPointCompare(const cv::Point &a, const cv::Point &b) {
    return a.x < b.x;
}

std::vector<cv::Point> getMinBoxes(const std::vector<cv::Point> &inVec, float &minSideLen, float &allEdgeSize) {
    std::vector<cv::Point> minBoxVec;
    cv::RotatedRect textRect = cv::minAreaRect(inVec);
    cv::Mat boxPoints2f;
    cv::boxPoints(textRect, boxPoints2f);

    float *p1 = (float *) boxPoints2f.data;
    std::vector<cv::Point> tmpVec;
    for (int i = 0; i < 4; ++i, p1 += 2) {
        tmpVec.emplace_back(int(p1[0]), int(p1[1]));
    }

    std::sort(tmpVec.begin(), tmpVec.end(), cvPointCompare);

    minBoxVec.clear();

    int index1, index2, index3, index4;
    if (tmpVec[1].y > tmpVec[0].y) {
        index1 = 0;
        index4 = 1;
    } else {
        index1 = 1;
        index4 = 0;
    }

    if (tmpVec[3].y > tmpVec[2].y) {
        index2 = 2;
        index3 = 3;
    } else {
        index2 = 3;
        index3 = 2;
    }

    minBoxVec.clear();

    minBoxVec.push_back(tmpVec[index1]);
    minBoxVec.push_back(tmpVec[index2]);
    minBoxVec.push_back(tmpVec[index3]);
    minBoxVec.push_back(tmpVec[index4]);

    minSideLen = (std::min)(textRect.size.width, textRect.size.height);
    allEdgeSize = 2.f * (textRect.size.width + textRect.size.height);

    return minBoxVec;
}

float boxScoreFast(const cv::Mat &inMat, const std::vector<cv::Point> &inBox) {
    std::vector<cv::Point> box = inBox;
    int width = inMat.cols;
    int height = inMat.rows;
    int maxX = -1, minX = 1000000, maxY = -1, minY = 1000000;
    for (int i = 0; i < box.size(); ++i) {
        if (maxX < box[i].x)
            maxX = box[i].x;
        if (minX > box[i].x)
            minX = box[i].x;
        if (maxY < box[i].y)
            maxY = box[i].y;
        if (minY > box[i].y)
            minY = box[i].y;
    }
    maxX = (std::min)((std::max)(maxX, 0), width - 1);
    minX = (std::max)((std::min)(minX, width - 1), 0);
    maxY = (std::min)((std::max)(maxY, 0), height - 1);
    minY = (std::max)((std::min)(minY, height - 1), 0);

    for (int i = 0; i < box.size(); ++i) {
        box[i].x = box[i].x - minX;
        box[i].y = box[i].y - minY;
    }

    std::vector<std::vector<cv::Point>> maskBox;
    maskBox.push_back(box);
    cv::Mat maskMat(maxY - minY + 1, maxX - minX + 1, CV_8UC1, cv::Scalar(0, 0, 0));
    cv::fillPoly(maskMat, maskBox, cv::Scalar(1, 1, 1), 1);

    // 	cv::Mat normat;
    // 	cv::normalize(maskMat, normat, 255, 0, cv::NORM_MINMAX);
    //
    // 	cv::Mat maskbinmat;
    // 	normat.convertTo(maskbinmat, CV_8UC1, 1);
    // 	imwrite("subbin.jpg", maskbinmat);

    //std::cout << inMat << std::endl;

    return cv::mean(inMat(cv::Rect(cv::Point(minX, minY), cv::Point(maxX + 1, maxY + 1))).clone(),
                    maskMat).val[0];
}

// use clipper
std::vector<cv::Point> unClip(const std::vector<cv::Point> &inBox, float perimeter, float unClipRatio) {
    std::vector<cv::Point> outBox;
    ClipperLib::Path poly;

    for (int i = 0; i < inBox.size(); ++i) {
        poly.push_back(ClipperLib::IntPoint(inBox[i].x, inBox[i].y));
    }

    double distance = unClipRatio * ClipperLib::Area(poly) / (double) perimeter;

    ClipperLib::ClipperOffset clipperOffset;
    clipperOffset.AddPath(poly, ClipperLib::JoinType::jtRound, ClipperLib::EndType::etClosedPolygon);
    ClipperLib::Paths polys;
    polys.push_back(poly);
    clipperOffset.Execute(polys, distance);

    outBox.clear();
    std::vector<cv::Point> rsVec;
    for (int i = 0; i < polys.size(); ++i) {
        ClipperLib::Path tmpPoly = polys[i];
        for (int j = 0; j < tmpPoly.size(); ++j) {
            outBox.emplace_back(tmpPoly[j].X, tmpPoly[j].Y);
        }
    }
    return outBox;
}

std::vector<float> substractMeanNormalize(cv::Mat &src, const float *meanVals, const float *normVals) {
    auto inputTensorSize = src.cols * src.rows * src.channels();
    std::vector<float> inputTensorValues(inputTensorSize);
    size_t numChannels = src.channels();
    size_t imageSize = src.cols * src.rows;
    for (size_t pid = 0; pid < imageSize; pid++) {
        for (size_t ch = 0; ch < numChannels; ++ch) {
            float data = (float) (src.data[pid * numChannels + ch] * normVals[ch] - meanVals[ch] * normVals[ch]);
            inputTensorValues[ch * imageSize + pid] = data;  // 这个赋值方式，相当于从nhwc-> nchw?
        }
    }
    return inputTensorValues;
}


std::vector<int> getAngleIndexes(std::vector<Angle> &angles) {
    std::vector<int> angleIndexes;
    angleIndexes.reserve(angles.size());
    for (int i = 0; i < angles.size(); ++i) {
        angleIndexes.push_back(angles[i].index);
    }
    return angleIndexes;
}

void saveImg(cv::Mat &img, const char *imgPath) {
    cv::imwrite(imgPath, img);
}

std::string getSrcImgFilePath(const char *path, const char *imgName) {
    std::string filePath;
    filePath.append(path).append(imgName);
    return filePath;
}

std::string getResultTxtFilePath(const char *path, const char *imgName) {
    std::string filePath;
    filePath.append(path).append(imgName).append("-result.txt");
    return filePath;
}

std::string getResultImgFilePath(const char *path, const char *imgName) {
    std::string filePath;
    filePath.append(path).append(imgName).append("-result.jpg");
    return filePath;
}

std::string getDebugImgFilePath(const char *path, const char *imgName, int i, const char *tag) {
    std::string filePath;
    filePath.append(path).append(imgName).append(tag).append(std::to_string(i)).append(".jpg");
    return filePath;
}