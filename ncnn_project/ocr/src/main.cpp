#include "precomp.h"
int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s  /path/to/image/file  longsize  verbose_mode models_dir \n", argv[0]);
        std::cout << "eg: " << argv[0] << " e:\\temp\\test.jpg  560" << std::endl;
        std::cout << "eg: " << argv[0] << " e:\\temp\\test.jpg  560  1  e:\\ocr_models" << std::endl;
        std::cout << "Caution: please copy your models directory into  the same diirectory as your program: " <<argv[0] << std::endl;

        return -1;
    }


    string strModelDir="models/";
    const int long_size = atoi(argv[2]);
    bool bVerbose = false;
    if (argc >= 4)
    {
        bVerbose = atoi(argv[3]) > 0;
    }
    if (argc == 5)
        strModelDir = argv[4];
    OCRENGINE_PTR pEngine =COL_InitOCREngine(strModelDir.c_str());
    if (!pEngine)
    {
        cout << "Cannot initialize the OCR Engine, please check your model path!" << endl;
        return -1;
    }


    if(bVerbose)
        COL_SetVerbose(pEngine, 1);

    char* pResult;
    int nSize = COL_Recognition(pEngine, argv[1], &pResult, long_size);
    if (nSize>= 0)
    {
        if(!bVerbose)
            printf("%s",pResult);
	    cout <<endl<<"total size: "<< nSize << " Bytes"<< endl;
    }
    else
    { 
        cout << "failed to run ocr tasks" << endl;
    }



    if (pResult)
        COL_FreeResult(pResult);

    if(pEngine)
        COL_FreeOCREngine(pEngine);
    return 0;
}
