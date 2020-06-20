#include <android/bitmap.h>
#include <android/log.h>
#include <jni.h>
#include <string>
#include <vector>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#include "ocr.h"   //这里看成自己的id.h
#include <sys/time.h>
//#include <unistd.h>
#include "ncnn/net.h"

static OCR *textocr;
#define TAG "ocrso"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_abbott_mytext_textocr_Init(JNIEnv *env, jobject thiz,jobject assetManager,
        jbyteArray pse_param, jbyteArray pse_bin,
        jbyteArray crnn_param, jbyteArray crnn_bin,jobject label) {
// TODO: implement textocr_init

    std::vector<ncnn::Mat> param_files;
    std::vector<ncnn::Mat> bin_files;

    ncnn::Mat ncnn_pse_param,ncnn_crnn_param;
    ncnn::Mat ncnn_pse_bin,ncnn_crnn_bin;


    // init pse_param
    {
    int len = env->GetArrayLength(pse_param);
    ncnn_pse_param.create(len, (size_t) 1u);
    env->GetByteArrayRegion(pse_param, 0, len, (jbyte *) ncnn_pse_param);
    param_files.push_back(ncnn_pse_param);

    }

    // init pse_bin
    {
    int len = env->GetArrayLength(pse_bin);
    ncnn_pse_bin.create(len, (size_t) 1u);
    env->GetByteArrayRegion(pse_bin, 0, len, (jbyte *) ncnn_pse_bin);

    bin_files.push_back(ncnn_pse_bin);

    }


    // init crnn_param
    {
    int len = env->GetArrayLength(crnn_param);
    ncnn_crnn_param.create(len, (size_t) 1u);
    env->GetByteArrayRegion(crnn_param, 0, len, (jbyte *) ncnn_crnn_param);
    param_files.push_back(ncnn_crnn_param);
    }

    // init crnn_bin
    {
    int len = env->GetArrayLength(crnn_bin);
    ncnn_crnn_bin.create(len, (size_t) 1u);
    env->GetByteArrayRegion(crnn_bin, 0, len, (jbyte *) ncnn_crnn_bin);
    bin_files.push_back(ncnn_crnn_bin);
    }


    AAssetManager * mgr = AAssetManager_fromJava( env, assetManager);
    textocr = new OCR(mgr,param_files,bin_files);

    return JNI_TRUE;

}

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_abbott_mytext_textocr_Detect(JNIEnv *env, jobject thiz, jobject bitmap,jint lengthsize) {
    // TODO: implement Detect()


    // ncnn from bitmap
        AndroidBitmapInfo info;
        AndroidBitmap_getInfo(env, bitmap, &info);

        int width = info.width;
        int height = info.height;



        void* indata;
        AndroidBitmap_lockPixels(env, bitmap, &indata);
        LOGD("convert bitmap to cv::Mat");
        // 把像素转换成data，并指定通道顺序
        // 因为图像预处理每个网络层输入的数据格式不一样一般为300*300 128*128等等所以这类需要一个resize的操作可以在cpp中写，也可以是java读入图片时有个resize操作
        //in = ncnn::Mat::from_pixels_resize((const unsigned char*)indata, ncnn::Mat::PIXEL_RGBA2RGB, width, height,300,300);
        cv::Mat test(info.height, info.width, CV_8UC4, (char*)indata); // RGBA
        cv::Mat img_bgr;
        cv::cvtColor(test, img_bgr, CV_RGBA2BGR);

        std::vector<std::vector<int>> out = textocr->Detect(img_bgr,lengthsize);

        int size = out.size();
        for(int i = 0;i < out.size();i++){
            size += out[i].size();
        }

        jint res[size];
        int count = 0;
        for(int i =0;i < out.size();i++){
            for(int j = 0;j < out[i].size();j++){
                res[count++] = out[i][j];
            }
            res[count++] = -1;
        }
        jintArray jOutputData = env->NewIntArray(size);
        if (jOutputData == nullptr) return nullptr;
        env->SetIntArrayRegion(jOutputData, 0,  size,res);
        return jOutputData;



}
