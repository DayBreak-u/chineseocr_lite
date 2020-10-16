#include "NCNNDBNet.h"
#include "ZUtil.h"
NCNNDBNet::NCNNDBNet()
{
	DBNet.load_param("../model/dbnet.param");
	DBNet.load_model("../model/dbnet.bin");
	printf("load model ok\n");
}

NCNNDBNet::~NCNNDBNet()
{
}

std::vector<TextBox> NCNNDBNet::Forward(cv::Mat& srcmat, int inshortsize)
{
	std::vector<TextBox> rsbox;

	int srcwid, srchi, wid, hi;
	cv::Mat inmat;
	//inmat = srcmat.clone();
	cv::cvtColor(srcmat, inmat, cv::COLOR_BGR2RGB);
	srcwid = wid = inmat.cols;
	srchi = hi = inmat.rows;

	float scale = 1.f;
	if (wid > hi)
	{
		scale = (float)inshortsize / wid;
		wid = inshortsize;
		hi = hi * scale;
	}
	else
	{
		scale = (float)inshortsize / hi;
		hi = inshortsize;
		wid = wid * scale;
	}
	if (hi % 32 != 0)
	{
		hi = (hi / 32 + 1) * 32;
	}
	if (wid % 32 != 0)
	{
		wid = (wid / 32 + 1) * 32;
	}

	ncnn::Mat  dbnet_input = ncnn::Mat::from_pixels_resize(inmat.data,ncnn::Mat::PIXEL_BGR2RGB, srcwid, srchi, wid,hi);

	dbnet_input.substract_mean_normalize(mean_vals_dbnet, norm_vals_dbnet);
	ncnn::Extractor dbnet_ex = DBNet.create_extractor();
	dbnet_ex.set_num_threads(1);
	dbnet_ex.input("input0", dbnet_input);
	ncnn::Mat dbnet_out;
	dbnet_ex.extract("out1", dbnet_out);

	//printf("c=%d,wid=%d,hi=%d\n",dbnet_out.c,dbnet_out.w,dbnet_out.h);

	cv::Mat fmapmat(hi, wid, CV_32FC1);
	memcpy(fmapmat.data, (float*)dbnet_out.data, wid * hi * sizeof(float));

	cv::Mat norfmapmat;

	norfmapmat = fmapmat > thresh;

 	rsbox.clear();
 	std::vector<std::vector<cv::Point>> contours;
 	cv::findContours(norfmapmat, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
 	for (int i = 0; i < contours.size(); ++i)
 	{
 		std::vector<cv::Point> minbox;
 		float minedgesize, alledgesize;
 		get_mini_boxes(contours[i], minbox, minedgesize, alledgesize);
 
 		if (minedgesize < min_size)
 			continue;
 		float score = box_score_fast(fmapmat, contours[i]);

		if (score < box_thresh)
			continue;

 
 		std::vector<cv::Point> newbox;
 		unclip(minbox, alledgesize, newbox, unclip_ratio);
 
 		get_mini_boxes(newbox, minbox, minedgesize, alledgesize);
 
 		if (minedgesize < min_size + 2)
 			continue;
 
 		for (int j = 0; j < minbox.size(); ++j)
 		{
 			minbox[j].x = (minbox[j].x / (float)wid * srcwid);
 			minbox[j].x = (std::min)((std::max)(minbox[j].x, 0), srcwid);
 
 			minbox[j].y = (minbox[j].y / (float)hi * srchi);
 			minbox[j].y = (std::min)((std::max)(minbox[j].y, 0), srchi);
 		}
 
 		rsbox.push_back(TextBox(minbox, score));
 	}
	return rsbox;
}
