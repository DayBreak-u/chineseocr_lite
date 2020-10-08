#include "chocrlite.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <sys/timeb.h>

#if defined(WIN32)
# define  TIMEB    _timeb
# define  ftime    _ftime
typedef __int64  TIME_T;
#else
#define TIMEB timeb
typedef long long TIME_T;
#endif

using namespace std;
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s  /path/to/image/file  shortsize  verbose_mode  use_gpu models_dir \n", argv[0]);
        std::cout << "eg (detect the short size automatically): " << argv[0] << " e:\\temp\\test.jpg " << std::endl;
        std::cout << "eg: " << argv[0] << " e:\\temp\\test.jpg  560  1  1 e:\\ocr_models" << std::endl;
        std::cout << "Caution: please copy your models directory into  the same diirectory as your program: " <<argv[0] << std::endl;
        std::cout << "By default: We don't use GPU. If you turn on GPU mode, it's possible there's no result." << std::endl;
        return -1;
    }


    string strModelDir="models/";
    int short_size = 0;
    if( argc >2)
        short_size= atoi(argv[2]);
    bool bVerbose = false;
    bool bUseGPU = false;
    if (argc >= 4)
    {
        bVerbose = atoi(argv[3]) > 0;
    }
    if (argc >= 5)
    {
        bUseGPU= atoi(argv[4]) > 0;
    }
    if (argc == 6)
        strModelDir = argv[5];
    OCRENGINE_PTR pEngine =COL_InitOCREngine(strModelDir.c_str(), bUseGPU);
    if (!pEngine)
    {
        cout << "Cannot initialize the OCR Engine, please check your model path!" << endl;
        return -1;
    }


    if(bVerbose)
        COL_SetVerbose(pEngine, 1);

    char* pResult;


    struct TIMEB ts1, ts2;
    TIME_T t1, t2;
   


    
    //ftime(&ts1);
    double nTotalTime = 0;
    int nSize = COL_Recognition(pEngine, argv[1], &pResult, short_size,&nTotalTime);
 /*   ftime(&ts2);

    t1 = (TIME_T)ts1.time * 1000 + ts1.millitm;
    t2 = (TIME_T)ts2.time * 1000 + ts2.millitm;*/
   

    if (nSize >= 0 && !bVerbose)
    {
              
            printf("%s", pResult);
            cout << endl << "total size: " << nSize << " Bytes" << endl;
            
            //t1 = (t2 - t1) / 1000;
            cout << "total time: " << nTotalTime << " seconds" << endl;
    }
        
    if(nSize <0)
    {
        cout << "failed to run ocr tasks" << endl;
    }
    



    if (pResult)
        COL_FreeResult(pResult);

    if(pEngine)
        COL_FreeOCREngine(pEngine);
    return 0;
}
