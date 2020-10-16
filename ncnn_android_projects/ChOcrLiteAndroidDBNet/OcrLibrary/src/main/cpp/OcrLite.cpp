#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "OcrLite.h"
#include "LogUtils.h"
#include "RRLib.h"
#include <iosfwd>

using namespace RRLib;

char *readKeysFromAssets(AAssetManager *mgr) {
    //LOGI("readKeysFromAssets start...");
    if (mgr == NULL) {
        LOGE(" %s", "AAssetManager==NULL");
        return NULL;
    }
    char *buffer;
    /*获取文件名并打开*/
    AAsset *asset = AAssetManager_open(mgr, "keys.txt", AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
        LOGE(" %s", "asset==NULL");
        return NULL;
    }
    /*获取文件大小*/
    off_t bufferSize = AAsset_getLength(asset);
    //LOGI("file size : %d", bufferSize);
    buffer = (char *) malloc(bufferSize + 1);
    buffer[bufferSize] = 0;
    int numBytesRead = AAsset_read(asset, buffer, bufferSize);
    //LOGI("readKeysFromAssets: %d", numBytesRead);
    /*关闭文件*/
    AAsset_close(asset);
    //LOGI("readKeysFromAssets exit...");
    return buffer;
}

OcrLite::OcrLite(JNIEnv *env, jobject assetManager) {
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    if (mgr == NULL) {
        LOGE(" %s", "AAssetManager==NULL");
    }

    int ret1 = dbNet.load_param(mgr, "dbnet.param");
    int ret2 = dbNet.load_model(mgr, "dbnet.bin");

    int ret3 = crnnNet.load_param(mgr, "crnn_lite_lstm_v2.param");
    int ret4 = crnnNet.load_model(mgr, "crnn_lite_lstm_v2.bin");
    int ret5 = crnnVerticalNet.load_param(mgr, "crnn_lite_lstm_vertical.param");
    int ret6 = crnnVerticalNet.load_model(mgr, "crnn_lite_lstm_vertical.bin");

    int ret7 = angleNet.load_param(mgr, "shufflenetv2_05_angle.param");
    int ret8 = angleNet.load_model(mgr, "shufflenetv2_05_angle.bin");
    if (ret1 != 0 || ret2 != 0 || ret3 != 0 || ret4 != 0 ||
        ret5 != 0 || ret6 != 0 || ret7 != 0 || ret8 != 0) {
        LOGE("# %d  %d  %d  %d %d  %d  %d  %d", ret1, ret2, ret3, ret4, ret5, ret6, ret7, ret8);
    }
    //int lineCount = 0;
    char *buffer = readKeysFromAssets(mgr);
    if (buffer != NULL) {//有该文件
        std::istringstream inStr(buffer);
        std::string line;
        //LOGI(" txt file  found");
        while (getline(inStr, line)) { // line中不包括每行的换行符
            keys.emplace_back(line);
            //lineCount++;
        }
        //LOGI("lineCount = %d", lineCount);
        free(buffer);
    } else {// 没有该文件
        LOGE(" txt file not found");
    }
    //LOGI("初始化完成!");
}

std::vector<TextBox>
OcrLite::getTextBoxes(cv::Mat &src, ScaleParam &s,
                      float boxScoreThresh, float boxThresh, float minArea) {
    std::vector<TextBox> rsBoxes;
    ncnn::Mat input = ncnn::Mat::from_pixels_resize(src.data, ncnn::Mat::PIXEL_BGR2RGB,
                                                    src.cols, src.rows,
                                                    s.dstWidth, s.dstHeight);

    input.substract_mean_normalize(meanValsDBNet, normValsDBNet);
    ncnn::Extractor extractor = dbNet.create_extractor();
    extractor.set_num_threads(numThread);
    extractor.input("input0", input);
    ncnn::Mat out;
    extractor.extract("out1", out);

    cv::Mat fMapMat(s.dstHeight, s.dstWidth, CV_32FC1);
    memcpy(fMapMat.data, (float *) out.data, s.dstWidth * s.dstHeight * sizeof(float));

    cv::Mat norfMapMat;

    norfMapMat = fMapMat > boxThresh;

    rsBoxes.clear();
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(norfMapMat, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); ++i) {
        std::vector<cv::Point> minBox;
        float minEdgeSize, allEdgeSize;
        getMiniBoxes(contours[i], minBox, minEdgeSize, allEdgeSize);

        if (minEdgeSize < minArea)
            continue;
        float score = boxScoreFast(fMapMat, contours[i]);

        if (score < boxScoreThresh)
            continue;

        for (int j = 0; j < minBox.size(); ++j) {
            minBox[j].x = (minBox[j].x / s.scaleWidth);
            minBox[j].x = (std::min)((std::max)(minBox[j].x, 0), s.srcWidth);

            minBox[j].y = (minBox[j].y / s.scaleHeight);
            minBox[j].y = (std::min)((std::max)(minBox[j].y, 0), s.srcHeight);
        }

        rsBoxes.emplace_back(TextBox(minBox, score));
    }
    reverse(rsBoxes.begin(), rsBoxes.end());//反转，调整顺序
    return rsBoxes;
}

