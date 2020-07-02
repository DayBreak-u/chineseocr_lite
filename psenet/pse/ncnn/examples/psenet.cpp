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
#include <stdio.h>
#include <vector>
#include <numeric>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "platform.h"
#include "net.h"

void ncnn2cv(ncnn::Mat src, cv::Mat &score, cv::Mat &thre_img, const float thre_val = 0.5) {
    float *srcdata = (float *) src.data;
    for (int i = 0; i < src.h; i++) {
        for (int j = 0; j < src.w; j++) {
            score.at<float>(i, j) = srcdata[i * src.w + j];
            if (srcdata[i * src.w + j] >= thre_val) {
                thre_img.at<uchar>(i, j) = 255;
            } else {
                thre_img.at<uchar>(i, j) = 0;
            }
        }
    }
}

cv::Mat resize_img(cv::Mat src,const int long_size)
{
    int w = src.cols;
    int h = src.rows;
    std::cout<<"原图尺寸 (" << w << ", "<<h<<")"<<std::endl;
    float scale = 1.f;
    if (w > h)
    {
        scale = (float)long_size / w;
        w = long_size;
        h = h * scale;
    }
    else
    {
        scale = (float)long_size / h;
        h = long_size;
        w = w * scale;
    }
    if (h % 32 != 0)
    {
        h = (h / 32 + 1) * 32;
    }
    if (w % 32 != 0)
    {
        w = (w / 32 + 1) * 32;
    }
    std::cout<<"缩放尺寸 (" << w << ", "<<h<<")"<<std::endl;
    cv::Mat result;
    cv::resize(src, result, cv::Size(w, h));
    return result;
}

cv::Mat draw_bbox(cv::Mat &src, const std::vector<std::vector<cv::Point>> &bboxs) {
    cv::Mat dst;
    if (src.channels() == 1) {
        cv::cvtColor(src, dst, cv::COLOR_GRAY2BGR);
    } else {
        dst = src.clone();
    }
    auto color = cv::Scalar(0, 0, 255);
    for (auto bbox :bboxs) {
        cv::line(dst, bbox[0], bbox[1], color, 3);
        cv::line(dst, bbox[1], bbox[2], color, 3);
        cv::line(dst, bbox[2], bbox[3], color, 3);
        cv::line(dst, bbox[3], bbox[0], color, 3);
    }
    return dst;
}

std::vector<std::vector<cv::Point>> deocde(const cv::Mat &score, const cv::Mat &thre, const int scale, const float h_scale, const float w_scale) {
    int img_rows = score.rows;
    int img_cols = score.cols;
    auto min_w_h = std::min(img_cols,img_rows);
    min_w_h *= min_w_h / 20;
    cv::Mat stats, centroids, label_img(thre.size(), CV_32S);
    // 二值化
//    cv::threshold(cv_img * 255, thre, 0, 255, cv::THRESH_OTSU);
    // 计算连通域ss
    int nLabels = connectedComponentsWithStats(thre, label_img, stats, centroids);

    std::vector<float> angles;
    std::vector<std::vector<cv::Point>> bboxs;

    for (int label = 1; label < nLabels; label++) {
        float area = stats.at<int>(label, cv::CC_STAT_AREA);
        if (area < min_w_h / (scale * scale)) {
            continue;
        }
        // 计算该label的平均分数
        std::vector<float> scores;
        std::vector<cv::Point> points;
        for (int y = 0; y < img_rows; ++y) {
            for (int x = 0; x < img_cols; ++x) {
                if (label_img.at<int>(y, x) == label) {
                    scores.emplace_back(score.at<float>(y, x));
                    points.emplace_back(cv::Point(x, y));
                }
            }
        }

        //均值
        double sum = std::accumulate(std::begin(scores), std::end(scores), 0.0);
        if (sum == 0) {
            continue;
        }
        double mean = sum / scores.size();

        if (mean < 0.8) {
            continue;
        }
        cv::RotatedRect rect = cv::minAreaRect(points);
        float w = rect.size.width;
        float h = rect.size.height;
        float angle = rect.angle;

        if (w < h) {
            std::swap(w, h);
            angle -= 90;
        }
        if (45 < std::abs(angle) && std::abs(angle) < 135) {
            std::swap(img_rows, img_cols);
        }
        points.clear();
        // 对卡号进行限制，长宽比，卡号的宽度不能超过图片宽高的95%
        if (w > h * 8 && w < img_cols * 0.95) {
            cv::Mat bbox;
            cv::boxPoints(rect, bbox);
            for (int i = 0; i < bbox.rows; ++i) {
                points.emplace_back(cv::Point(int(bbox.at<float>(i, 0) * w_scale), int(bbox.at<float>(i, 1) * h_scale)));
            }
            bboxs.emplace_back(points);
            angles.emplace_back(angle);
        }
    }
    return bboxs;
}

static int detect_rfcn(const char *model, const char *model_param, const char *imagepath, const int long_size = 800) {
    cv::Mat im_bgr = cv::imread(imagepath, 1);

    if (im_bgr.empty()) {
        fprintf(stderr, "cv::imread %s failed\n", imagepath);
        return -1;
    }
    // 图像缩放
    auto im = resize_img(im_bgr, long_size);
    float h_scale = im_bgr.rows * 1.0 / im.rows;
    float w_scale = im_bgr.cols * 1.0 / im.cols;

    ncnn::Mat in = ncnn::Mat::from_pixels(im.data, ncnn::Mat::PIXEL_BGR, im.cols, im.rows);
    const float norm_vals[3] = { 1 / 255.f ,1 / 255.f ,1 / 255.f};
    in.substract_mean_normalize(0,norm_vals);

    std::cout << "输入尺寸 (" << in.w << ", " << in.h << ")" << std::endl;

    ncnn::Net psenet;
    psenet.load_param(model_param);
    psenet.load_model(model);
    ncnn::Extractor ex = psenet.create_extractor();
//    ex.set_num_threads(4);ss
    ex.input("0", in);

    ncnn::Mat preds;
    double time1 = static_cast<double>( cv::getTickCount());
    ex.extract("636", preds);
    std::cout << "前向时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
    std::cout << "网络输出尺寸 (" << preds.w << ", " << preds.h << ", " << preds.c << ")" << std::endl;

    time1 = static_cast<double>( cv::getTickCount());
    cv::Mat score = cv::Mat::zeros(preds.h, preds.w, CV_32FC1);
    cv::Mat thre = cv::Mat::zeros(preds.h, preds.w, CV_8UC1);
    ncnn2cv(preds, score, thre);
    auto bboxs = deocde(score, thre, 1, h_scale, w_scale);
    std::cout << "decode 时间:" << (static_cast<double>( cv::getTickCount()) - time1) / cv::getTickFrequency() << "s" << std::endl;
    auto result = draw_bbox(im_bgr, bboxs);
    cv::imwrite("/home/zj/project/ncnn/examples/imgs/result.jpg", result);
    cv::imwrite("/home/zj/project/ncnn/examples/imgs/net_result.jpg", score * 255);
    cv::imwrite("/home/zj/project/ncnn/examples/imgs/net_thre.jpg", thre);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s [model model path imagepath long_size]\n", argv[0]);
        return -1;
    }
    const char *model = argv[1];
    const char *model_param = argv[2];
    const char *imagepath = argv[3];
    const int long_size = atoi(argv[4]);
    std::cout << model << " " << model_param << " " << imagepath << " " << long_size << std::endl;

    detect_rfcn(model, model_param, imagepath, long_size);
    return 0;
}
