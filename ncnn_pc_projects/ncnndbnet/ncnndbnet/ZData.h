#pragma once
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
struct TextBox
{
	std::vector<cv::Point> Box;
	float Score;
	TextBox(std::vector<cv::Point> box, float score) : Box(box), Score(score) {};
};