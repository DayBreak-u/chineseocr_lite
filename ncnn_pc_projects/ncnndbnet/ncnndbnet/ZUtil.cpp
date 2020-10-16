#include "ZUtil.h"
#include "clipper.hpp"

bool cvpointcompare(cv::Point a, cv::Point b)
{
	return a.x < b.x;
}

int DrawMatText(cv::Mat& srcmat, std::vector<cv::Point>& textpoint)
{
	cv::RotatedRect textrect = cv::minAreaRect(textpoint);
	cv::Point2f vertices[4];      //定义4个点的数组
	textrect.points(vertices);   //将四个点存储到vertices数组中
	for (int i = 0; i < 4; i++)
		cv::line(srcmat, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
	//cv::polylines(srcmat, textpoint, true, cv::Scalar(0, 255, 0), 2);

	return 1;
}

int get_mini_boxes(std::vector<cv::Point>& invec, std::vector<cv::Point>& minboxvec, float& minedgesize, float& alledgesize)
{
	cv::RotatedRect textrect = cv::minAreaRect(invec);
	cv::Mat boxPoints2f;
	cv::boxPoints(textrect, boxPoints2f);

	float* p1 = (float*)boxPoints2f.data;
	std::vector<cv::Point> tmpvev;
	for (int i = 0; i < 4; ++i, p1 += 2)
	{
		//std::cout << p1[0] << "," << p1[1] << std::endl;
		tmpvev.push_back(cv::Point((int)(p1[0]), (int)(p1[1])));
	}

	std::sort(tmpvev.begin(), tmpvev.end(), cvpointcompare);

	minboxvec.clear();

	int index1 = 0, index2 = 0, index3 = 0, index4 = 0;
	if (tmpvev[1].y > tmpvev[0].y)
	{
		index1 = 0;
		index4 = 1;
	}
	else
	{
		index1 = 1;
		index4 = 0;
	}

	if (tmpvev[3].y > tmpvev[2].y)
	{
		index2 = 2;
		index3 = 3;
	}
	else
	{
		index2 = 3;
		index3 = 2;
	}

	minboxvec.clear();

	minboxvec.push_back(tmpvev[index1]);
	minboxvec.push_back(tmpvev[index2]);
	minboxvec.push_back(tmpvev[index3]);
	minboxvec.push_back(tmpvev[index4]);

	minedgesize = (std::min)(textrect.size.width, textrect.size.height);
	alledgesize = 2.f * (textrect.size.width + textrect.size.height);


	return 1;
}

float box_score_fast(cv::Mat& mapmat, std::vector<cv::Point>& _box)
{
	std::vector<cv::Point> box = _box;
	int wid = mapmat.cols;
	int hi = mapmat.rows;
	int xmax = -1, xmin = 1000000, ymax = -1, ymin = 1000000;
	for (int i = 0; i < box.size(); ++i)
	{
		if (xmax < box[i].x)
			xmax = box[i].x;
		if (xmin > box[i].x)
			xmin = box[i].x;
		if (ymax < box[i].y)
			ymax = box[i].y;
		if (ymin > box[i].y)
			ymin = box[i].y;
	}
	xmax = (std::min)((std::max)(xmax, 0), wid - 1);
	xmin = (std::max)((std::min)(xmin, wid - 1), 0);
	ymax = (std::min)((std::max)(ymax, 0), hi - 1);
	ymin = (std::max)((std::min)(ymin, hi - 1), 0);

	for (int i = 0; i < box.size(); ++i)
	{
		box[i].x = box[i].x - xmin;
		box[i].y = box[i].y - ymin;
	}

	std::vector<std::vector<cv::Point>> tmpbox;
	tmpbox.push_back(box);
	cv::Mat maskmat(ymax - ymin + 1, xmax - xmin + 1, CV_8UC1, cv::Scalar(0, 0, 0));
	cv::fillPoly(maskmat, tmpbox, cv::Scalar(1, 1, 1), 1);

	// 	cv::Mat normat;
	// 	cv::normalize(maskmat, normat, 255, 0, cv::NORM_MINMAX);
	// 
	// 	cv::Mat maskbinmat;
	// 	normat.convertTo(maskbinmat, CV_8UC1, 1);
	// 	imwrite("subbin.jpg", maskbinmat);

		//std::cout << mapmat << std::endl;

	return cv::mean(mapmat(cv::Rect(cv::Point(xmin, ymin), cv::Point(xmax + 1, ymax + 1))).clone(), maskmat).val[0];

}


int unclip(std::vector<cv::Point>& minboxvec, float alledgesize, std::vector<cv::Point>& outvec, float unclip_ratio)
{
	ClipperLib::Path poly;

	for (int i = 0; i < minboxvec.size(); ++i)
	{
		poly.push_back(ClipperLib::IntPoint(minboxvec[i].x, minboxvec[i].y));
	}

	double distace = unclip_ratio * ClipperLib::Area(poly) / (double)alledgesize;

	ClipperLib::ClipperOffset clipperoffset;
	clipperoffset.AddPath(poly, ClipperLib::JoinType::jtRound, ClipperLib::EndType::etClosedPolygon);
	ClipperLib::Paths polys;
	polys.push_back(poly);
	clipperoffset.Execute(polys, distace);


	outvec.clear();
	std::vector<cv::Point> rsvec;
	for (int i = 0; i < polys.size(); ++i)
	{
		ClipperLib::Path tmppoly = polys[i];
		for (int j = 0; j < tmppoly.size(); ++j)
		{
			outvec.push_back(cv::Point(tmppoly[j].X, tmppoly[j].Y));
		}
	}

	return 1;

}