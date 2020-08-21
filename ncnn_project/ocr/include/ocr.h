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
        void detect(cv::Mat im_bgr,int short_size);
        void dbnet_decode(cv::Mat im_bgr,int long_size);


    private:
       



        ncnn::Net  dbnet,crnn_net,angle_net;
        ncnn::Mat  img;
        int num_thread = 4;
        int angle_target_w  = 192;
        int angle_target_h  = 32;
        int crnn_h = 32;

        const float mean_vals_dbnet[3] = { 0.485 * 255, 0.456 * 255, 0.406 * 255 };
        const float norm_vals_dbnet[3] = { 1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0 };
        const float unclip_ratio = 2.0;
        const float box_thresh = 0.5;
        const float thresh = 0.3;
        const int min_size = 3;

        const float mean_vals_crnn_angle[3] = { 127.5, 127.5, 127.5};
        const float norm_vals_crnn_angle[3] = { 1.0 /127.5,1.0 /127.5,1.0 /127.5};

        std::vector<std::string>   alphabetChinese;


};




#endif