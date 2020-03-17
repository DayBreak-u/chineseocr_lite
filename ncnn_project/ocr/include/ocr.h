#ifndef __OCR_H__
#define __OCR_H__



#include <vector>
#include "net.h"
#include <algorithm>
#include <math.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"    
#include "opencv2/imgproc/imgproc.hpp"
#include "RRLib.h"
#include "polygon.h"
#include <fstream>
#include <string>
#include <iostream>


using namespace std;

struct Bbox
{
    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;
    int x4;
    int y4;
};

class OCR
{
    public:
        OCR();
        void detect(cv::Mat im_bgr,int long_size);


    private:
       



        ncnn::Net  psenet,crnn_net,crnn_vertical_net,angle_net;
        ncnn::Mat  img;
        int num_thread = 4;
        int shufflenetv2_target_w  = 196;
        int shufflenetv2_target_h  = 48;
        int crnn_h = 32;

        const float mean_vals_pse_angle[3] = { 0.485 * 255, 0.456 * 255, 0.406 * 255};
        const float norm_vals_pse_angle[3] = { 1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 /0.225 / 255.0};

        const float mean_vals_crnn[1] = { 127.5};
        const float norm_vals_crnn[1] = { 1.0 /127.5};

        std::vector<std::string>   alphabetChinese;


};




#endif