#include "DbNet.h"
#include "OcrUtils.h"

void DbNet::setGpuIndex(int gpuIndex) {
#ifdef __VULKAN__
    if (gpuIndex >= 0) {
        net.opt.use_vulkan_compute = true;
        net.set_vulkan_device(gpuIndex);
        printf("dbNet try to use Gpu%d\n", gpuIndex);
    } else {
        net.opt.use_vulkan_compute = false;
        printf("dbNet use Cpu\n");
    }
#endif
}

DbNet::~DbNet() {
    net.clear();
}

void DbNet::setNumThread(int numOfThread) {
    numThread = numOfThread;
}

bool DbNet::initModel(const std::string &pathStr) {
    int dbParam = net.load_param((pathStr + ".param").c_str());
    int dbModel = net.load_model((pathStr + ".bin").c_str());
    if (dbParam != 0 || dbModel != 0) {
        printf("DBNet load param(%d), model(%d)\n", dbParam, dbModel);
        return false;
    } else {
        return true;
    }
}

std::vector<TextBox> findRsBoxes(const cv::Mat &fMapMat, const cv::Mat &norfMapMat, ScaleParam &s,
                                 const float boxScoreThresh, const float unClipRatio) {
    float minArea = 3;
    std::vector<TextBox> rsBoxes;
    rsBoxes.clear();
    std::vector<std::vector<cv::Point>> contours;
    findContours(norfMapMat, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); ++i) {
        float minSideLen, perimeter;
        std::vector<cv::Point> minBox = getMinBoxes(contours[i], minSideLen, perimeter);
        if (minSideLen < minArea)
            continue;
        float score = boxScoreFast(fMapMat, contours[i]);
        if (score < boxScoreThresh)
            continue;
        //---use clipper start---
        std::vector<cv::Point> clipBox = unClip(minBox, perimeter, unClipRatio);
        std::vector<cv::Point> clipMinBox = getMinBoxes(clipBox, minSideLen, perimeter);
        //---use clipper end---

        if (minSideLen < minArea + 2)
            continue;

        for (int j = 0; j < clipMinBox.size(); ++j) {
            clipMinBox[j].x = (clipMinBox[j].x / s.ratioWidth);
            clipMinBox[j].x = (std::min)((std::max)(clipMinBox[j].x, 0), s.srcWidth);

            clipMinBox[j].y = (clipMinBox[j].y / s.ratioHeight);
            clipMinBox[j].y = (std::min)((std::max)(clipMinBox[j].y, 0), s.srcHeight);
        }

        rsBoxes.emplace_back(TextBox{clipMinBox, score});
    }
    reverse(rsBoxes.begin(), rsBoxes.end());
    return rsBoxes;
}

std::vector<TextBox>
DbNet::getTextBoxes(cv::Mat &src, ScaleParam &s, float boxScoreThresh, float boxThresh, float unClipRatio) {
    cv::Mat srcResize;
    resize(src, srcResize, cv::Size(s.dstWidth, s.dstHeight));
    ncnn::Mat input = ncnn::Mat::from_pixels(srcResize.data, ncnn::Mat::PIXEL_RGB,
                                             srcResize.cols, srcResize.rows);

    input.substract_mean_normalize(meanValues, normValues);
    ncnn::Extractor extractor = net.create_extractor();
    extractor.set_num_threads(numThread);
    extractor.input("input0", input);
    ncnn::Mat out;
    extractor.extract("out1", out);
    //-----Data preparation-----
    cv::Mat fMapMat(srcResize.rows, srcResize.cols, CV_32FC1);
    memcpy(fMapMat.data, (float *) out.data, srcResize.rows * srcResize.cols * sizeof(float));

    //-----boxThresh-----
    cv::Mat norfMapMat;
    norfMapMat = fMapMat > boxThresh;

    return findRsBoxes(fMapMat, norfMapMat, s, boxScoreThresh, unClipRatio);
}
