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
    dbNet->release();
    angleNet->release();
    crnnNet->release();
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

std::vector<const char *> getInputNames(std::unique_ptr<Ort::Session> &session) {
    Ort::AllocatorWithDefaultOptions allocator;
    size_t numInputNodes = session->GetInputCount();
    std::vector<const char *> inputNodeNames(numInputNodes);
    std::vector<int64_t> inputNodeDims;

    //printf("Number of inputs = %zu\n", numInputNodes);

    for (int i = 0; i < numInputNodes; i++) {
        // print input node names
        char *inputName = session->GetInputName(i, allocator);
        //printf("Input %d : name=%s\n", i, inputName);
        inputNodeNames[i] = inputName;

        // print input node types
        Ort::TypeInfo typeInfo = session->GetInputTypeInfo(i);
        auto tensorInfo = typeInfo.GetTensorTypeAndShapeInfo();

        ONNXTensorElementDataType type = tensorInfo.GetElementType();
        //printf("Input %d : type=%d\n", i, type);

        // print input shapes/dims
        inputNodeDims = tensorInfo.GetShape();
        //printf("Input %d : num_dims=%zu\n", i, inputNodeDims.size());
        /*for (int j = 0; j < inputNodeDims.size(); j++)
            printf("Input %d : dim %d=%jd\n", i, j, inputNodeDims[j]);*/
    }
    return inputNodeNames;
}

std::vector<const char *> getOutputNames(std::unique_ptr<Ort::Session> &session) {
    Ort::AllocatorWithDefaultOptions allocator;
    size_t numOutputNodes = session->GetOutputCount();
    std::vector<const char *> outputNodeNames(numOutputNodes);
    std::vector<int64_t> outputNodeDims;

    //printf("Number of outputs = %zu\n", numOutputNodes);

    for (int i = 0; i < numOutputNodes; i++) {
        // print input node names
        char *outputName = session->GetOutputName(i, allocator);
        //printf("Input %d : name=%s\n", i, outputName);
        outputNodeNames[i] = outputName;

        // print input node types
        Ort::TypeInfo type_info = session->GetOutputTypeInfo(i);
        auto tensorInfo = type_info.GetTensorTypeAndShapeInfo();

        ONNXTensorElementDataType type = tensorInfo.GetElementType();
        //printf("Output %d : type=%d\n", i, type);

        // print input shapes/dims
        outputNodeDims = tensorInfo.GetShape();
        //printf("Output %d : num_dims=%zu\n", i, outputNodeDims.size());
        /*for (int j = 0; j < outputNodeDims.size(); j++)
            printf("Output %d : dim %d=%jd\n", i, j, outputNodeDims[j]);*/
    }
    return outputNodeNames;
}

