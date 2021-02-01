#include "CrnnNet.h"
#include "OcrUtils.h"
#include <fstream>
#include <numeric>

void CrnnNet::setGpuIndex(int gpuIndex) {
#ifdef __VULKAN__
    if (gpuIndex >= 0) {
        net.opt.use_vulkan_compute = true;
        net.set_vulkan_device(gpuIndex);
        printf("CrnnNet try to use Gpu%d\n", gpuIndex);
    } else {
        net.opt.use_vulkan_compute = false;
        printf("CrnnNet use Cpu\n");
    }
#endif
}

CrnnNet::~CrnnNet() {
    net.clear();
}

void CrnnNet::setNumThread(int numOfThread) {
    numThread = numOfThread;
}

bool CrnnNet::initModel(const std::string &pathStr, const std::string &keysPath) {
    int ret_param = net.load_param((pathStr + ".param").c_str());
    int ret_bin = net.load_model((pathStr + ".bin").c_str());
    if (ret_param != 0 || ret_bin != 0) {
        printf("CrnnNet load param(%d), model(%d)\n", ret_param, ret_bin);
        return false;
    }
    //load keys
    std::ifstream in(keysPath.c_str());
    std::string line;
    if (in) {
        while (getline(in, line)) {// line中不包括每行的换行符
            keys.push_back(line);
        }
    } else {
        printf("The keys.txt file was not found\n");
        return false;
    }
    if (keys.size() != 5531) {
        fprintf(stderr, "missing keys\n");
        return false;
    }
    printf("total keys size(%lu)\n", keys.size());
    return true;
}

template<class ForwardIterator>
inline static size_t argmax(ForwardIterator first, ForwardIterator last) {
    return std::distance(first, std::max_element(first, last));
}

TextLine CrnnNet::scoreToTextLine(const std::vector<float> &outputData, int h, int w) {
    int keySize = keys.size();
    std::string strRes;
    std::vector<float> scores;
    int lastIndex = 0;
    int maxIndex;
    float maxValue;

    for (int i = 0; i < h; i++) {
        maxIndex = 0;
        maxValue = -1000.f;
        //do softmax
        std::vector<float> exps(w);
        for (int j = 0; j < w; j++) {
            float expSingle = exp(outputData[i * w + j]);
            exps.at(j) = expSingle;
        }
        float partition = accumulate(exps.begin(), exps.end(), 0.0);//row sum
        maxIndex = int(argmax(exps.begin(), exps.end()));
        maxValue = float(*std::max_element(exps.begin(), exps.end())) / partition;
        if (maxIndex > 0 && maxIndex < keySize && (!(i > 0 && maxIndex == lastIndex))) {
            scores.emplace_back(maxValue);
            strRes.append(keys[maxIndex - 1]);
        }
        lastIndex = maxIndex;
    }
    return {strRes, scores};
}

TextLine CrnnNet::getTextLine(const cv::Mat &src) {
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

    ncnn::Mat out;
    extractor.extract("out", out);

    float *floatArray = (float *) out.data;
    std::vector<float> outputData(floatArray, floatArray + out.h * out.w);
    return scoreToTextLine(outputData, out.h, out.w);
}

std::vector<TextLine> CrnnNet::getTextLines(std::vector<cv::Mat> &partImg, const char *path, const char *imgName) {
    int size = partImg.size();
    std::vector<TextLine> textLines(size);
#ifdef __OPENMP__
#pragma omp parallel for num_threads(numThread)
#endif
    for (int i = 0; i < size; ++i) {
        //OutPut DebugImg
        if (isOutputDebugImg) {
            std::string debugImgFile = getDebugImgFilePath(path, imgName, i, "-debug-");
            saveImg(partImg[i], debugImgFile.c_str());
        }

        //getTextLine
        double startCrnnTime = getCurrentTime();
        TextLine textLine = getTextLine(partImg[i]);
        double endCrnnTime = getCurrentTime();
        textLine.time = endCrnnTime - startCrnnTime;
        textLines[i] = textLine;
    }
    return textLines;
}