#include "LogUtils.h"
#include "opencv/cv.hpp"

double getCurrentTime() {
    return (static_cast<double>(cv::getTickCount())) / cv::getTickFrequency() * 1000;//单位毫秒
}

double printTime(char *tag, double start, double end) {
    double time = end - start;
    LOGI("%s = %fms", tag, time);
    return time;
}