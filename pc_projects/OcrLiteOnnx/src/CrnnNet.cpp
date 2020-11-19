#include "CrnnNet.h"
#include "OcrUtils.h"
#include <fstream>
#include <numeric>

CrnnNet::~CrnnNet() {
    session.release();
}

bool CrnnNet::initModel(string &pathStr, Env &env, SessionOptions &sessionOptions) {
#ifdef _WIN32
    wstring crnnPath = strToWstr(pathStr + "/crnn_lite_lstm.onnx");
    session = makeUnique<Session>(env, crnnPath.c_str(), sessionOptions);
#else
    session = makeUnique<Session>(env, (pathStr + "/crnn_lite_lstm.onnx").c_str(), sessionOptions);
#endif
    inputNames = getInputNames(session);
    outputNames = getOutputNames(session);

    //load keys
    ifstream in((pathStr + "/keys.txt").c_str());
    string line;
    int keysSize = 0;
    if (in) {
        while (getline(in, line)) {// line中不包括每行的换行符
            keys.push_back(line);
        }
        keysSize = keys.size();
    } else {
        printf("The keys.txt file was not found\n");
    }

    printf("keys size(%d)\n", keysSize);
    if (keysSize != 5531) return false;

    return true;
}

TextLine CrnnNet::scoreToTextLine(const float *srcData, int h, int w) {
    string strRes;
    int lastIndex = 0;
    int keySize = keys.size();
    vector<float> scores;
    for (int i = 0; i < h; i++) {
        int maxIndex = 0;
        float maxValue = -1000.f;
        //do softmax
        vector<float> exps(w);
        for (int j = 0; j < w; j++) {
            float expSingle = exp(srcData[i * w + j]);
            exps.at(j) = expSingle;
        }
        float partition = accumulate(exps.begin(), exps.end(), 0.0);//row sum
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

TextLine CrnnNet::getTextLine(Mat &src) {
    float scale = (float) dstHeight / (float) src.rows;
    int dstWidth = int((float) src.cols * scale);

    Mat srcResize;
    resize(src, srcResize, Size(dstWidth, dstHeight));

    vector<float> inputTensorValues = substractMeanNormalize(srcResize, meanValues, normValues);

    array<int64_t, 4> inputShape{1, srcResize.channels(), srcResize.rows, srcResize.cols};

    auto memoryInfo = MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);


    Value inputTensor = Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(),
                                                   inputTensorValues.size(), inputShape.data(),
                                                   inputShape.size());
    assert(inputTensor.IsTensor());

    auto outputTensor = session->Run(RunOptions{nullptr}, inputNames.data(), &inputTensor,
                                     inputNames.size(),
                                     outputNames.data(), outputNames.size());

    assert(outputTensor.size() == 1 && outputTensor.front().IsTensor());

    size_t count = outputTensor.front().GetTensorTypeAndShapeInfo().GetElementCount();
    size_t rows = count / crnnCols;
    float *floatArray = outputTensor.front().GetTensorMutableData<float>();

    Mat score(rows, crnnCols, CV_32FC1);
    memcpy(score.data, floatArray, rows * crnnCols * sizeof(float));

    return scoreToTextLine((float *) score.data, rows, crnnCols);
}

vector<TextLine> CrnnNet::getTextLines(vector<Mat> &partImg, const char *path, const char *imgName) {
    vector<TextLine> textLines;
    for (int i = 0; i < partImg.size(); ++i) {
        //OutPut DebugImg
        if (isOutputDebugImg) {
            string debugImgFile = getDebugImgFilePath(path, imgName, i, "-debug-");
            saveImg(partImg[i], debugImgFile.c_str());
        }

        //getTextLine
        double startCrnnTime = getCurrentTime();
        TextLine textLine = getTextLine(partImg[i]);
        double endCrnnTime = getCurrentTime();
        textLine.time = endCrnnTime - startCrnnTime;

        //Log textLine
        //Logger("textLine[%d](%s)\n", i, textLine.text.c_str());
        textLines.emplace_back(textLine);
        ostringstream txtScores;
        for (int s = 0; s < textLine.charScores.size(); ++s) {
            if (s == 0) {
                txtScores << textLine.charScores[s];
            } else {
                txtScores << " ," << textLine.charScores[s];
            }
        }
        //Logger("textScores[%d]{%s}\n", i, string(txtScores.str()).c_str());
        //Logger("crnnTime[%d](%fms)\n", i, textLine.time);
    }
    return textLines;
}