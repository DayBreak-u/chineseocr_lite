#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "RRLib.h"
#include "OcrLite.h"
#include "OcrUtils.h"
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

OcrLite::OcrLite(JNIEnv *env, jobject assetManager, int numOfThread) {
    numThread = numOfThread;
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
    //load keys
    char *buffer = readKeysFromAssets(mgr);
    if (buffer != NULL) {
        std::istringstream inStr(buffer);
        std::string line;
        //LOGI(" txt file  found");
        while (getline(inStr, line)) {
            keys.emplace_back(line);
            //lineCount++;
        }
        //LOGI("lineCount = %d", lineCount);
        free(buffer);
    } else {
        LOGE(" txt file not found");
    }
    LOGI("Init Models Success!");
}

OcrLite::~OcrLite() {
    dbNet.clear();
    angleNet.clear();
    crnnNet.clear();
}

std::vector<TextBox>
OcrLite::getTextBoxes(cv::Mat &src, ScaleParam &s,
                      float boxScoreThresh, float boxThresh, float minArea) {
    cv::Mat srcResize;
    cv::resize(src, srcResize, cv::Size(s.dstWidth, s.dstHeight));
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

    std::vector<TextBox> rsBoxes;
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
    reverse(rsBoxes.begin(), rsBoxes.end());
    return rsBoxes;
}

Angle scoreToAngle(const float *srcData, int w) {
    int angleIndex = 0;
    float maxValue = -1000.0f;
    for (int i = 0; i < w; i++) {
        if (i == 0)maxValue = srcData[i];
        else if (srcData[i] > maxValue) {
            angleIndex = i;
            maxValue = srcData[i];
        }
    }
    return Angle(angleIndex, maxValue);
}

Angle OcrLite::getAngle(cv::Mat &src) {
    ncnn::Mat input = ncnn::Mat::from_pixels(
            src.data, ncnn::Mat::PIXEL_BGR2RGB,
            src.cols, src.rows);
    input.substract_mean_normalize(meanValsAngle, normValsAngle);
    ncnn::Extractor extractor = angleNet.create_extractor();
    extractor.set_num_threads(numThread);
    extractor.input("input", input);
    ncnn::Mat out;
    extractor.extract("out", out);
    return scoreToAngle((float *) out.data, out.w);
}

