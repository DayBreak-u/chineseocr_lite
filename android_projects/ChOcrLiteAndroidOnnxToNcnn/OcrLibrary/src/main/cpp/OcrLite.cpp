#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "OcrLite.h"
#include "OcrUtils.h"
#include <iosfwd>
#include <opencv/cv.hpp>
#include <numeric>

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

    int ret1 = dbNet.load_param(mgr, "dbnet_op.param");
    int ret2 = dbNet.load_model(mgr, "dbnet_op.bin");

    int ret3 = crnnNet.load_param(mgr, "crnn_lite_op.param");
    int ret4 = crnnNet.load_model(mgr, "crnn_lite_op.bin");

    int ret5 = angleNet.load_param(mgr, "angle_op.param");
    int ret6 = angleNet.load_model(mgr, "angle_op.bin");
    if (ret1 != 0 || ret2 != 0 || ret3 != 0 || ret4 != 0 ||
        ret5 != 0 || ret6 != 0) {
        LOGE("# %d  %d  %d  %d %d  %d", ret1, ret2, ret3, ret4, ret5, ret6);
    }

    char *buffer = readKeysFromAssets(mgr);
    if (buffer != NULL) {
        std::istringstream inStr(buffer);
        std::string line;
        while (getline(inStr, line)) {
            keys.emplace_back(line);
        }
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
}

std::vector<TextBox>
OcrLite::getTextBoxes(cv::Mat &src, ScaleParam &s,
                      float boxScoreThresh, float boxThresh, float minArea, float unClipRatio) {
    cv::Mat srcResize;
    cv::resize(src, srcResize, cv::Size(s.dstWidth, s.dstHeight));

    ncnn::Mat input = ncnn::Mat::from_pixels(srcResize.data, ncnn::Mat::PIXEL_BGR2RGB,
                                             srcResize.cols, srcResize.rows);

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
        //---use clipper start---
        std::vector<cv::Point> newbox;
        unClip(minBox, allEdgeSize, newbox, unClipRatio);

        getMiniBoxes(newbox, minBox, minEdgeSize, allEdgeSize);

        if (minEdgeSize < minArea + 2)
            continue;
        //---use clipper end---

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
        int maxIndex = 0;
        float maxValue = -1000.f;

        //do softmax
        std::vector<float> exps(w);
        for (int j = 0; j < w; j++) {
            float expSingle = exp(srcData[i * w + j]);
            exps.at(j) = expSingle;
        }
        float partition = std::accumulate(exps.begin(), exps.end(), 0.0);//row sum
        for (int j = 0; j < w; j++) {
            float softmax = exps[j] / partition;
            if (softmax > maxValue) {
                maxValue = softmax;
                maxIndex = j;
            }
        }
        //no softmax
        /*for (int j = 0; j < w; j++) {
            if (srcData[i * w + j] > maxValue) {
                maxValue = srcData[i * w + j];
                maxIndex = j;
            }
        }*/
        if (maxIndex > 0 && maxIndex < keySize && (!(i > 0 && maxIndex == lastIndex))) {
            scores.emplace_back(maxValue);
            strRes.append(keys[maxIndex - 1]);
        }
        lastIndex = maxIndex;
    }
    return TextLine(strRes, scores);
}

TextLine OcrLite::getTextLine(cv::Mat &src) {
    float scale = (float) crnnDstHeight / (float) src.rows;
    int dstWidth = int((float) src.cols * scale);

    cv::Mat srcResize;
    cv::resize(src, srcResize, cv::Size(dstWidth, crnnDstHeight));

    ncnn::Mat input = ncnn::Mat::from_pixels(
            srcResize.data, ncnn::Mat::PIXEL_BGR2GRAY,
            srcResize.cols, srcResize.rows);

    input.substract_mean_normalize(meanValsCrnn, normValsCrnn);

    ncnn::Extractor extractor = crnnNet.create_extractor();
    extractor.set_num_threads(numThread);
    extractor.input("input", input);

    // lstm
    ncnn::Mat blob162;
    extractor.extract("1000", blob162);

    // batch fc
    ncnn::Mat blob263(5531, blob162.h);
    for (int i = 0; i < blob162.h; i++) {
        ncnn::Extractor extractor2 = crnnNet.create_extractor();
        extractor2.set_num_threads(numThread);
        ncnn::Mat blob243_i = blob162.row_range(i, 1);
        extractor2.input("1014", blob243_i);

        ncnn::Mat blob263_i;
        extractor2.extract("1015", blob263_i);

        memcpy(blob263.row(i), blob263_i, 5531 * sizeof(float));
    }

    return scoreToTextLine((float *) blob263.data, blob263.h, blob263.w);
}

