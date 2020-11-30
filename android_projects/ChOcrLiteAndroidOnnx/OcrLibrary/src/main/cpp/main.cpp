#include "OcrResultUtils.h"
#include "BitmapUtils.h"
#include "OcrLite.h"
#include "OcrUtils.h"
#include "omp.h"

static OcrLite *ocrLite;

extern "C" JNIEXPORT
void JNI_OnUnload(JavaVM *vm, void *reserved) {
    free(ocrLite);
    LOGI("Goodbye OcrLite!");
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_init(JNIEnv *env, jobject thiz, jobject assetManager,
                                               jint numThread) {

    ocrLite = new OcrLite(env, assetManager, numThread);
    omp_set_num_threads(numThread);
    //ocrLite->initLogger(false);
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
JNIEXPORT jobject JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_detect(JNIEnv *env, jobject thiz, jobject input,
                                                 jobject output,
                                                 jint padding, jint reSize,
                                                 jfloat boxScoreThresh, jfloat boxThresh,
                                                 jfloat minArea, jfloat unClipRatio,
                                                 jboolean doAngle, jboolean mostAngle) {
    Logger("padding(%d),reSize(%d),boxScoreThresh(%f),boxThresh(%f),minArea(%f),unClipRatio(%f),doAngle(%d),mostAngle(%d)",
           padding, reSize, boxScoreThresh, boxThresh, minArea, unClipRatio, doAngle, mostAngle);
    cv::Mat imgRGBA, imgRGB, imgOut;
    bitmapToMat(env, input, imgRGBA);
    cv::cvtColor(imgRGBA, imgRGB, cv::COLOR_RGBA2RGB);
    cv::Rect originRect(padding, padding, imgRGB.cols, imgRGB.rows);
    cv::Mat src = makePadding(imgRGB, padding);
    //按比例缩小图像，减少文字分割时间
    ScaleParam s = getScaleParam(src, reSize);//例：按长或宽缩放 src.cols=不缩放，src.cols/2=长度缩小一半
    OcrResult ocrResult = ocrLite->detect(src, originRect, s,
                                          boxScoreThresh, boxThresh, minArea,
                                          unClipRatio, doAngle, mostAngle);

    /*double startTest = getCurrentTime();
    int loopCount = 100;
    for (int i = 0; i < loopCount; ++i) {
        ocrLite->detect(src, originRect, s,
                        boxScoreThresh, boxThresh, minArea,
                        unClipRatio, doAngle, mostAngle);
    }
    double endTest = getCurrentTime();
    LOGI("average time=%f\n", (endTest - startTest) / loopCount);*/

    cv::cvtColor(ocrResult.boxImg, imgOut, cv::COLOR_RGB2RGBA);
    matToBitmap(env, imgOut, output);

    return OcrResultUtils(env, ocrResult, output).getJObject();
}