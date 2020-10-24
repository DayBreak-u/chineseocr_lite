#include <iosfwd>
#include <opencv/cv.hpp>
#include <RRLib.h>
#include "OcrLite.h"
#include "OcrUtils.h"

OcrLite::OcrLite(int numOfThread) {
    numThread = numOfThread;
}

OcrLite::~OcrLite() {
    if (isOutputResultTxt) {
        fclose(resultTxt);
    }
    dbNet.clear();
    angleNet.clear();
    crnnNet.clear();
}

void OcrLite::initLogger(const char *path, const char *imgName, bool isConsole,
                         bool isPartImg, bool isAngleImg, bool isDebugImg, bool isResultTxt, bool isResultImg) {
    isOutputConsole = isConsole;
    isOutputPartImg = isPartImg;
    isOutputAngleImg = isAngleImg;
    isOutputDebugImg = isDebugImg;
    isOutputResultTxt = isResultTxt;
    isOutputResultImg = isResultImg;
    if (isResultTxt) {
        std::string resultTxtPath = getResultTxtFilePath(path, imgName);
        printf("resultTxtPath(%s)\n", resultTxtPath.c_str());
        resultTxt = fopen(resultTxtPath.c_str(), "w");
    }
}

bool OcrLite::initModels(const char *path) {
    Logger("=====init Models=====\n");
    std::string pathStr = path;
    int dbParam = dbNet.load_param((pathStr + "/dbnet_op.param").c_str());
    int dbModel = dbNet.load_model((pathStr + "/dbnet_op.bin").c_str());
    Logger("DBNet load param(%d), model(%d)\n", dbParam, dbModel);
    if (dbParam != 0 || dbModel != 0) return false;

    int angleParam = angleNet.load_param((pathStr + "/angle_op.param").c_str());
    int angleModel = angleNet.load_model((pathStr + "/angle_op.bin").c_str());
    Logger("AngleNet load param(%d), model(%d)\n", angleParam, angleModel);
    if (angleParam != 0 || angleModel != 0) return false;

    int crnnParam = crnnNet.load_param((pathStr + "/crnn_lite_op.param").c_str());
    int crnnModel = crnnNet.load_model((pathStr + "/crnn_lite_op.bin").c_str());
    Logger("CrnnNet load param(%d), model(%d)\n", crnnParam, crnnModel);
    if (crnnParam != 0 || crnnModel != 0) return false;

    //load keys
    std::ifstream in((pathStr + "/keys.txt").c_str());
    std::string line;
    int keysSize = 0;
    if (in) {
        while (getline(in, line)) {// line中不包括每行的换行符
            keys.push_back(line);
        }
        keysSize = keys.size();
    } else {
        Logger("The keys.txt file was not found\n");
    }

    Logger("keys size(%d)\n", keysSize);
    if (keysSize == 0) return false;
    Logger("Init Models Success!\n");
    return true;
}

void OcrLite::Logger(const char *format, ...) {
    if (!(isOutputConsole || isOutputResultTxt)) return;
    char *buffer = (char *) malloc(4096);
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    if (isOutputConsole) printf("%s", buffer);
    if (isOutputResultTxt) fprintf(resultTxt, "%s", buffer);
}

