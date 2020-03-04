// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2018 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
#include <iostream>
#include <math.h>
#include <cstdio>
#include <vector>
#include <numeric>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <boost/locale.hpp>
#include "platform.h"
#include "net.h"

using namespace std;

static int shufflenev2_05(const char *model, const char *model_param, const char *imagepath) {
    cv::Mat im_bgr = cv::imread(imagepath, 1);

    if (im_bgr.empty()) {
        fprintf(stderr, "cv::imread %s failed\n", imagepath);
        return -1;
    }
    // 图像缩放

    int target_w = 196 ;
    int target_h = 48 ;


    ncnn::Mat  in = ncnn::Mat::from_pixels_resize(im_bgr.data, 
                ncnn::Mat::PIXEL_BGR2RGB, im_bgr.cols, im_bgr.rows , target_w,target_h );
    const float mean_vals[3] = { 255 *  0.485, 255 * 0.456, 255 * 0.406  };
    const float norm_vals[3] = { 1.0 / 255.0 / 0.229,1.0 / 255.0 / 0.224, 1.0 / 255.0 /0.225  };

    in.substract_mean_normalize(mean_vals,norm_vals );

    std::cout << "输入尺寸 (" << in.w << ", " << in.h<< ", " << in.c << ")" << std::endl;

    ncnn::Net crnn_dense;
    crnn_dense.load_param(model_param);
    crnn_dense.load_model(model);
    ncnn::Extractor ex = crnn_dense.create_extractor();
//    ex.set_num_threads(4);ss
    ex.input("input", in);

    ncnn::Mat preds;
    double time1 = static_cast<double>( cv::getTickCount());

    ex.extract("out", preds);

    std::cout << "前向时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
    std::cout << "网络输出尺寸 (" << preds.w << ", " << preds.h << ", " << preds.c << ")" << std::endl;

    float *srcdata =(float*) preds.data;

     int max_index = 0;
     int max_value ;
    for (int i=0; i<preds.w;i++){
        std::cout << srcdata[i] << std::endl;
        if (i==0)max_value = srcdata[i];
        else if (srcdata[i] > max_index) {
            max_index = i ;
            max_value = srcdata[i];
        }
    }

    std::cout<<  max_index  <<std::endl;
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s [model model path imagepath \n", argv[0]);
        return -1;
    }
    const char *model = argv[1];
    const char *model_param = argv[2];
    const char *imagepath = argv[3];



    shufflenev2_05(model, model_param, imagepath);
    return 0;
}
