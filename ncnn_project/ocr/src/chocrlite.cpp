#include "precomp.h"

//COL = ChineseOCR_lite
/*
*  szImageFilePath: the path of a image file to be recognized
*  szResult: the result text lines for the image.
*  nLines:  How many lines the function returns.
* 
*/

int _COL_EXPORT COL_Recognition(const OCRENGINE_PTR pEngine, const char* szImageFilePath, char** szResult,const int short_size,double * dTotalTime)
{

    cv::Mat im_bgr = cv::imread(szImageFilePath, 1);
    if (im_bgr.empty()) {
        fprintf(stderr, "cv::imread %s failed\n", szImageFilePath);
        return -1;
    }

    OCR* ocrengine = static_cast<OCR *>(pEngine);
    
    double dTime = 0;
    ocrengine->detect(im_bgr, short_size,dTime);
    *dTotalTime = dTime;
    
    vector<string>& result = ocrengine->GetResult();
    int nSize = 0;
    for (size_t i = 0; i < result.size(); i++)
    {
        
        nSize += result[i].length();
        
    }

    if (nSize <= 0)
        return 0;

    char* pStrings = new char[nSize+1];
    *szResult = pStrings;
    
    for (size_t i = 0; i < result.size(); i++)
    {
        strncpy(pStrings, result[i].c_str(),result[i].size());
        pStrings += result[i].size();
        
   
    }
    *pStrings = '\0';
   
    return nSize;
}

void _COL_EXPORT COL_FreeResult(void* pResult)
{
    if (pResult)
        delete[](char*)pResult;
}
OCRENGINE_PTR _COL_EXPORT  COL_InitOCREngine(const char* szOCrModelDir,int nUseGPU)
{

    OCR* ocrengine = new OCR(szOCrModelDir,nUseGPU>0?true:false);
    return ocrengine;
}

void  _COL_EXPORT COL_FreeOCREngine(OCRENGINE_PTR pEngine)
{
    if (pEngine)
    {
        delete (OCR*)pEngine;
    }

}


void  _COL_EXPORT COL_SetVerbose(OCRENGINE_PTR pEngine,int nVerbose)
{
    if (pEngine)
        ((OCR*)pEngine)->set_verbose(nVerbose > 0);
}