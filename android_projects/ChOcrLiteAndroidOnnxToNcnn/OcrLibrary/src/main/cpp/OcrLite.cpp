#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "OcrLite.h"
#include "OcrUtils.h"

OcrLite::OcrLite(JNIEnv *env, jobject assetManager, int numOfThread) {
    AAssetManager *mgr = AAssetManager_fromJava(env, assetManager);
    if (mgr == NULL) {
        LOGE(" %s", "AAssetManager==NULL");
    }

    dbNet.setNumOfThreads(numOfThread);
    bool retDbNet = dbNet.initModel(mgr);

    angleNet.setNumOfThreads(numOfThread);
    bool retAngleNet = angleNet.initModel(mgr);

    crnnNet.setNumOfThreads(numOfThread);
    bool retCrnnNet = crnnNet.initModel(mgr);

    if (!retDbNet || !retAngleNet || !retCrnnNet) {
        LOGE("Init Models Failed! %d  %d  %d", retDbNet, retAngleNet, retCrnnNet);
    }

    LOGI("Init Models Success!");
}

void OcrLite::initLogger(bool isDebug) {
    isLOG = isDebug;
}

void OcrLite::Logger(const char *format, ...) {
    if (!isLOG) return;
    char *buffer = (char *) malloc(4096);
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    if (isLOG) LOGI("%s", buffer);
    free(buffer);
}

void OcrLite::drawTextBoxes(Mat &boxImg, vector<TextBox> &textBoxes, int thickness) {
    for (int i = 0; i < textBoxes.size(); ++i) {
        drawTextBox(boxImg, textBoxes[i].boxPoint, thickness);
        Logger("TextBoxPos[%d]([x: %d, y: %d], [x: %d, y: %d], [x: %d, y: %d], [x: %d, y: %d])",
               i,
               textBoxes[i].boxPoint[0].x, textBoxes[i].boxPoint[0].y,
               textBoxes[i].boxPoint[1].x, textBoxes[i].boxPoint[1].y,
               textBoxes[i].boxPoint[2].x, textBoxes[i].boxPoint[2].y,
               textBoxes[i].boxPoint[3].x, textBoxes[i].boxPoint[3].y);
    }
}

vector<Mat> OcrLite::getPartImages(Mat &src, vector<TextBox> &textBoxes) {
    vector<Mat> partImages;
    for (int i = 0; i < textBoxes.size(); ++i) {
        Mat partImg = GetRotateCropImage(src, textBoxes[i].boxPoint);
        partImages.emplace_back(partImg);
    }
    return partImages;
}

OcrResult OcrLite::detect(Mat &src, Rect &originRect, ScaleParam &scale,
                          float boxScoreThresh, float boxThresh, float minArea,
                          float unClipRatio, bool doAngle, bool mostAngle) {

    Mat textBoxPaddingImg = src.clone();
    int thickness = getThickness(src);

    Logger("=====Start detect=====");
    Logger("ScaleParam(sw:%d,sh:%d,dw:%d,dh:%d,%f,%f)", scale.srcWidth, scale.srcHeight,
           scale.dstWidth, scale.dstHeight,
           scale.scaleWidth, scale.scaleHeight);

    Logger("---------- step: dbNet getTextBoxes ----------");
    double startTime = getCurrentTime();
    vector<TextBox> textBoxes = dbNet.getTextBoxes(src, scale, boxScoreThresh,
                                                   boxThresh, minArea, unClipRatio);
    Logger("TextBoxesSize(%ld)", textBoxes.size());
    double endDbNetTime = getCurrentTime();
    double dbNetTime = endDbNetTime - startTime;
    Logger("dbNetTime(%fms)", dbNetTime);

    Logger("---------- step: drawTextBoxes ----------");
    drawTextBoxes(textBoxPaddingImg, textBoxes, thickness);

    //---------- getPartImages ----------
    vector<Mat> partImages = getPartImages(src, textBoxes);

    Logger("---------- step: angleNet getAngles ----------");
    vector<Angle> angles;
    angles = angleNet.getAngles(partImages, doAngle, mostAngle);

    Logger("---------- step: crnnNet getTextLine ----------");
    vector<TextLine> textLines = crnnNet.getTextLines(partImages);

    vector<TextBlock> textBlocks;
    for (int i = 0; i < textLines.size(); ++i) {
        TextBlock textBlock(textBoxes[i].boxPoint, textBoxes[i].score, angles[i].index,
                            angles[i].score,
                            angles[i].time, textLines[i].text, textLines[i].charScores,
                            textLines[i].time,
                            angles[i].time + textLines[i].time);
        textBlocks.emplace_back(textBlock);
    }

    double endTime = getCurrentTime();
    double fullTime = endTime - startTime;
    Logger("=====End detect=====");
    Logger("FullDetectTime(%fms)", fullTime);

    //cropped to original size
    Mat textBoxImg;
    if (originRect.x > 0 && originRect.y > 0) {
        textBoxPaddingImg(originRect).copyTo(textBoxImg);
    } else {
        textBoxImg = textBoxPaddingImg;
    }

    string strRes;
    for (int i = 0; i < textBlocks.size(); ++i) {
        strRes.append(textBlocks[i].text);
        strRes.append("\n");
    }

    return OcrResult(textBlocks, dbNetTime, textBoxImg, fullTime, strRes);
}