void OcrLite::drawTextBoxes(cv::Mat &boxImg, std::vector<TextBox> &textBoxes, int thickness) {
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

std::vector<cv::Mat> OcrLite::getPartImages(cv::Mat &src, std::vector<TextBox> &textBoxes) {
    std::vector<cv::Mat> partImages;
    for (int i = 0; i < textBoxes.size(); ++i) {
        cv::Mat partImg = GetRotateCropImage(src, textBoxes[i].boxPoint);
        partImages.emplace_back(partImg);
    }
    return partImages;
}

std::vector<Angle>
OcrLite::getAngles(std::vector<cv::Mat> &partImgs, bool doAngle, bool mostAngle) {
    std::vector<Angle> angles;
    if (doAngle) {
        for (int i = 0; i < partImgs.size(); ++i) {
            //getAngle
            double startAngle = getCurrentTime();
            auto angleImg = adjustTargetImg(partImgs[i], angleDstWidth, angleDstHeight);
            Angle angle = getAngle(angleImg);
            double endAngle = getCurrentTime();
            angle.time = endAngle - startAngle;

            //Log Angle
            Logger("angle[%d](index=%d, score=%f time=%fms)", i, angle.index, angle.score,
                   angle.time);
            angles.emplace_back(angle);
        }
    } else {
        for (int i = 0; i < partImgs.size(); ++i) {
            Angle angle(-1, 0.f);
            //Log Angle
            Logger("angle[%d]Disabled(index=%d, score=%f time=%fms)", i, angle.index,
                   angle.score, angle.time);
            angles.emplace_back(angle);
        }
    }
    //Most Possible AngleIndex
    if (doAngle && mostAngle) {
        auto angleIndexes = getAngleIndexes(angles);
        double sum = std::accumulate(angleIndexes.begin(), angleIndexes.end(), 0.0);
        double halfPercent = angles.size() / 2.0f;
        int mostAngleIndex;
        if (sum < halfPercent) {//all angle set to 0
            mostAngleIndex = 0;
        } else {//all angle set to 1
            mostAngleIndex = 1;
        }
        Logger("Set All Angle to mostAngleIndex(%d)", mostAngleIndex);
        for (int i = 0; i < angles.size(); ++i) {
            Angle angle = angles[i];
            angle.index = mostAngleIndex;
            angles.at(i) = angle;
        }
    }
    //Rotate partImgs
    for (int i = 0; i < partImgs.size(); ++i) {
        if (angles[i].index == 0) {
            partImgs.at(i) = matRotateClockWise180(partImgs[i]);
        }
    }

    return angles;
}

std::vector<TextLine> OcrLite::getTextLines(std::vector<cv::Mat> &partImg) {
    std::vector<TextLine> textLines;
    for (int i = 0; i < partImg.size(); ++i) {
        //getTextLine
        double startCrnnTime = getCurrentTime();
        TextLine textLine = getTextLine(partImg[i]);
        double endCrnnTime = getCurrentTime();
        textLine.time = endCrnnTime - startCrnnTime;

        //Log textLine
        Logger("textLine[%d](%s)", i, textLine.text.c_str());
        textLines.emplace_back(textLine);
        std::ostringstream txtScores;
        for (int s = 0; s < textLine.charScores.size(); ++s) {
            if (s == 0) txtScores << textLine.charScores[s];
            txtScores << " ," << textLine.charScores[s];
        }
        Logger("textScores[%d]{%s}", i, std::string(txtScores.str()).c_str());
        Logger("crnnTime[%d](%fms)", i, textLine.time);
    }
    return textLines;
}

OcrResult OcrLite::detect(cv::Mat &src, cv::Rect &originRect, ScaleParam &scale,
                          float boxScoreThresh, float boxThresh, float minArea,
                          float unClipRatio, bool doAngle, bool mostAngle) {

    cv::Mat textBoxPaddingImg = src.clone();
    int thickness = getThickness(src);

    Logger("=====Start detect=====");
    Logger("ScaleParam(sw:%d,sh:%d,dw:%d,dH%d,%f,%f)", scale.srcWidth, scale.srcHeight,
           scale.dstWidth, scale.dstHeight,
           scale.scaleWidth, scale.scaleHeight);

    Logger("---------- step: dbNet getTextBoxes ----------");
    double startTime = getCurrentTime();
    std::vector<TextBox> textBoxes = getTextBoxes(src, scale, boxScoreThresh, boxThresh,
                                                  minArea, unClipRatio);
    Logger("TextBoxesSize(%ld)", textBoxes.size());
    double endDbNetTime = getCurrentTime();
    double dbNetTime = endDbNetTime - startTime;
    Logger("dbNetTime(%fms)", dbNetTime);

    Logger("---------- step: drawTextBoxes ----------");
    drawTextBoxes(textBoxPaddingImg, textBoxes, thickness);

    //---------- getPartImages ----------
    std::vector<cv::Mat> partImages = getPartImages(src, textBoxes);

    Logger("---------- step: angleNet getAngles ----------");
    std::vector<Angle> angles;
    angles = getAngles(partImages, doAngle, mostAngle);

    Logger("---------- step: crnnNet getTextLine ----------");
    std::vector<TextLine> textLines = getTextLines(partImages);

    std::vector<TextBlock> textBlocks;
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
    cv::Mat textBoxImg;
    if (originRect.x > 0 && originRect.y > 0) {
        textBoxPaddingImg(originRect).copyTo(textBoxImg);
    } else {
        textBoxImg = textBoxPaddingImg;
    }

    std::string strRes;
    for (int i = 0; i < textBlocks.size(); ++i) {
        strRes.append(textBlocks[i].text);
        strRes.append("\n");
    }

    return OcrResult(textBlocks, dbNetTime, textBoxImg, fullTime, strRes);
}
