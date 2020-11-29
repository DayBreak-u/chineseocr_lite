#include "OcrLite.h"
#include "OcrUtils.h"
#include <stdarg.h> //windows&linux

OcrLite::OcrLite(int numOfThread) {
    numThread = numOfThread;
}

OcrLite::~OcrLite() {
    if (isOutputResultTxt) {
        fclose(resultTxt);
    }
}

void OcrLite::initLogger(bool isConsole, bool isPartImg, bool isResultImg) {
    isOutputConsole = isConsole;
    isOutputPartImg = isPartImg;
    isOutputResultImg = isResultImg;
}

void OcrLite::enableResultTxt(const char *path, const char *imgName) {
    isOutputResultTxt = true;
    std::string resultTxtPath = getResultTxtFilePath(path, imgName);
    printf("resultTxtPath(%s)\n", resultTxtPath.c_str());
    resultTxt = fopen(resultTxtPath.c_str(), "w");
}

bool OcrLite::initModels(const char *path) {
    Logger("=====Init Models=====\n");
    std::string pathStr = path;
    dbNet.setNumOfThreads(numThread);
    bool retDbNet = dbNet.initModel(pathStr);

    angleNet.setNumOfThreads(numThread);
    bool retAngleNet = angleNet.initModel(pathStr);

    crnnNet.setNumOfThreads(numThread);
    bool retCrnnNet = crnnNet.initModel(pathStr);

    if (!retDbNet || !retAngleNet || !retCrnnNet) {
        Logger("Init Models Failed! %d  %d  %d\n", retDbNet, retAngleNet, retCrnnNet);
    }
    return true;
}

void OcrLite::Logger(const char *format, ...) {
    if (!(isOutputConsole || isOutputResultTxt)) return;
    char *buffer = (char *) malloc(8192);
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    if (isOutputConsole) printf("%s", buffer);
    if (isOutputResultTxt) fprintf(resultTxt, "%s", buffer);
    free(buffer);
}

cv::Mat makePadding(cv::Mat &src, const int padding) {
    if (padding <= 0) return src;
    cv::Scalar paddingScalar = {255, 255, 255};
    cv::Mat paddingSrc;
    cv::copyMakeBorder(src, paddingSrc, padding, padding, padding, padding, cv::BORDER_ISOLATED, paddingScalar);
    return paddingSrc;
}

OcrResult OcrLite::detect(const char *path, const char *imgName,
                          const int padding, const int imgResize,
                          float boxScoreThresh, float boxThresh, float minArea,
                          float unClipRatio, bool doAngle, bool mostAngle) {
    std::string imgFile = getSrcImgFilePath(path, imgName);
    cv::Mat bgrSrc = imread(imgFile, cv::IMREAD_COLOR);//default : BGR
    cv::Mat originSrc;
    cvtColor(bgrSrc, originSrc, cv::COLOR_BGR2RGB);// convert to RGB
    cv::Rect originRect(padding, padding, originSrc.cols, originSrc.rows);
    cv::Mat src = makePadding(originSrc, padding);

    int resize;
    if (imgResize <= 0) {
        resize = (std::max)(src.cols, src.rows);
    } else {
        resize = imgResize;
    }

    ScaleParam scale = getScaleParam(src, resize);
    OcrResult result;
    result = detect(path, imgName, src, originRect, scale,
                    boxScoreThresh, boxThresh, minArea, unClipRatio, doAngle, mostAngle);

    /*double startTest = getCurrentTime();
    int loopCount = 100;
    for (int i = 0; i < loopCount; ++i) {
        detect(path, imgName, src, originRect, scale,
                        boxScoreThresh, boxThresh, minArea, unClipRatio, doAngle, mostAngle);
    }
    double endTest = getCurrentTime();
    printf("average time=%f\n", (endTest - startTest) / loopCount);*/

    return result;
}

std::vector<cv::Mat> OcrLite::getPartImages(cv::Mat &src, std::vector<TextBox> &textBoxes,
                                            const char *path, const char *imgName) {
    std::vector<cv::Mat> partImages;
    for (int i = 0; i < textBoxes.size(); ++i) {
        cv::Mat partImg = GetRotateCropImage(src, textBoxes[i].boxPoint);
        partImages.emplace_back(partImg);
        //OutPut DebugImg
        if (isOutputPartImg) {
            std::string debugImgFile = getDebugImgFilePath(path, imgName, i, "-part-");
            saveImg(partImg, debugImgFile.c_str());
        }
    }
    return partImages;
}

