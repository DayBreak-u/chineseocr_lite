#include "CrnnNet.h"
#include "OcrUtils.h"
#include <numeric>

CrnnNet::CrnnNet() {}

CrnnNet::~CrnnNet() {
    delete session;
}

void CrnnNet::setNumThread(int numOfThread) {
    numThread = numOfThread;
}

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

bool CrnnNet::initModel(AAssetManager *mgr, const char* crnnpath) {
    net.reset(MNN::Interpreter::createFromFile(crnnpath));
    MNN::ScheduleConfig config;
    config.type  = MNN_FORWARD_CPU;
    config.numThread = numThread;
    session = net->createSession(config);

    //load keys
    char *buffer = readKeysFromAssets(mgr);
    if (buffer != NULL) {
        std::istringstream inStr(buffer);
        std::string line;
        int size = 0;
        while (getline(inStr, line)) {
            keys.emplace_back(line);
            size++;
        }
        free(buffer);
        LOGI("keys size(%d)", size);
    } else {
        LOGE(" txt file not found");
        return false;
    }

    return true;
}

TextLine CrnnNet::scoreToTextLine(const float *srcData, int h, int w) {
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
        float partition = accumulate(exps.begin(), exps.end(), 0.0);//row sum
        for (int j = 0; j < w; j++) {
            float softmax = exps[j] / partition;
            if (softmax > maxValue) {
                maxValue = softmax;
                maxIndex = j;
            }
        }

        if (maxIndex > 0 && maxIndex < keySize && (!(i > 0 && maxIndex == lastIndex))) {
            scores.emplace_back(maxValue);
            strRes.append(keys[maxIndex - 1]);
        }
        lastIndex = maxIndex;
    }
    return {strRes, scores};
}

TextLine CrnnNet::getTextLine(cv::Mat &src) {
    float scale = (float) dstHeight / (float) src.rows;
    int dstWidth = int((float) src.cols * scale);

    cv::Mat srcResize;
    resize(src, srcResize, cv::Size(dstWidth, dstHeight));

    std::vector<float> inputTensorValues = substractMeanNormalize(srcResize, meanValues, normValues);

    std::vector<int>inputShape = {1, srcResize.channels(), srcResize.rows, srcResize.cols};
    auto input = net->getSessionInput(session, "input");
    auto output = net->getSessionOutput(session, "out");
    auto shape = input->shape();
    shape[0]   = 1;
    shape[2]   = srcResize.rows;
    shape[3]   = srcResize.cols;
    net->resizeTensor(input, shape);
    net->resizeSession(session);
    auto nchwTensor = MNN::Tensor::create(inputShape,halide_type_of<float>(),&inputTensorValues[0],MNN::Tensor::CAFFE);
    input->copyFromHostTensor(nchwTensor);
    delete nchwTensor;
    net->runSession(session);

    std::shared_ptr<MNN::Tensor> outputUser(new MNN::Tensor(output, output->getDimensionType())); //nchw
    output->copyToHostTensor(outputUser.get());
    auto values = outputUser->host<float>();

    return scoreToTextLine(values, output->shape()[0], output->shape()[2]);
}

std::vector<TextLine> CrnnNet::getTextLines(std::vector<cv::Mat> &partImg) {
    int size = partImg.size();
    std::vector<TextLine> textLines(size);
#ifdef __OPENMP__
#pragma omp parallel for num_threads(numThread)
#endif
    for (int i = 0; i < size; ++i) {
        //getTextLine
        double startCrnnTime = getCurrentTime();
        TextLine textLine = getTextLine(partImg[i]);
        double endCrnnTime = getCurrentTime();
        textLine.time = endCrnnTime - startCrnnTime;

        //Log textLine
        //Logger("textLine[%d](%s)", i, textLine.text.c_str());
        textLines[i] = textLine;

        /*std::ostringstream txtScores;
        for (int s = 0; s < textLine.charScores.size(); ++s) {
            if (s == 0) {
                txtScores << textLine.charScores[s];
            } else {
                txtScores << " ," << textLine.charScores[s];
            }
        }*/
        //Logger("textScores[%d]{%s}", i, string(txtScores.str()).c_str());
        //Logger("crnnTime[%d](%fms)", i, textLine.time);
    }
    return textLines;
}