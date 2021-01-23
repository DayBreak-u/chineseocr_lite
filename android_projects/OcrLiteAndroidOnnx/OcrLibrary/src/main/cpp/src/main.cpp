#include "OcrResultUtils.h"
#include "BitmapUtils.h"
#include "OcrLite.h"
#include "OcrUtils.h"
#include "omp.h"

static OcrLite *ocrLite;

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    ocrLite = new OcrLite();
    return JNI_VERSION_1_4;
}

JNIEXPORT void JNI_OnUnload(JavaVM *vm, void *reserved) {
    LOGI("Goodbye OcrLite!");
    delete ocrLite;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_init(JNIEnv *env, jobject thiz, jobject assetManager,
                                               jint numThread) {

    ocrLite->init(env, assetManager, numThread);
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
Java_com_benjaminwan_ocrlibrary_OcrEngine_detect(JNIEnv *env, jobject thiz, jobject input, jobject output,
                                                 jint padding, jint maxSideLen, jfloat boxScoreThresh, jfloat boxThresh,
                                                 jfloat unClipRatio, jboolean doAngle, jboolean mostAngle) {
    Logger("padding(%d),maxSideLen(%d),boxScoreThresh(%f),boxThresh(%f),unClipRatio(%f),doAngle(%d),mostAngle(%d)",
           padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
    cv::Mat imgRGBA, imgRGB, imgOut;
    bitmapToMat(env, input, imgRGBA);
    cv::cvtColor(imgRGBA, imgRGB, cv::COLOR_RGBA2RGB);
    int originMaxSide = (std::max)(imgRGB.cols, imgRGB.rows);
    int resize;
    if (maxSideLen <= 0 || maxSideLen > originMaxSide) {
        resize = originMaxSide;
    } else {
        resize = maxSideLen;
    }
    resize += 2*padding;
    cv::Rect paddingRect(padding, padding, imgRGB.cols, imgRGB.rows);
    cv::Mat paddingSrc = makePadding(imgRGB, padding);
    //按比例缩小图像，减少文字分割时间
    ScaleParam s = getScaleParam(paddingSrc, resize);//例：按长或宽缩放 src.cols=不缩放，src.cols/2=长度缩小一半
    OcrResult ocrResult = ocrLite->detect(paddingSrc, paddingRect, s, boxScoreThresh, boxThresh,
                                          unClipRatio, doAngle, mostAngle);

    cv::cvtColor(ocrResult.boxImg, imgOut, cv::COLOR_RGB2RGBA);
    matToBitmap(env, imgOut, output);

    return OcrResultUtils(env, ocrResult, output).getJObject();
}

extern "C" JNIEXPORT jdouble JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_benchmark(JNIEnv *env, jobject thiz, jobject input,
                                                    jint loop) {
    int padding = 50;
    int maxSideLen = 0;
    float boxScoreThresh = 0.6;
    float boxThresh = 0.3;
    float unClipRatio = 2.0;
    bool doAngle = true;
    bool mostAngle = true;
    LOGI("padding(%d),maxSideLen(%d),boxScoreThresh(%f),boxThresh(%f),unClipRatio(%f),doAngle(%d),mostAngle(%d)",
         padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
    cv::Mat imgRGBA, imgRGB, imgOut;
    bitmapToMat(env, input, imgRGBA);
    cv::cvtColor(imgRGBA, imgRGB, cv::COLOR_RGBA2RGB);
    cv::Rect originRect(padding, padding, imgRGB.cols, imgRGB.rows);
    cv::Mat src = makePadding(imgRGB, padding);
    //按比例缩小图像，减少文字分割时间
    ScaleParam s = getScaleParam(src, src.cols);//例：按长或宽缩放 src.cols=不缩放，src.cols/2=长度缩小一半

    LOGI("=====warmup=====");
    OcrResult result = ocrLite->detect(src, originRect, s, boxScoreThresh, boxThresh,
                                       unClipRatio, doAngle, mostAngle);
    LOGI("dbNetTime(%f) detectTime(%f)\n", result.dbNetTime, result.detectTime);
    double dbTime = 0.0f;
    double detectTime = 0.0f;
    int loopCount = loop;
    for (int i = 0; i < loopCount; ++i) {
        LOGI("=====loop:%d=====", i + 1);
        OcrResult ocrResult = ocrLite->detect(src, originRect, s, boxScoreThresh, boxThresh,
                                              unClipRatio, doAngle, mostAngle);
        LOGI("dbNetTime(%f) detectTime(%f)\n", ocrResult.dbNetTime, ocrResult.detectTime);
        dbTime += ocrResult.dbNetTime;
        detectTime += ocrResult.detectTime;
    }
    LOGI("=====result=====\n");
    double averageTime = detectTime / loopCount;
    LOGI("average dbNetTime=%fms, average detectTime=%fms\n", dbTime / loopCount,
         averageTime);
    return (jdouble) averageTime;
}