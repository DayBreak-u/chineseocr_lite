#pragma once
struct TextBox
{
	std::vector<cv::Point> Box;
	float Score;
	TextBox(std::vector<cv::Point> box, float score) : Box(box), Score(score) {};
};