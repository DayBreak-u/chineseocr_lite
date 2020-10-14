#include "ncnn/net.h"
#include "Logger.h"
#include "BitmapUtils.h"
#include "OcrLite.h"
#include "OcrUtil.h"

static OcrLite *ocrLite;

extern "C" JNIEXPORT jboolean JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_init(JNIEnv *env, jobject thiz, jobject assetManager) {

    ocrLite = new OcrLite(env, assetManager);
    return JNI_TRUE;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_detect(JNIEnv *env, jobject thiz, jobject input,
                                                 jobject output, jint reSize,
                                                 jfloat boxScoreThresh, jfloat boxThresh,
                                                 jfloat minArea, jfloat angleScaleWidth,
                                                 jfloat angleScaleHeight,
                                                 jfloat textScaleWidth, jfloat textScaleHeight) {
    cv::Mat imgRGBA, imgBGR, imgOut;
    bitmapToMat(env, input, imgRGBA);
    cv::cvtColor(imgRGBA, imgBGR, cv::COLOR_RGBA2BGR);

    //按比例缩小图像，减少文字分割时间
    ScaleParam s = getScaleParam(imgRGBA, reSize);//例：按长或宽缩放 src.cols=不缩放，src.cols/2=长度缩小一半

    cv::Mat imgBox = imgBGR.clone();
    std::string outStr = ocrLite->detect(imgBGR, s, imgBox,
                                         boxScoreThresh, boxThresh, minArea,
                                         angleScaleWidth, angleScaleHeight,
                                         textScaleWidth, textScaleHeight);

    cv::cvtColor(imgBox, imgOut, cv::COLOR_BGR2RGBA);
    matToBitmap(env, imgOut, output);

    return env->NewStringUTF(outStr.c_str());
}