#pragma once

#ifdef WIN32

#ifdef __COL_DLL_EXPORT_LIBRARY
#define  _COL_EXPORT __declspec(dllexport)
#else
#define  _COL_EXPORT __declspec(dllimport)
#endif // __COL_DLL_EXPORT_LIBRARY

	
#else
	#define  _COL_EXPORT 
#endif

#ifdef __cplusplus

extern "C" {
#endif

	typedef  void* OCRENGINE_PTR;

	OCRENGINE_PTR _COL_EXPORT COL_InitOCREngine(const char * szOCrModelDir,int nUseGPU);
	int _COL_EXPORT COL_Recognition(const OCRENGINE_PTR pEngine, const char* szImageFilePath, char** szResult, const int nLongSize);
	void _COL_EXPORT COL_FreeResult(void * pResult);
	void  _COL_EXPORT COL_FreeOCREngine(OCRENGINE_PTR pEngine);
	void  _COL_EXPORT COL_SetVerbose(OCRENGINE_PTR pEngine,int nVerbose);



#ifdef __cplusplus

}
#endif