TextLine OcrLite::scoreToTextLine(const float *srcData, int h, int w) {
    std::string strRes;
    int lastIndex = 0;
    int keySize = keys.size();
    std::vector<float> scores;
    for (int i = 0; i < h; i++) {
        //find max score
        int maxIndex = 0;
        float maxValue = -1000.f;
        for (int j = 0; j < w; j++) {
            if (srcData[i * w + j] > maxValue) {
                maxValue = srcData[i * w + j];
                maxIndex = j;
            }
        }
        if (maxIndex > 0 && maxIndex < keySize && (!(i > 0 && maxIndex == lastIndex))) {
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
    float scale = (float) crnnDstHeight / (float) src.rows;
    int dstWidth = int((float) src.cols * scale);

    cv::Mat srcResize;
    cv::resize(src, srcResize, cv::Size(dstWidth, crnnDstHeight));

    ncnn::Mat input = ncnn::Mat::from_pixels(
            srcResize.data, ncnn::Mat::PIXEL_BGR2GRAY,
            srcResize.cols, srcResize.rows);

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

    return scoreToTextLine((float *) blob263.data, blob263.h, blob263.w);
}

cv::Mat adjustAngleImg(cv::Mat &src, int dstWidth, int dstHeight) {
    cv::Mat srcResize;
    float scale = (float) dstHeight / (float) src.rows;
    int angleWidth = int((float) src.cols * scale);
    cv::resize(src, srcResize, cv::Size(angleWidth, dstHeight));
    cv::Mat srcFit = cv::Mat(dstHeight, dstWidth, CV_8UC3, cv::Scalar(255, 255, 255));
    if (angleWidth < dstWidth) {
        cv::Rect rect(0, 0, srcResize.cols, srcResize.rows);
        srcResize.copyTo(srcFit(rect));
    } else {
        cv::Rect rect(0, 0, dstWidth, dstHeight);
        srcResize(rect).copyTo(srcFit);
    }
    return srcFit;
}

OcrResult OcrLite::detect(cv::Mat &src, cv::Rect &originRect, ScaleParam &scale,
                          float boxScoreThresh, float boxThresh, float minArea,
                          float scaleWidth, float scaleHeight) {

    cv::Mat textBoxPaddingImg = src.clone();
    //文字框 线宽
    int thickness = getThickness(src);
    //图像文字分割
    LOGI("=====Start detect=====");
    LOGI("ScaleParam(sw:%d,sh:%d,dw:%d,dH%d,%f,%f)", scale.srcWidth, scale.srcHeight,
         scale.dstWidth, scale.dstHeight,
         scale.scaleWidth, scale.scaleHeight);

    double startTime = getCurrentTime();
    std::vector<TextBox> textBoxes = getTextBoxes(src, scale, boxScoreThresh, boxThresh,
                                                  minArea);
    LOGI("TextBoxesSize(%ld)\n", textBoxes.size());

    double endDbNetTime = getCurrentTime();
    double dbNetTime = endDbNetTime - startTime;
    LOGI("dbNetTime(%fms)\n", dbNetTime);

    std::vector<TextBlock> textBlocks;
    std::string strRes;
    for (int i = 0; i < textBoxes.size(); ++i) {
        LOGI("-----TextBox[%d] score(%f)-----\n", i, textBoxes[i].score);
        double startTextBox = getCurrentTime();
        cv::Mat partImg;
        cv::RotatedRect partRect = getPartRect(textBoxes[i].boxPoint, scaleWidth,
                                               scaleHeight);
        LOGI("partRect(center.x=%f, center.y=%f, width=%f, height=%f, angle=%f)\n",
             partRect.center.x, partRect.center.y,
             partRect.size.width, partRect.size.height,
             partRect.angle);

        RRLib::getRotRectImg(partRect, src, partImg);

        //drawTextBox
        drawTextBox(textBoxPaddingImg, partRect, thickness);
        LOGI("TextBoxPos([x: %d, y: %d], [x: %d, y: %d], [x: %d, y: %d], [x: %d, y: %d])\n",
             textBoxes[i].boxPoint[0].x, textBoxes[i].boxPoint[0].y,
             textBoxes[i].boxPoint[1].x, textBoxes[i].boxPoint[1].y,
             textBoxes[i].boxPoint[2].x, textBoxes[i].boxPoint[2].y,
             textBoxes[i].boxPoint[3].x, textBoxes[i].boxPoint[3].y);

        //Rotate Img
        if (partImg.rows > 1.5 * partImg.cols) {
            partImg = matRotateClockWise90(partImg);
        }

        //getAngle
        double startAngle = getCurrentTime();
        auto angleImg = adjustAngleImg(partImg, angleDstWidth, angleDstHeight);
        Angle angle = getAngle(angleImg);
        double endAngle = getCurrentTime();
        angle.time = endAngle - startAngle;

        //Log Angle
        LOGI("angle(index=%d, score=%f)\n", angle.index, angle.score);
        LOGI("getAngleTime(%fms)\n", angle.time);

        //Rotate Img
        if (angle.index == 0 || angle.index == 2) {
            partImg = matRotateClockWise180(partImg);
        }

        //getTextLine
        double startCrnnTime = getCurrentTime();
        TextLine textLine = getTextLine(partImg, angle.index);
        double endCrnnTime = getCurrentTime();
        textLine.time = endCrnnTime - startCrnnTime;

        //Log textLine
        LOGI("textLine(%s)\n", textLine.text.c_str());
        std::ostringstream txtScores;
        for (int s = 0; s < textLine.charScores.size(); ++s) {
            if (s == 0) txtScores << textLine.charScores[s];
            txtScores << " ," << textLine.charScores[s];
        }
        LOGI("textScores{%s}\n", std::string(txtScores.str()).c_str());
        LOGI("crnnTime(%fms)\n", textLine.time);

        //Log TextBox[i]Time
        double endTextBox = getCurrentTime();
        double timeTextBox = endTextBox - startTextBox;
        LOGI("TextBox[%i]Time(%fms)\n", i, timeTextBox);

        strRes.append(textLine.text);
        strRes.append("\n");

        TextBlock textBlock(textBoxes[i].boxPoint, textBoxes[i].score, angle.index, angle.score,
                            angle.time, textLine.text, textLine.charScores, textLine.time,
                            timeTextBox);
        textBlocks.emplace_back(textBlock);
    }
    double endTime = getCurrentTime();
    double fullTime = endTime - startTime;
    LOGI("=====End detect=====\n");
    LOGI("FullDetectTime(%fms)\n", fullTime);

    //cropped to original size
    cv::Mat textBoxImg;
    if (originRect.x > 0 && originRect.y > 0) {
        textBoxPaddingImg(originRect).copyTo(textBoxImg);
    } else {
        textBoxImg = textBoxPaddingImg;
    }

    return OcrResult(textBlocks, dbNetTime, textBoxImg, fullTime, strRes);
}
