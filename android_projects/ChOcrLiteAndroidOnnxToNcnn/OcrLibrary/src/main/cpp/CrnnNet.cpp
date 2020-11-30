#include "include/CrnnNet.h"
#include "OcrUtils.h"
#include <numeric>

CrnnNet::~CrnnNet() {
    net.clear();
}

void CrnnNet::setNumOfThreads(int numOfThread) {
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

bool CrnnNet::initModel(AAssetManager *mgr) {
    int ret_param = net.load_param(mgr, "crnn_lite_op.param");
    int ret_bin = net.load_model(mgr, "crnn_lite_op.bin");
    if (ret_param != 0 || ret_bin != 0) {
        LOGE("# %d  %d", ret_param, ret_bin);
        return false;
    }

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
    return {strRes, scores};
}

TextLine CrnnNet::getTextLine(cv::Mat &src) {
    float scale = (float) dstHeight / (float) src.rows;
    int dstWidth = int((float) src.cols * scale);

    cv::Mat srcResize;
    resize(src, srcResize, cv::Size(dstWidth, dstHeight));

    ncnn::Mat input = ncnn::Mat::from_pixels(
            srcResize.data, ncnn::Mat::PIXEL_RGB,
            srcResize.cols, srcResize.rows);

    input.substract_mean_normalize(meanValues, normValues);

    ncnn::Extractor extractor = net.create_extractor();
    extractor.set_num_threads(numThread);
    extractor.input("input", input);

    // lstm
    ncnn::Mat blob162;
    extractor.extract("1000", blob162);

    // batch fc
    ncnn::Mat blob263(5531, blob162.h);
    for (int i = 0; i < blob162.h; i++) {
        ncnn::Extractor extractor2 = net.create_extractor();
        extractor2.set_num_threads(numThread);
        ncnn::Mat blob243_i = blob162.row_range(i, 1);
        extractor2.input("1014", blob243_i);

        ncnn::Mat blob263_i;
        extractor2.extract("1015", blob263_i);

        memcpy(blob263.row(i), blob263_i, 5531 * sizeof(float));
    }

    return scoreToTextLine((float *) blob263.data, blob263.h, blob263.w);
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