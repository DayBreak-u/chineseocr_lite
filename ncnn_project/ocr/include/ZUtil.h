#pragma once

int get_mini_boxes(std::vector<cv::Point>& invec, std::vector<cv::Point>& minboxvec, float& minedgesize, float& alledgesize);
float box_score_fast(cv::Mat& mapmat, std::vector<cv::Point>& _box);
int unclip(std::vector<cv::Point>& minboxvec, float alledgesize, std::vector<cv::Point>& outvec, float unclip_ratio);
int DrawMatText(cv::Mat& srcmat, std::vector<cv::Point>& textpoint);