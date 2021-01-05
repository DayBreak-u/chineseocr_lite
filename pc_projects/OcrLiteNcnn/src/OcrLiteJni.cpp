#ifdef __JNI__

#include "version.h"
#include <jni.h>
#include "OcrLite.h"
#include "OcrResultUtils.h"

static OcrLite *ocrLite;

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved){
    ocrLite = new OcrLite();
    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM *vm, void *reserved){
    //printf("JNI_OnUnload\n");
	delete ocrLite;
}

#ifdef _WIN32
char *jstringToChar(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("gbk");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}
#else

char *jstringToChar(JNIEnv *env, jstring input) {
    char *str = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(input, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        str = (char *) malloc(alen + 1);
        memcpy(str, ba, alen);
        str[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return str;
}

#endif

extern "C" JNIEXPORT jstring JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_getVersion(JNIEnv *env, jobject thiz) {
    jstring ver = env->NewStringUTF(VERSION);
    return ver;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_setNumThread(JNIEnv *env, jobject thiz, jint numThread) {
    ocrLite->setNumThread(numThread);
    printf("numThread=%d\n", numThread);
    return JNI_TRUE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_initLogger(JNIEnv *env, jobject thiz, jboolean isConsole,
                                                     jboolean isPartImg, jboolean isResultImg) {
    ocrLite->initLogger(isConsole,//isOutputConsole
                        isPartImg,//isOutputPartImg
                        isResultImg);//isOutputResultImg
}

extern "C" JNIEXPORT void JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_enableResultText(JNIEnv *env, jobject thiz, jstring input) {
    char *inputStr = jstringToChar(env, input);
    std::string argImgPath = std::string(inputStr);
    std::string imgPath = argImgPath.substr(0, argImgPath.find_last_of('/') + 1);
    std::string imgName = argImgPath.substr(argImgPath.find_last_of('/') + 1);
    ocrLite->enableResultTxt(imgPath.c_str(), imgName.c_str());
    free(inputStr);
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_initModels(JNIEnv *env, jobject thiz, jstring path) {
    char *models = jstringToChar(env, path);
    printf("models dir=%s\n", models);
    ocrLite->initModels(models);
	free(models);
    return true;
}

extern "C" JNIEXPORT void JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_setGpuIndex(JNIEnv *env, jobject thiz, jint gpuIndex) {
    ocrLite->setGpuIndex(gpuIndex);
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_benjaminwan_ocrlibrary_OcrEngine_detect(JNIEnv *env, jobject thiz, jstring input,
                                                 jint padding, jint reSize,
                                                 jfloat boxScoreThresh, jfloat boxThresh,
                                                 jfloat minArea, jfloat unClipRatio,
                                                 jboolean doAngle, jboolean mostAngle
) {
    char *inputStr = jstringToChar(env, input);
    std::string  argImgPath = std::string(inputStr);
    std::string  imgPath = argImgPath.substr(0, argImgPath.find_last_of('/') + 1);
    std::string  imgName = argImgPath.substr(argImgPath.find_last_of('/') + 1);
    printf("imgPath=%s, imgName=%s\n", imgPath.c_str(), imgName.c_str());
    OcrResult result = ocrLite->detect(imgPath.c_str(), imgName.c_str(),
                                       padding, reSize,
                                       boxScoreThresh, boxThresh, minArea,
                                       unClipRatio, doAngle, mostAngle);
	free(inputStr);
    return OcrResultUtils(env, result).getJObject();
}
#endif