#ifndef __OCR_LITE_LOG_UTILS_H__
#define __OCR_LITE_LOG_UTILS_H__

#include <android/log.h>

#define TAG "OcrLite"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE,TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

double getCurrentTime();
double printTime(char *tag, double start, double end);

#endif //__OCR_LITE_LOG_UTILS_H__
