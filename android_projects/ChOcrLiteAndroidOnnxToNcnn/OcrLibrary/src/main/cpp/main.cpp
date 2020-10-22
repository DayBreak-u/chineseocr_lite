#include "BitmapUtils.h"
#include "OcrLite.h"
#include "OcrUtils.h"

static OcrLite *ocrLite;

extern "C" JNIEXPORT jboolean JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_init(JNIEnv *env, jobject thiz, jobject assetManager,
                                               jint numThread) {

    ocrLite = new OcrLite(env, assetManager, numThread);
    return JNI_TRUE;
}

cv::Mat makePadding(cv::Mat &src, const int padding) {
    if (padding <= 0) return src;
    cv::Scalar paddingScalar = {255, 255, 255};
    cv::Mat paddingSrc;
    cv::copyMakeBorder(src, paddingSrc, padding, padding, padding, padding, cv::BORDER_ISOLATED,
                       paddingScalar);
    return paddingSrc;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_detect(JNIEnv *env, jobject thiz, jobject input,
                                                 jobject output,
                                                 jint padding, jint reSize,
                                                 jfloat boxScoreThresh, jfloat boxThresh,
                                                 jfloat minArea, jfloat scaleWidth,
                                                 jfloat scaleHeight) {
    LOGI("padding=%d,reSize=%d,boxScoreThresh=%f,boxThresh=%f,minArea=%f,scaleWidth=%f,scaleHeight=%f",
         padding, reSize, boxScoreThresh, boxThresh, minArea, scaleWidth, scaleHeight);
    cv::Mat imgRGBA, imgBGR, imgOut;
    bitmapToMat(env, input, imgRGBA);
    cv::cvtColor(imgRGBA, imgBGR, cv::COLOR_RGBA2BGR);
    cv::Rect originRect(padding, padding, imgBGR.cols, imgBGR.rows);
    cv::Mat src = makePadding(imgBGR, padding);
    //按比例缩小图像，减少文字分割时间
    ScaleParam s = getScaleParam(src, reSize);//例：按长或宽缩放 src.cols=不缩放，src.cols/2=长度缩小一半
    OcrResult ocrResult = ocrLite->detect(src, originRect, s,
                                          boxScoreThresh, boxThresh, minArea,
                                          scaleWidth, scaleHeight);

    cv::cvtColor(ocrResult.textBoxImg, imgOut, cv::COLOR_BGR2RGBA);
    matToBitmap(env, imgOut, output);

    return env->NewStringUTF(ocrResult.strRes.c_str());
}