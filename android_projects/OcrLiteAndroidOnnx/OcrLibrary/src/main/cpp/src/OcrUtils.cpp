#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include "OcrUtils.h"
#include "clipper.hpp"

double getCurrentTime() {
    return (static_cast<double>(cv::getTickCount())) / cv::getTickFrequency() * 1000;//单位毫秒
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

    float scale = 1.f;
    if (dstWidth > dstHeight) {
        scale = (float) targetSize / (float) dstWidth;
        dstWidth = targetSize;
        dstHeight = int((float) dstHeight * scale);
    } else {
        scale = (float) targetSize / (float) dstHeight;
        dstHeight = targetSize;
        dstWidth = int((float) dstWidth * scale);
    }
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

cv::RotatedRect getPartRect(std::vector<cv::Point> &box, float scaleWidth, float scaleHeight) {
    cv::RotatedRect rect = cv::minAreaRect(box);
    int minSize = rect.size.width > rect.size.height ? rect.size.height : rect.size.width;
    if (rect.size.width > rect.size.height) {
        rect.size.width = rect.size.width + (float) minSize * scaleWidth;
        rect.size.height = rect.size.height + (float) minSize * scaleHeight;
    } else {
        rect.size.width = rect.size.width + (float) minSize * scaleHeight;
        rect.size.height = rect.size.height + (float) minSize * scaleWidth;
    }
    return rect;
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

cv::Mat GetRotateCropImage(const cv::Mat &src, std::vector<cv::Point> box) {
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

bool cvPointCompare(cv::Point a, cv::Point b) {
    return a.x < b.x;
}

int
getMiniBoxes(std::vector<cv::Point> &inVec, std::vector<cv::Point> &minBoxVec, float &minEdgeSize,
             float &allEdgeSize) {
    cv::RotatedRect textRect = cv::minAreaRect(inVec);
    cv::Mat boxPoints2f;
    cv::boxPoints(textRect, boxPoints2f);

    float *p1 = (float *) boxPoints2f.data;
    std::vector<cv::Point> tmpVec;
    for (int i = 0; i < 4; ++i, p1 += 2) {
        tmpVec.emplace_back((int) (p1[0]), (int) (p1[1]));
    }

    std::sort(tmpVec.begin(), tmpVec.end(), cvPointCompare);

    minBoxVec.clear();

    int index1 = 0, index2 = 0, index3 = 0, index4 = 0;
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

    minEdgeSize = (std::min)(textRect.size.width, textRect.size.height);
    allEdgeSize = 2.f * (textRect.size.width + textRect.size.height);

    return 1;
}

float boxScoreFast(cv::Mat &mapmat, std::vector<cv::Point> &_box) {
    std::vector<cv::Point> box = _box;
    int wid = mapmat.cols;
    int hi = mapmat.rows;
    int xmax = -1, xmin = 1000000, ymax = -1, ymin = 1000000;
    for (int i = 0; i < box.size(); ++i) {
        if (xmax < box[i].x)
            xmax = box[i].x;
        if (xmin > box[i].x)
            xmin = box[i].x;
        if (ymax < box[i].y)
            ymax = box[i].y;
        if (ymin > box[i].y)
            ymin = box[i].y;
    }
    xmax = (std::min)((std::max)(xmax, 0), wid - 1);
    xmin = (std::max)((std::min)(xmin, wid - 1), 0);
    ymax = (std::min)((std::max)(ymax, 0), hi - 1);
    ymin = (std::max)((std::min)(ymin, hi - 1), 0);

    for (int i = 0; i < box.size(); ++i) {
        box[i].x = box[i].x - xmin;
        box[i].y = box[i].y - ymin;
    }

    std::vector<std::vector<cv::Point>> tmpbox;
    tmpbox.push_back(box);
    cv::Mat maskmat(ymax - ymin + 1, xmax - xmin + 1, CV_8UC1, cv::Scalar(0, 0, 0));
    cv::fillPoly(maskmat, tmpbox, cv::Scalar(1, 1, 1), 1);

    // 	cv::Mat normat;
    // 	cv::normalize(maskmat, normat, 255, 0, cv::NORM_MINMAX);
    //
    // 	cv::Mat maskbinmat;
    // 	normat.convertTo(maskbinmat, CV_8UC1, 1);
    // 	imwrite("subbin.jpg", maskbinmat);

    //std::cout << mapmat << std::endl;

    return cv::mean(mapmat(cv::Rect(cv::Point(xmin, ymin), cv::Point(xmax + 1, ymax + 1))).clone(),
                    maskmat).val[0];

}

// use clipper
void unClip(std::vector<cv::Point> &minBoxVec, float allEdgeSize, std::vector<cv::Point> &outVec, float unClipRatio) {
    ClipperLib::Path poly;

    for (int i = 0; i < minBoxVec.size(); ++i) {
        poly.push_back(ClipperLib::IntPoint(minBoxVec[i].x, minBoxVec[i].y));
    }

    double distance = unClipRatio * ClipperLib::Area(poly) / (double) allEdgeSize;

    ClipperLib::ClipperOffset clipperOffset;
    clipperOffset.AddPath(poly, ClipperLib::JoinType::jtRound, ClipperLib::EndType::etClosedPolygon);
    ClipperLib::Paths polys;
    polys.push_back(poly);
    clipperOffset.Execute(polys, distance);

    outVec.clear();
    std::vector<cv::Point> rsVec;
    for (int i = 0; i < polys.size(); ++i) {
        ClipperLib::Path tmpPoly = polys[i];
        for (int j = 0; j < tmpPoly.size(); ++j) {
            outVec.emplace_back(tmpPoly[j].X, tmpPoly[j].Y);
        }
    }
}

std::vector<float> substractMeanNormalize(cv::Mat &src, const float *meanVals, const float *normVals) {
    auto inputTensorSize = src.cols * src.rows * src.channels();
    std::vector<float> inputTensorValues(inputTensorSize);
    size_t numChannels = src.channels();
    size_t imageSize = src.cols * src.rows;

    for (size_t pid = 0; pid < imageSize; pid++) {
        for (size_t ch = 0; ch < numChannels; ++ch) {
            float data = (float) (src.data[pid * numChannels + ch] * normVals[ch] - meanVals[ch] * normVals[ch]);
            inputTensorValues[ch * imageSize + pid] = data;
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

std::vector<const char *> getInputNames(Ort::Session *session) {
    Ort::AllocatorWithDefaultOptions allocator;
    size_t numInputNodes = session->GetInputCount();
    std::vector<const char *> inputNodeNames(numInputNodes);
    //std::vector<int64_t> inputNodeDims;

    //printf("Number of inputs = %zu\n", numInputNodes);

    for (int i = 0; i < numInputNodes; i++) {
        // print input node names
        const char *inputName = session->GetInputName(i, allocator);
        //printf("Input %d : name=%s\n", i, inputName);
        inputNodeNames[i] = inputName;

        // print input node types
        //Ort::TypeInfo typeInfo = session->GetInputTypeInfo(i);
        //auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();

        //ONNXTensorElementDataType type = tensorInfo.GetElementType();
        //printf("Input %d : type=%d\n", i, type);

        // print input shapes/dims
        //inputNodeDims = tensorInfo.GetShape();
        //printf("Input %d : num_dims=%zu\n", i, inputNodeDims.size());
        /*for (int j = 0; j < inputNodeDims.size(); j++)
            printf("Input %d : dim %d=%jd\n", i, j, inputNodeDims[j]);*/
    }
    return inputNodeNames;
}

std::vector<const char *> getOutputNames(Ort::Session *session) {
    Ort::AllocatorWithDefaultOptions allocator;
    size_t numOutputNodes = session->GetOutputCount();
    std::vector<const char *> outputNodeNames(numOutputNodes);
    //std::vector<int64_t> outputNodeDims;

    //printf("Number of outputs = %zu\n", numOutputNodes);

    for (int i = 0; i < numOutputNodes; i++) {
        // print input node names
        const char *outputName = session->GetOutputName(i, allocator);
        //printf("Output %d : name=%s\n", i, outputName);
        outputNodeNames[i] = outputName;

        // print input node types
        //Ort::TypeInfo type_info = session->GetOutputTypeInfo(i);
        //auto tensorInfo = type_info.GetTensorTypeAndShapeInfo();

        //ONNXTensorElementDataType type = tensorInfo.GetElementType();
        //printf("Output %d : type=%d\n", i, type);

        // print input shapes/dims
        //outputNodeDims = tensorInfo.GetShape();
        //printf("Output %d : num_dims=%zu\n", i, outputNodeDims.size());
        /*for (int j = 0; j < outputNodeDims.size(); j++)
            printf("Output %d : dim %d=%jd\n", i, j, outputNodeDims[j]);*/
    }
    return outputNodeNames;
}

void *getModelDataFromAssets(AAssetManager *mgr, const char *modelName, int &size) {
    if (mgr == NULL) {
        LOGE(" %s", "AAssetManager==NULL");
        return NULL;
    }
    AAsset *asset = AAssetManager_open(mgr, modelName, AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
        LOGE(" %s", "asset==NULL");
        return NULL;
    }
    off_t bufferSize = AAsset_getLength(asset);
    void *modelData = malloc(bufferSize + 1);
    size = AAsset_read(asset, modelData, bufferSize);
    AAsset_close(asset);
    LOGI("model=%s, numBytesRead=%d", modelName, size);
    return modelData;
}