std::vector<TextBox>
OcrLite::getTextBoxes(cv::Mat &src, ScaleParam &s,
                      float boxScoreThresh, float boxThresh, float minArea) {
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

cv::Mat makePadding(cv::Mat &src, const int padding) {
    if (padding <= 0) return src;
    cv::Scalar paddingScalar = {255, 255, 255};
    cv::Mat paddingSrc;
    cv::copyMakeBorder(src, paddingSrc, padding, padding, padding, padding, cv::BORDER_ISOLATED, paddingScalar);
    return paddingSrc;
}

OcrResult OcrLite::detect(const char *path, const char *imgName,
                          const int padding, const float imgScale,
                          float boxScoreThresh, float boxThresh, float minArea,
                          float scaleWidth, float scaleHeight) {
    std::string imgFile = getSrcImgFilePath(path, imgName);

    cv::Mat originSrc = cv::imread(imgFile);
    cv::Rect originRect(padding, padding, originSrc.cols, originSrc.rows);
    cv::Mat src = makePadding(originSrc, padding);

    ScaleParam scale = getScaleParam(src, imgScale);

    return detect(path, imgName, src, originRect, scale,
                  boxScoreThresh, boxThresh, minArea, scaleWidth,
                  scaleHeight);
}

OcrResult OcrLite::detect(const char *path, const char *imgName,
                          const int padding, const int imgResize,
                          float boxScoreThresh, float boxThresh, float minArea,
                          float scaleWidth, float scaleHeight) {
    std::string imgFile = getSrcImgFilePath(path, imgName);

    cv::Mat originSrc = cv::imread(imgFile);
    cv::Rect originRect(padding, padding, originSrc.cols, originSrc.rows);
    cv::Mat src = makePadding(originSrc, padding);

    int resize;
    if (imgResize <= 0) {
        resize = (std::max)(src.cols, src.rows);
    } else {
        resize = imgResize;
    }

    ScaleParam scale = getScaleParam(src, resize);

    OcrResult result = detect(path, imgName, src, originRect, scale,
                              boxScoreThresh, boxThresh, minArea, scaleWidth,
                              scaleHeight);

    /*double startTest = getCurrentTime();
    for (int i = 0; i < 500; ++i) {
        detect(path, imgName, src, originRect, scale,
               boxScoreThresh, boxThresh, minArea, scaleWidth,
               scaleHeight);
    }
    double endTest = getCurrentTime();
    printf("time=%f\n", (endTest - startTest) / 500);*/

    return result;
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

OcrResult OcrLite::detect(const char *path, const char *imgName,
                          cv::Mat &src, cv::Rect &originRect, ScaleParam &scale,
                          float boxScoreThresh, float boxThresh, float minArea,
                          float scaleWidth, float scaleHeight) {

    cv::Mat textBoxPaddingImg = src.clone();
    int thickness = getThickness(src);

    Logger("=====Start detect=====\n");
    Logger("ScaleParam(sw:%d,sh:%d,dw:%d,dH%d,%f,%f)\n", scale.srcWidth, scale.srcHeight,
           scale.dstWidth, scale.dstHeight,
           scale.scaleWidth, scale.scaleHeight);

    double startTime = getCurrentTime();
    std::vector<TextBox> textBoxes = getTextBoxes(src, scale, boxScoreThresh, boxThresh, minArea);
    Logger("TextBoxesSize(%ld)\n", textBoxes.size());

    double endTextBoxesTime = getCurrentTime();
    double textBoxesTime = endTextBoxesTime - startTime;
    Logger("getTextBoxesTime(%fms)\n", textBoxesTime);

    std::vector<Angle> angles;
    std::vector<TextLine> textLines;
    std::string strRes;
    for (int i = 0; i < textBoxes.size(); ++i) {
        Logger("-----TextBox[%d] score(%f)-----\n", i, textBoxes[i].score);
        double startTextBox = getCurrentTime();
        cv::Mat partImg;
        cv::RotatedRect partRect = getPartRect(textBoxes[i].box, scaleWidth,
                                                scaleHeight);
        Logger("partRect(center.x=%f, center.y=%f, width=%f, height=%f, angle=%f)\n",
               partRect.center.x, partRect.center.y,
               partRect.size.width, partRect.size.height,
               partRect.angle);

        RRLib::getRotRectImg(partRect, src, partImg);

        //drawTextBox
        drawTextBox(textBoxPaddingImg, partRect, thickness);
        Logger("TextBoxPos([x: %d, y: %d], [x: %d, y: %d], [x: %d, y: %d], [x: %d, y: %d])\n",
               textBoxes[i].box[0].x, textBoxes[i].box[0].y,
               textBoxes[i].box[1].x, textBoxes[i].box[1].y,
               textBoxes[i].box[2].x, textBoxes[i].box[2].y,
               textBoxes[i].box[3].x, textBoxes[i].box[3].y);

        //OutPut PartImg
        if (isOutputPartImg) {
            std::string partImgFile = getDebugImgFilePath(path, imgName, i, "-part-");
            saveImg(partImg, partImgFile.c_str());
        }

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
        Logger("angle(index=%d, score=%f)\n", angle.index, angle.score);
        Logger("getAngleTime(%fms)\n", angle.time);
        angles.push_back(angle);

        //OutPut AngleImg
        if (isOutputAngleImg) {
            std::string angleImgFile = getDebugImgFilePath(path, imgName, i, "-angle-");
            saveImg(angleImg, angleImgFile.c_str());
        }

        //Rotate Img
        if (angle.index == 0) {
            partImg = matRotateClockWise180(partImg);
        }

        //OutPut DebugImg
        if (isOutputDebugImg) {
            std::string debugImgFile = getDebugImgFilePath(path, imgName, i, "-debug-");
            saveImg(partImg, debugImgFile.c_str());
        }

        //getTextLine
        double startTextLine = getCurrentTime();
        TextLine textLine = getTextLine(partImg);
        double endTextLine = getCurrentTime();
        textLine.time = endTextLine - startTextLine;

        //Log textLine
        Logger("textLine(%s)\n", textLine.line.c_str());
        std::ostringstream txtScores;
        for (int s = 0; s < textLine.scores.size(); ++s) {
            if (s == 0) txtScores << textLine.scores[s];
            txtScores << " ," << textLine.scores[s];
        }
        Logger("textScores{%s}\n", std::string(txtScores.str()).c_str());
        Logger("getTextLineTime(%fms)\n", textLine.time);
        textLines.push_back(textLine);

        //Log TextBox[i]Time
        double endTextBox = getCurrentTime();
        double timeTextBox = endTextBox - startTextBox;
        Logger("TextBox[%i]Time(%fms)\n", i, timeTextBox);

        strRes.append(textLine.line);
        strRes.append("\n");
    }
    double endTime = getCurrentTime();
    double fullTime = endTime - startTime;
    Logger("=====End detect=====\n");
    Logger("FullDetectTime(%fms)\n", fullTime);

    //cropped to original size
    cv::Mat textBoxImg;
    if (originRect.x > 0 && originRect.y > 0) {
        textBoxPaddingImg(originRect).copyTo(textBoxImg);
    } else {
        textBoxImg = textBoxPaddingImg;
    }

    //Save result.jpg
    if (isOutputResultImg) {
        std::string resultImgFile = getResultImgFilePath(path, imgName);
        cv::imwrite(resultImgFile, textBoxImg);
    }

    return OcrResult(textBoxes, textBoxesTime, angles, textLines, textBoxImg, strRes, fullTime);
}
