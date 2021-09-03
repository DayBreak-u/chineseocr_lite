#ifdef __CLIB__

#include "OcrLiteCaller.h"

#include <io.h>

#include "OcrLite.h"

OcrLiteCaller::OcrLiteCaller()
	: m_ocrlite_ptr(std::make_shared<OcrLite>())
{
	;
}

void OcrLiteCaller::setNumThread(int numOfThread)
{
	m_ocrlite_ptr->setNumThread(numOfThread);
}

bool OcrLiteCaller::initModels(const std::string& detPath, const std::string& clsPath, const std::string& recPath, const std::string& keysPath)
{
	// onnx的是直接通过构造函数传进去的，好像没法检查init是否正常，这里仅简单检查一下文件是否可读
	if (access(detPath.c_str(), 4) == -1
		|| access(clsPath.c_str(), 4) == -1
		|| access(recPath.c_str(), 4) == -1
		|| access(keysPath.c_str(), 4) == -1)
	{
		return false;
	}
	m_ocrlite_ptr->initModels(detPath, clsPath, recPath, keysPath);
	return true;
}

OcrResult OcrLiteCaller::detect(const cv::Mat& mat, int padding, int maxSideLen, float boxScoreThresh, float boxThresh, float unClipRatio, bool doAngle, bool mostAngle)
{
	return m_ocrlite_ptr->detect(mat, padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
}

OcrResult OcrLiteCaller::detect(const std::string& dir, const std::string& file, int padding, int maxSideLen, float boxScoreThresh, float boxThresh, float unClipRatio, bool doAngle, bool mostAngle)
{
	return m_ocrlite_ptr->detect(dir.c_str(), file.c_str(), padding, maxSideLen, boxScoreThresh, boxThresh, unClipRatio, doAngle, mostAngle);
}

#endif