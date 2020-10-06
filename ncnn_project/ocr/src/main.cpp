#include "ocr.h"
#include "cnocrlite.h"
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s  /path/to/image/file  longsize  models_dir \n", argv[0]);
        std::cout << "eg: " << argv[0] << " e:\\temp\\test.jpg  560" << std::endl;
        return -1;
    }


    string strModelDir="models/";
    const int long_size = atoi(argv[2]);
    if (argc == 4)
        strModelDir = argv[3];
    OCRENGINE_PTR pEngine =COL_InitOCREngine(strModelDir.c_str());
    if (!pEngine)
    {
        cout << "Cannot initialize the OCR Engine, please check your model path!" << endl;
        return -1;
    }


    // COL_SetVerbose(pEngine, 1);

    char* pResult;
    int nSize = COL_Recognition(pEngine, argv[1], &pResult, long_size);
    if (nSize> 0)
    {
        printf(pResult);
    }

    cout <<endl<<"total size: "<< nSize << endl;

    if (pResult)
        COL_FreeResult(pResult);

    if(pEngine)
        COL_FreeOCREngine(pEngine);
    return 0;
}
