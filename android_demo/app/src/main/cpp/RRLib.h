#include "opencv2/opencv.hpp"
class RRLib {
    //----------------------------------------------
    // Check if rotated box contained in image region
    //----------------------------------------------
    bool boxInRange(cv::Mat& img, cv::RotatedRect& r);
    //----------------------------------------------
    // Check if rotated box contained in rectangular region
    //----------------------------------------------
    bool boxInRange(cv::Rect r, cv::RotatedRect& rr);
    //----------------------------------------------------------
    // Extracts rotated region and returns it as dst image
    //----------------------------------------------------------
    void getRotRectImg(cv::RotatedRect rr, cv::Mat &img, cv::Mat& dst);
    //----------------------------------------------------------
    // Copies image region (src_roi) from src image, to rotated region on image dst  
    //----------------------------------------------------------
    void copyToRotRectImg(cv::Rect src_roi, cv::RotatedRect rr, cv::Mat &src, cv::Mat& dst);
    //----------------------------------------------------------
    // Copies image src, to rotated region on image dst  
    //----------------------------------------------------------
    void copyToRotRectImg(cv::RotatedRect rr, cv::Mat &src, cv::Mat& mask, cv::Mat& dst);
    //----------------------------------------------------------
    // Draws rotated rectangle
    //----------------------------------------------------------
    void drawRR(cv::Mat &img, cv::RotatedRect rr, cv::Scalar color, int width);
};