Angle scoreToAngle(ncnn::Mat &score) {
    auto *srcData = (float *) score.data;
    int angleIndex = 0;
    float maxValue = -1000.0f;
    for (int i = 0; i < score.w; i++) {
        if (i == 0)maxValue = srcData[i];
        else if (srcData[i] > maxValue) {
            angleIndex = i;
            maxValue = srcData[i];
        }
    }
    return Angle(angleIndex, maxValue);
}

Angle OcrLite::getAngle(cv::Mat &src) {
    float scale = (float) angleDstHeight / (float) src.rows;
    int angleDstWidth = int((float) src.cols * scale);
    ncnn::Mat input = ncnn::Mat::from_pixels_resize(
            src.data, ncnn::Mat::PIXEL_BGR2RGB,
            src.cols, src.rows,
            angleDstWidth, angleDstHeight);
    input.substract_mean_normalize(meanValsAngle, normValsAngle);
    ncnn::Extractor extractor = angleNet.create_extractor();
    extractor.set_num_threads(numThread);
    extractor.input("input", input);
    ncnn::Mat out;
    extractor.extract("out", out);
    return scoreToAngle(out);
}

TextLine OcrLite::scoreToTextLine(ncnn::Mat &score) {
    auto *srcData = (float *) score.data;
    std::string strRes;
    int lastIndex = 0;
    std::vector<float> scores;
    for (int i = 0; i < score.h; i++) {
        //find max score
        int maxIndex = 0;
        float maxValue = -1000.f;
        for (int j = 0; j < score.w; j++) {
            if (srcData[i * score.w + j] > maxValue) {
                maxValue = srcData[i * score.w + j];
                maxIndex = j;
            }
        }
        if (maxIndex > 0 && (not(i > 0 && maxIndex == lastIndex))) {
            scores.emplace_back(maxValue);
            strRes.append(keys[maxIndex - 1]);
        }
        lastIndex = maxIndex;
    }
    return TextLine(strRes, scores);
}

ncnn::Extractor OcrLite::getExtractor(int angleIndex) {
    if (angleIndex == 0 || angleIndex == 1) {
        return crnnNet.create_extractor();
    } else {
        return crnnVerticalNet.create_extractor();
    }
}

TextLine OcrLite::getTextLine(cv::Mat &src, int angleIndex) {
    // 开始文本识别
    float scale = (float) crnnDstHeight / (float) src.rows;
    int dstWidth = int((float) src.cols * scale);

    ncnn::Mat input = ncnn::Mat::from_pixels_resize(
            src.data, ncnn::Mat::PIXEL_BGR2GRAY,
            src.cols, src.rows, dstWidth, crnnDstHeight);

    input.substract_mean_normalize(meanValsCrnn, normValsCrnn);

    ncnn::Extractor extractor = getExtractor(angleIndex);
    extractor.set_num_threads(numThread);
    extractor.input("input", input);

    // lstm
    ncnn::Mat blob162;
    extractor.extract("234", blob162);

    // batch fc
    ncnn::Mat blob182(256, blob162.h);
    for (int i = 0; i < blob162.h; i++) {
        ncnn::Extractor extractor1 = getExtractor(angleIndex);
        extractor1.set_num_threads(numThread);
        ncnn::Mat blob162_i = blob162.row_range(i, 1);
        extractor1.input("253", blob162_i);

        ncnn::Mat blob182_i;
        extractor1.extract("254", blob182_i);

        memcpy(blob182.row(i), blob182_i, 256 * sizeof(float));
    }

    // lstm
    ncnn::Mat blob243;
    extractor.input("260", blob182);
    extractor.extract("387", blob243);

    // batch fc
    ncnn::Mat blob263(5530, blob243.h);
    for (int i = 0; i < blob243.h; i++) {
        ncnn::Extractor extractor2 = getExtractor(angleIndex);
        extractor2.set_num_threads(numThread);
        ncnn::Mat blob243_i = blob243.row_range(i, 1);
        extractor2.input("406", blob243_i);

        ncnn::Mat blob263_i;
        extractor2.extract("407", blob263_i);

        memcpy(blob263.row(i), blob263_i, 5530 * sizeof(float));
    }

    return scoreToTextLine(blob263);
}

