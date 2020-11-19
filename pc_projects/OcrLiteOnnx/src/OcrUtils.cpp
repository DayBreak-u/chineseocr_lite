#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <numeric>
#include "OcrUtils.h"
#include "clipper.hpp"

double getCurrentTime() {
    return (static_cast<double>(getTickCount())) / getTickFrequency() * 1000;//单位毫秒
}

//onnxruntime init windows
wstring strToWstr(string str) {
    if (str.length() == 0)
        return L"";
    wstring wstr;
    wstr.assign(str.begin(), str.end());
    return wstr;
}

ScaleParam getScaleParam(Mat &src, const float scale) {
    int srcWidth = src.cols;
    int srcHeight = src.rows;
    int dstWidth = int((float) srcWidth * scale);
    int dstHeight = int((float) srcHeight * scale);
    if (dstWidth % 32 != 0) {
        dstWidth = (dstWidth / 32 - 1) * 32;
        dstWidth = (max)(dstWidth, 32);
    }
    if (dstHeight % 32 != 0) {
        dstHeight = (dstHeight / 32 - 1) * 32;
        dstHeight = (max)(dstHeight, 32);
    }
    float scaleWidth = (float) dstWidth / (float) srcWidth;
    float scaleHeight = (float) dstHeight / (float) srcHeight;
    return {srcWidth, srcHeight, dstWidth, dstHeight, scaleWidth, scaleHeight};
}

ScaleParam getScaleParam(Mat &src, const int targetSize) {
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
        dstWidth = (max)(dstWidth, 32);
    }
    if (dstHeight % 32 != 0) {
        dstHeight = (dstHeight / 32 - 1) * 32;
        dstHeight = (max)(dstHeight, 32);
    }
    float scaleWidth = (float) dstWidth / (float) srcWidth;
    float scaleHeight = (float) dstHeight / (float) srcHeight;
    return {srcWidth, srcHeight, dstWidth, dstHeight, scaleWidth, scaleHeight};
}

