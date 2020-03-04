//
// Created by dl on 19-7-19.
//

#ifndef FACE_DETECTOR_H
#define FACE_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <stack>
#include "net.h"
#include <chrono>
using namespace std::chrono;

class Timer
{
public:
    // std::stack<clock_t> tictoc_stack;
    std::stack<high_resolution_clock::time_point> tictoc_stack;

    void tic()
    {
        // tictoc_stack.push(clock());
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        tictoc_stack.push(t1);
    }

    double toc(std::string msg = "", bool flag = true)
    {
        double diff = duration_cast<milliseconds>(high_resolution_clock::now() - tictoc_stack.top()).count();
        if(msg.size() > 0){
            //fprintf(stderr, "%s time elapsed: %f ms\n", msg.c_str(), diff);
            if (flag)
                //LOGINFO("%s time elapsed: %f ms\n", msg.c_str(), diff);
                printf("%s time elapsed: %f ms\n", msg.c_str(), diff);
        }

        tictoc_stack.pop();
        return diff;
    }
    void reset()
    {
        // tictoc_stack = std::stack<clock_t>();
        tictoc_stack = std::stack<high_resolution_clock::time_point>();
    }
};

struct bbox{
    float x1;
    float y1;
    float x2;
    float y2;
    float s;
};

struct box{
    float cx;
    float cy;
    float sx;
    float sy;
};

class Detector
{

public:
    Detector();

    void Init(const std::string &model_param, const std::string &model_bin);

    Detector(const std::string &model_param, const std::string &model_bin);

    inline void Release();

    void nms(std::vector<bbox> &input_boxes, float NMS_THRESH);

    void Detect(cv::Mat& bgr, std::vector<bbox>& boxes);

    void create_anchor(std::vector<box> &anchor, int w, int h);

    inline void SetDefaultParams();

    static inline bool cmp(bbox a, bbox b);

    ~Detector();

public:
    float _nms;
    float _threshold;
    float _mean_val[3];

    ncnn::Net *Net;
};
#endif //FASTDETECTOR_H