std::string OcrLite::detect(cv::Mat &src, ScaleParam &scale, cv::Mat &imgBox,
                            float boxScoreThresh, float boxThresh, float minArea,
                            float angleScaleWidth, float angleScaleHeight,
                            float textScaleWidth, float textScaleHeight) {

    //文字框 线宽
    int thickness = getThickness(src);
    //图像文字分割
    LOGI("=====Start detect=====");
    LOGI("ScaleParam(sw:%d,sh:%d,dw:%d,dH%d,%f,%f)", scale.srcWidth, scale.srcHeight,
         scale.dstWidth, scale.dstHeight,
         scale.scaleWidth, scale.scaleHeight);
    double startTime = getCurrentTime();
    std::vector<TextBox> textBoxes = getTextBoxes(src, scale, boxScoreThresh, boxThresh, minArea);
    LOGI("TextBoxes Size = %d", textBoxes.size());
    double endTimeTextBoxes = getCurrentTime();
    printTime("Time getTextBoxes", startTime, endTimeTextBoxes);

    std::string strRes;//存放结果
    for (int i = 0; i < textBoxes.size(); ++i) {
        LOGI("-----TextBox[%d] score(%f)-----", i, textBoxes[i].score);
        double startTextLine = getCurrentTime();
        cv::Mat angleImg;//用于识别文字方向
        cv::RotatedRect rectAngle = getPartRect(textBoxes[i].box, angleScaleWidth,
                                                angleScaleHeight);//识别文字方向的范围可以小一些
        RRLib::getRotRectImg(rectAngle, src, angleImg);
        LOGI("rectAngle(%f, %f)", rectAngle.size.width, rectAngle.size.height);

        cv::Mat textImg;//用于识别文字
        cv::RotatedRect rectText = getPartRect(textBoxes[i].box, textScaleWidth,
                                               textScaleHeight);//识别文字的范围需要加大一些
        RRLib::getRotRectImg(rectText, src, textImg);
        LOGI("rectText(%f, %f)", rectText.size.width, rectText.size.height);

        //文字框
        drawTextBox(imgBox, rectText, thickness);
        for (int p = 0; p < 4; ++p) {
            LOGI("Pt%d(x: %d, y: %d)", p, textBoxes[i].box[p].x, textBoxes[i].box[p].y);
        }

        //文字方向识别
        if (angleImg.rows > 1.5 * angleImg.cols) {//把竖向的图统一转为横向
            angleImg = matRotateClockWise90(angleImg);
            textImg = matRotateClockWise90(textImg);
        }
        Angle angle = getAngle(angleImg);
        if (angle.index == 0 || angle.index == 2)
            textImg = matRotateClockWise180(textImg); //把文字的方向统一
        LOGI("angle(index=%d, score=%f)", angle.index, angle.score);

        //文字识别
        TextLine textLine = getTextLine(textImg, angle.index);
        std::ostringstream txtScores;
        for (int s = 0; s < textLine.scores.size(); ++s) {
            txtScores << textLine.scores[s] << ",";
        }
        LOGI("text(line=%s, scores={%s})", textLine.line.c_str(), string(txtScores.str()).c_str());
        strRes.append(textLine.line);
        strRes.append("\n");
        double endTextLine = getCurrentTime();
        printTime("Time TextLine", startTextLine, endTextLine);
    }
    double endTime = getCurrentTime();
    printTime("Time Full", startTime, endTime);
    LOGI("=====End detect=====");
    return strRes;
}
