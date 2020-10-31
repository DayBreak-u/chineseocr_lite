#ifdef __JNI__

#include "version.h"
#include <jni.h>
#include "OcrLite.h"
#include "OcrResultUtils.h"
static OcrLite *ocrLite;

const char *jstringToChar(JNIEnv *env, jstring input) {
    const char *str;
    jboolean isCopy = false;
    str = env->GetStringUTFChars(input, &isCopy);
    if (str == NULL) {
        return NULL;
    }
    env->ReleaseStringUTFChars(input, str);
    return str;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_getVersion(JNIEnv *env, jobject thiz) {
    jstring ver = env->NewStringUTF(VERSION);
    return ver;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_init(JNIEnv *env, jobject thiz, jint numThread) {
    ocrLite = new OcrLite(numThread);
    printf("numThread=%d\n", numThread);
    return JNI_TRUE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_initLogger(JNIEnv *env, jobject thiz, jboolean isConsole,
                                                     jboolean isPartImg, jboolean isAngleImg, jboolean isDebugImg,
                                                     jboolean isResultImg) {
    ocrLite->initLogger(isConsole,//isOutputConsole
                        isPartImg,//isOutputPartImg
                        isAngleImg,//isOutputAngleImg
                        isDebugImg,//isOutputDebugImg
                        isResultImg);//isOutputResultImg
}

extern "C" JNIEXPORT void JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_enableResultText(JNIEnv *env, jobject thiz, jstring input) {
    std::string argImgPath, imgPath, imgName;
    const char *inputStr = jstringToChar(env, input);
    argImgPath = std::string(inputStr);
    imgPath = argImgPath.substr(0, argImgPath.find_last_of('/') + 1);
    imgName = argImgPath.substr(argImgPath.find_last_of('/') + 1);
    ocrLite->enableResultTxt(imgPath.c_str(), imgName.c_str());
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_initModels(JNIEnv *env, jobject thiz, jstring path) {
    const char *models = jstringToChar(env, path);
    printf("models dir=%s\n", models);
    bool ret = ocrLite->initModels(models);
    return ret;
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_detect(JNIEnv *env, jobject thiz, jstring input,
                                                 jint padding, jint reSize,
                                                 jfloat boxScoreThresh, jfloat boxThresh,
                                                 jfloat minArea, jfloat unClipRatio,
                                                 jboolean doAngle, jboolean mostAngle
) {
    std::string argImgPath, imgPath, imgName;
    const char *inputStr = jstringToChar(env, input);
    argImgPath = std::string(inputStr);
    imgPath = argImgPath.substr(0, argImgPath.find_last_of('/') + 1);
    imgName = argImgPath.substr(argImgPath.find_last_of('/') + 1);
    printf("imgPath=%s, imgName=%s\n", imgPath.c_str(), imgName.c_str());
    OcrResult result = ocrLite->detect(imgPath.c_str(), imgName.c_str(),
                                       padding, reSize,
                                       boxScoreThresh, boxThresh, minArea,
                                       unClipRatio, doAngle, mostAngle);

    return OcrResultUtils(env, result).getJObject();
}
#endif