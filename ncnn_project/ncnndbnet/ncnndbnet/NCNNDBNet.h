#pragma once
#include <vector>
#include <algorithm>
#include <math.h>
#include <numeric>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"    
#include "opencv2/imgproc/imgproc.hpp"
#include "net.h"
#include "ZData.h"
class NCNNDBNet
{
public:
	NCNNDBNet();
	~NCNNDBNet();
	std::vector<TextBox> Forward(cv::Mat& srcmat, int inshortsize);
private:
	ncnn::Net DBNet;

	const float mean_vals_dbnet[3] = { 0.485 * 255, 0.456 * 255, 0.406 * 255 };
	const float norm_vals_dbnet[3] = { 1.0 / 0.229 / 255.0, 1.0 / 0.224 / 255.0, 1.0 / 0.225 / 255.0 };
	const float unclip_ratio = 1.5;
	const float box_thresh = 0.5;
	const float thresh = 0.3;
	const int min_size = 3;
};

