#ifndef __OCR_LITE_BITMAP_UTILS_H__
#define __OCR_LITE_BITMAP_UTILS_H__

#include <jni.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

void bitmapToMat(JNIEnv *env, jobject bitmap, cv::Mat &dst);

void matToBitmap(JNIEnv *env, cv::Mat &src, jobject bitmap);


#endif //__OCR_LITE_BITMAP_UTILS_H__