OcrResult OcrLite::detect(const char *path, const char *imgName,
                          cv::Mat &src, cv::Rect &originRect, ScaleParam &scale,
                          float boxScoreThresh, float boxThresh, float minArea,
                          float unClipRatio, bool doAngle, bool mostAngle) {

    cv::Mat textBoxPaddingImg = src.clone();
    int thickness = getThickness(src);

    Logger("=====Start detect=====\n");
    Logger("ScaleParam(sw:%d,sh:%d,dw:%d,dh:%d,%f,%f)\n", scale.srcWidth, scale.srcHeight,
           scale.dstWidth, scale.dstHeight,
           scale.scaleWidth, scale.scaleHeight);

    Logger("---------- step: dbNet getTextBoxes ----------\n");
    double startTime = getCurrentTime();
    std::vector<TextBox> textBoxes = dbNet.getTextBoxes(src, scale, boxScoreThresh, boxThresh, minArea, unClipRatio);
    Logger("TextBoxesSize(%ld)\n", textBoxes.size());
    double endDbNetTime = getCurrentTime();
    double dbNetTime = endDbNetTime - startTime;
    Logger("dbNetTime(%fms)\n", dbNetTime);

    for (int i = 0; i < textBoxes.size(); ++i) {
        Logger("TextBox[%d][score(%f),[x: %d, y: %d], [x: %d, y: %d], [x: %d, y: %d], [x: %d, y: %d]]\n", i,
               textBoxes[i].score,
               textBoxes[i].boxPoint[0].x, textBoxes[i].boxPoint[0].y,
               textBoxes[i].boxPoint[1].x, textBoxes[i].boxPoint[1].y,
               textBoxes[i].boxPoint[2].x, textBoxes[i].boxPoint[2].y,
               textBoxes[i].boxPoint[3].x, textBoxes[i].boxPoint[3].y);
    }

    Logger("---------- step: drawTextBoxes ----------\n");
    drawTextBoxes(textBoxPaddingImg, textBoxes, thickness);

    //---------- getPartImages ----------
    std::vector<cv::Mat> partImages = getPartImages(src, textBoxes, path, imgName);

    Logger("---------- step: angleNet getAngles ----------\n");
    std::vector<Angle> angles;
    angles = angleNet.getAngles(partImages, path, imgName, doAngle, mostAngle);

    //Log Angles
    for (int i = 0; i < angles.size(); ++i) {
        Logger("angle[%d][index(%d), score(%f), time(%fms)]\n", i, angles[i].index, angles[i].score, angles[i].time);
    }

    //Rotate partImgs
    for (int i = 0; i < partImages.size(); ++i) {
        if (angles[i].index == 0) {
            partImages.at(i) = matRotateClockWise180(partImages[i]);
        }
    }

    Logger("---------- step: crnnNet getTextLine ----------\n");
    std::vector<TextLine> textLines = crnnNet.getTextLines(partImages, path, imgName);
    //Log TextLines
    for (int i = 0; i < textLines.size(); ++i) {
        Logger("textLine[%d](%s)\n", i, textLines[i].text.c_str());
        std::ostringstream txtScores;
        for (int s = 0; s < textLines[i].charScores.size(); ++s) {
            if (s == 0) {
                txtScores << textLines[i].charScores[s];
            } else {
                txtScores << " ," << textLines[i].charScores[s];
            }
        }
        Logger("textScores[%d]{%s}\n", i, std::string(txtScores.str()).c_str());
        Logger("crnnTime[%d](%fms)\n", i, textLines[i].time);
    }

    std::vector<TextBlock> textBlocks;
    for (int i = 0; i < textLines.size(); ++i) {
        TextBlock textBlock{textBoxes[i].boxPoint, textBoxes[i].score, angles[i].index, angles[i].score,
                            angles[i].time, textLines[i].text, textLines[i].charScores, textLines[i].time,
                            angles[i].time + textLines[i].time};
        textBlocks.emplace_back(textBlock);
    }

    double endTime = getCurrentTime();
    double fullTime = endTime - startTime;
    Logger("=====End detect=====\n");
    Logger("FullDetectTime(%fms)\n", fullTime);

    //cropped to original size
    cv::Mat rgbBoxImg, textBoxImg;

    if (originRect.x > 0 && originRect.y > 0) {
        textBoxPaddingImg(originRect).copyTo(rgbBoxImg);
    } else {
        rgbBoxImg = textBoxPaddingImg;
    }
    cvtColor(rgbBoxImg, textBoxImg, cv::COLOR_RGB2BGR);//convert to BGR for Output Result Img

    //Save result.jpg
    if (isOutputResultImg) {
        std::string resultImgFile = getResultImgFilePath(path, imgName);
        imwrite(resultImgFile, textBoxImg);
    }

    std::string strRes;
    for (int i = 0; i < textBlocks.size(); ++i) {
        strRes.append(textBlocks[i].text);
        strRes.append("\n");
    }

    return OcrResult{dbNetTime, textBlocks, textBoxImg, fullTime, strRes};
}