template<typename T, typename... Ts>
std::unique_ptr<T> makeUnique(Ts &&... params) {
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

bool OcrLite::initModels(const char *path) {
    Logger("=====init Models=====\n");
    std::string pathStr = path;

    //===session options===
    // Sets the number of threads used to parallelize the execution within nodes
    // A value of 0 means ORT will pick a default
    //sessionOptions.SetIntraOpNumThreads(numThread);
    //set OMP_NUM_THREADS=16

    // Sets the number of threads used to parallelize the execution of the graph (across nodes)
    // If sequential execution is enabled this value is ignored
    // A value of 0 means ORT will pick a default
    sessionOptions.SetInterOpNumThreads(numThread);

    // Sets graph optimization level
    // ORT_DISABLE_ALL -> To disable all optimizations
    // ORT_ENABLE_BASIC -> To enable basic optimizations (Such as redundant node removals)
    // ORT_ENABLE_EXTENDED -> To enable extended optimizations (Includes level 1 + more complex optimizations like node fusions)
    // ORT_ENABLE_ALL -> To Enable All possible opitmizations
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

    Logger("--- init dbNet ---\n");
#ifdef _WIN32
    std::wstring dbPath = strToWstr(pathStr + "/dbnet.onnx");
    dbNet = makeUnique<Ort::Session>(env, dbPath.c_str(), sessionOptions);
#else
    dbNet = makeUnique<Ort::Session>(env, (pathStr + "/dbnet.onnx").c_str(), sessionOptions);
#endif
    dbNetInputNames = getInputNames(dbNet);
    dbNetOutputNames = getOutputNames(dbNet);

    Logger("--- init angleNet ---\n");
#ifdef _WIN32
    std::wstring anglePath = strToWstr(pathStr + "/angle_net.onnx");
    angleNet = makeUnique<Ort::Session>(env, anglePath.c_str(), sessionOptions);
#else
    angleNet = makeUnique<Ort::Session>(env, (pathStr + "/angle_net.onnx").c_str(), sessionOptions);
#endif
    angleNetInputNames = getInputNames(angleNet);
    angleNetOutputNames = getOutputNames(angleNet);

    Logger("--- init crnnNet ---\n");
#ifdef _WIN32
    std::wstring crnnPath = strToWstr(pathStr + "/crnn_lite_lstm.onnx");
    crnnNet = makeUnique<Ort::Session>(env, crnnPath.c_str(), sessionOptions);
#else
    crnnNet = makeUnique<Ort::Session>(env, (pathStr + "/crnn_lite_lstm.onnx").c_str(), sessionOptions);
#endif
    crnnNetInputNames = getInputNames(crnnNet);
    crnnNetOutputNames = getOutputNames(crnnNet);

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
    std::vector<float> inputTensorValues = substractMeanNormalize(srcResize, meanValsDBNet, normValsDBNet);

    std::array<int64_t, 4> inputShape{1, srcResize.channels(), srcResize.rows, srcResize.cols};

    auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(),
                                                             inputTensorValues.size(), inputShape.data(),
                                                             inputShape.size());
    assert(inputTensor.IsTensor());

    auto outputTensor = dbNet->Run(Ort::RunOptions{nullptr}, dbNetInputNames.data(), &inputTensor,
                                   dbNetInputNames.size(),
                                   dbNetOutputNames.data(), dbNetOutputNames.size());

    assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());

    float *floatArray = outputTensor.front().GetTensorMutableData<float>();

    cv::Mat fMapMat(srcResize.rows, srcResize.cols, CV_32FC1);
    memcpy(fMapMat.data, floatArray, srcResize.rows * srcResize.cols * sizeof(float));

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

    std::vector<float> inputTensorValues = substractMeanNormalize(src, meanValsAngle, normValsAngle);

    std::array<int64_t, 4> inputShape{1, src.channels(), src.rows, src.cols};

    auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(),
                                                             inputTensorValues.size(), inputShape.data(),
                                                             inputShape.size());
    assert(inputTensor.IsTensor());

    auto outputTensor = angleNet->Run(Ort::RunOptions{nullptr}, angleNetInputNames.data(), &inputTensor,
                                      angleNetInputNames.size(),
                                      angleNetOutputNames.data(), angleNetOutputNames.size());

    assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());

    size_t count = outputTensor.front().GetTensorTypeAndShapeInfo().GetElementCount();
    size_t rows = count / angleCols;
    float *floatArray = outputTensor.front().GetTensorMutableData<float>();

    cv::Mat score(rows, angleCols, CV_32FC1);
    memcpy(score.data, floatArray, rows * angleCols * sizeof(float));

    return scoreToAngle((float *) score.data, angleCols);
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

    std::vector<float> inputTensorValues = substractMeanNormalize(srcResize, meanValsCrnn, normValsCrnn);

    std::array<int64_t, 4> inputShape{1, srcResize.channels(), srcResize.rows, srcResize.cols};

    auto memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(),
                                                             inputTensorValues.size(), inputShape.data(),
                                                             inputShape.size());
    assert(inputTensor.IsTensor());

    auto outputTensor = crnnNet->Run(Ort::RunOptions{nullptr}, crnnNetInputNames.data(), &inputTensor,
                                     crnnNetInputNames.size(),
                                     crnnNetOutputNames.data(), crnnNetOutputNames.size());

    assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());

    size_t count = outputTensor.front().GetTensorTypeAndShapeInfo().GetElementCount();
    size_t rows = count / crnnCols;
    float *floatArray = outputTensor.front().GetTensorMutableData<float>();

    cv::Mat score(rows, crnnCols, CV_32FC1);
    memcpy(score.data, floatArray, rows * crnnCols * sizeof(float));

    return scoreToTextLine((float *) score.data, rows, crnnCols);
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
        if (angle.index == 0 || angle.index == 2) {
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