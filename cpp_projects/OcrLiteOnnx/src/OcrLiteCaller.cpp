#ifdef __CLIB__

#include "OcrLiteCaller.h"

#include "OcrLite.h"

OcrLiteCaller::OcrLiteCaller()
        : m_ocrlite_ptr(std::make_shared<OcrLite>()) {
    ;
}

void OcrLiteCaller::setNumThread(int numOfThread) {
    m_ocrlite_ptr->setNumThread(numOfThread);
}


bool OcrLiteCaller::initModels(const std::string &detPath, const std::string &clsPath, const std::string &recPath,
                               const std::string &keysPath) {
    return m_ocrlite_ptr->initModels(detPath, clsPath, recPath, keysPath);
}

OcrResult OcrLiteCaller::detect(const cv::Mat &mat, int padding, int maxSideLen, float boxScoreThresh, float boxThresh,
                                float unClipRatio, bool doAngle, bool mostAngle) {
    return m_ocrlite_ptr->detect(mat, padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
}

OcrResult OcrLiteCaller::detect(const std::string &dir, const std::string &file, int padding, int maxSideLen,
                                float boxScoreThresh, float boxThresh, float unClipRatio, bool doAngle,
                                bool mostAngle) {
    return m_ocrlite_ptr->detect(dir.c_str(), file.c_str(), padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio,
                                 doAngle, mostAngle);
}

#endif