RotatedRect getPartRect(vector<Point> &box, float scaleWidth, float scaleHeight) {
    RotatedRect rect = minAreaRect(box);
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

vector<Point2f> getBox(RotatedRect &rect) {
    Point2f vertices[4];
    rect.points(vertices);
    //vector<Point2f> ret(4);
    vector<Point2f> ret2(vertices, vertices + sizeof(vertices) / sizeof(vertices[0]));
    //memcpy(vertices, &ret[0], ret.size() * sizeof(ret[0]));
    return ret2;
}

int getThickness(Mat &boxImg) {
    int minSize = boxImg.cols > boxImg.rows ? boxImg.rows : boxImg.cols;
    int thickness = minSize / 1000 + 2;
    return thickness;
}

void drawTextBox(Mat &boxImg, RotatedRect &rect, int thickness) {
    Point2f vertices[4];
    rect.points(vertices);
    for (int i = 0; i < 4; i++)
        line(boxImg, vertices[i], vertices[(i + 1) % 4], Scalar(0, 0, 255), thickness);
    //polylines(srcmat, textpoint, true, Scalar(0, 255, 0), 2);
}

void drawTextBox(Mat &boxImg, const vector<Point> &box, int thickness) {
    auto color = Scalar(255, 0, 0);// R(255) G(0) B(0)
    line(boxImg, box[0], box[1], color, thickness);
    line(boxImg, box[1], box[2], color, thickness);
    line(boxImg, box[2], box[3], color, thickness);
    line(boxImg, box[3], box[0], color, thickness);
}

void drawTextBoxes(Mat &boxImg, vector<TextBox> &textBoxes, int thickness) {
    for (int i = 0; i < textBoxes.size(); ++i) {
        drawTextBox(boxImg, textBoxes[i].boxPoint, thickness);
    }
}

Mat matRotateClockWise180(Mat src) {
    flip(src, src, 0);
    flip(src, src, 1);
    return src;
}

Mat matRotateClockWise90(Mat src) {
    transpose(src, src);
    flip(src, src, 1);
    return src;
}

Mat GetRotateCropImage(const Mat &src, vector<Point> box) {
    Mat image;
    src.copyTo(image);
    vector<Point> points = box;

    int collectX[4] = {box[0].x, box[1].x, box[2].x, box[3].x};
    int collectY[4] = {box[0].y, box[1].y, box[2].y, box[3].y};
    int left = int(*min_element(collectX, collectX + 4));
    int right = int(*max_element(collectX, collectX + 4));
    int top = int(*min_element(collectY, collectY + 4));
    int bottom = int(*max_element(collectY, collectY + 4));

    Mat imgCrop;
    image(Rect(left, top, right - left, bottom - top)).copyTo(imgCrop);

    for (int i = 0; i < points.size(); i++) {
        points[i].x -= left;
        points[i].y -= top;
    }

    int imgCropWidth = int(sqrt(pow(points[0].x - points[1].x, 2) +
                                pow(points[0].y - points[1].y, 2)));
    int imgCropHeight = int(sqrt(pow(points[0].x - points[3].x, 2) +
                                 pow(points[0].y - points[3].y, 2)));

    Point2f ptsDst[4];
    ptsDst[0] = Point2f(0., 0.);
    ptsDst[1] = Point2f(imgCropWidth, 0.);
    ptsDst[2] = Point2f(imgCropWidth, imgCropHeight);
    ptsDst[3] = Point2f(0.f, imgCropHeight);

    Point2f ptsSrc[4];
    ptsSrc[0] = Point2f(points[0].x, points[0].y);
    ptsSrc[1] = Point2f(points[1].x, points[1].y);
    ptsSrc[2] = Point2f(points[2].x, points[2].y);
    ptsSrc[3] = Point2f(points[3].x, points[3].y);

    Mat M = getPerspectiveTransform(ptsSrc, ptsDst);

    Mat partImg;
    warpPerspective(imgCrop, partImg, M,
                        Size(imgCropWidth, imgCropHeight),
                        BORDER_REPLICATE);

    if (float(partImg.rows) >= float(partImg.cols) * 1.5) {
        Mat srcCopy = Mat(partImg.rows, partImg.cols, partImg.depth());
        transpose(partImg, srcCopy);
        flip(srcCopy, srcCopy, 0);
        return srcCopy;
    } else {
        return partImg;
    }
}

Mat adjustTargetImg(Mat &src, int dstWidth, int dstHeight) {
    Mat srcResize;
    float scale = (float) dstHeight / (float) src.rows;
    int angleWidth = int((float) src.cols * scale);
    resize(src, srcResize, Size(angleWidth, dstHeight));
    Mat srcFit = Mat(dstHeight, dstWidth, CV_8UC3, Scalar(255, 255, 255));
    if (angleWidth < dstWidth) {
        Rect rect(0, 0, srcResize.cols, srcResize.rows);
        srcResize.copyTo(srcFit(rect));
    } else {
        Rect rect(0, 0, dstWidth, dstHeight);
        srcResize(rect).copyTo(srcFit);
    }
    return srcFit;
}

bool cvPointCompare(Point a, Point b) {
    return a.x < b.x;
}

int
getMiniBoxes(vector<Point> &inVec, vector<Point> &minBoxVec, float &minEdgeSize,
             float &allEdgeSize) {
    RotatedRect textRect = minAreaRect(inVec);
    Mat boxPoints2f;
    boxPoints(textRect, boxPoints2f);

    float *p1 = (float *) boxPoints2f.data;
    vector<Point> tmpVec;
    for (int i = 0; i < 4; ++i, p1 += 2) {
        tmpVec.emplace_back((int) (p1[0]), (int) (p1[1]));
    }

    sort(tmpVec.begin(), tmpVec.end(), cvPointCompare);

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

    minEdgeSize = (min)(textRect.size.width, textRect.size.height);
    allEdgeSize = 2.f * (textRect.size.width + textRect.size.height);

    return 1;
}

float boxScoreFast(Mat &mapmat, vector<Point> &_box) {
    vector<Point> box = _box;
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
    xmax = (min)((max)(xmax, 0), wid - 1);
    xmin = (max)((min)(xmin, wid - 1), 0);
    ymax = (min)((max)(ymax, 0), hi - 1);
    ymin = (max)((min)(ymin, hi - 1), 0);

    for (int i = 0; i < box.size(); ++i) {
        box[i].x = box[i].x - xmin;
        box[i].y = box[i].y - ymin;
    }

    vector<vector<Point>> tmpbox;
    tmpbox.push_back(box);
    Mat maskmat(ymax - ymin + 1, xmax - xmin + 1, CV_8UC1, Scalar(0, 0, 0));
    fillPoly(maskmat, tmpbox, Scalar(1, 1, 1), 1);

    // 	Mat normat;
    // 	normalize(maskmat, normat, 255, 0, NORM_MINMAX);
    //
    // 	Mat maskbinmat;
    // 	normat.convertTo(maskbinmat, CV_8UC1, 1);
    // 	imwrite("subbin.jpg", maskbinmat);

    //cout << mapmat << endl;

    return mean(mapmat(Rect(Point(xmin, ymin), Point(xmax + 1, ymax + 1))).clone(),
                    maskmat).val[0];

}

// use clipper
void unClip(vector<Point> &minBoxVec, float allEdgeSize, vector<Point> &outVec, float unClipRatio) {
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
    vector<Point> rsVec;
    for (int i = 0; i < polys.size(); ++i) {
        ClipperLib::Path tmpPoly = polys[i];
        for (int j = 0; j < tmpPoly.size(); ++j) {
            outVec.emplace_back(tmpPoly[j].X, tmpPoly[j].Y);
        }
    }
}

vector<float> substractMeanNormalize(Mat &src, const float *meanVals, const float *normVals) {
    auto inputTensorSize = src.cols * src.rows * src.channels();
    vector<float> inputTensorValues(inputTensorSize);
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

vector<int> getAngleIndexes(vector<Angle> &angles) {
    vector<int> angleIndexes;
    angleIndexes.reserve(angles.size());
    for (int i = 0; i < angles.size(); ++i) {
        angleIndexes.push_back(angles[i].index);
    }
    return angleIndexes;
}

vector<const char *> getInputNames(unique_ptr<Ort::Session> &session) {
    Ort::AllocatorWithDefaultOptions allocator;
    size_t numInputNodes = session->GetInputCount();
    vector<const char *> inputNodeNames(numInputNodes);
    vector<int64_t> inputNodeDims;

    //printf("Number of inputs = %zu\n", numInputNodes);

    for (int i = 0; i < numInputNodes; i++) {
        // print input node names
        char *inputName = session->GetInputName(i, allocator);
        //printf("Input %d : name=%s\n", i, inputName);
        inputNodeNames[i] = inputName;

        // print input node types
        Ort::TypeInfo typeInfo = session->GetInputTypeInfo(i);
        auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();

        ONNXTensorElementDataType type = tensorInfo.GetElementType();
        //printf("Input %d : type=%d\n", i, type);

        // print input shapes/dims
        inputNodeDims = tensorInfo.GetShape();
        //printf("Input %d : num_dims=%zu\n", i, inputNodeDims.size());
        /*for (int j = 0; j < inputNodeDims.size(); j++)
            printf("Input %d : dim %d=%jd\n", i, j, inputNodeDims[j]);*/
    }
    return inputNodeNames;
}

vector<const char *> getOutputNames(unique_ptr<Ort::Session> &session) {
    Ort::AllocatorWithDefaultOptions allocator;
    size_t numOutputNodes = session->GetOutputCount();
    vector<const char *> outputNodeNames(numOutputNodes);
    vector<int64_t> outputNodeDims;

    //printf("Number of outputs = %zu\n", numOutputNodes);

    for (int i = 0; i < numOutputNodes; i++) {
        // print input node names
        char *outputName = session->GetOutputName(i, allocator);
        //printf("Input %d : name=%s\n", i, outputName);
        outputNodeNames[i] = outputName;

        // print input node types
        Ort::TypeInfo type_info = session->GetOutputTypeInfo(i);
        auto tensorInfo = type_info.GetTensorTypeAndShapeInfo();

        ONNXTensorElementDataType type = tensorInfo.GetElementType();
        //printf("Output %d : type=%d\n", i, type);

        // print input shapes/dims
        outputNodeDims = tensorInfo.GetShape();
        //printf("Output %d : num_dims=%zu\n", i, outputNodeDims.size());
        /*for (int j = 0; j < outputNodeDims.size(); j++)
            printf("Output %d : dim %d=%jd\n", i, j, outputNodeDims[j]);*/
    }
    return outputNodeNames;
}

int getMostProbabilityAngleIndex(vector<int> &input, double mean, double stdev) {
    if (stdev == 0) return mean;
    double start = mean - stdev;
    double end = mean + stdev;
    printf("start=%f,end=%f\n", start, end);
    for (auto it = input.begin(); it != input.end();) {
        if (*it < start || *it > end) {
            it = input.erase(it);
        }
        ++it;
    }
    auto sum = accumulate(input.begin(), input.end(), 0.0);
    return (int) (sum / input.size());
}

void saveImg(Mat &img, const char *imgPath) {
    imwrite(imgPath, img);
}

string getSrcImgFilePath(const char *path, const char *imgName) {
    string filePath;
    filePath.append(path).append(imgName);
    return filePath;
}

string getResultTxtFilePath(const char *path, const char *imgName) {
    string filePath;
    filePath.append(path).append(imgName).append("-result.txt");
    return filePath;
}

string getResultImgFilePath(const char *path, const char *imgName) {
    string filePath;
    filePath.append(path).append(imgName).append("-result.jpg");
    return filePath;
}

string getDebugImgFilePath(const char *path, const char *imgName, int i, const char *tag) {
    string filePath;
    filePath.append(path).append(imgName).append(tag).append(to_string(i)).append(".jpg");
    return filePath;
}