#include <omp.h>
#include <cstdio>
#include <string>
#include <OcrUtils.h>
#include "OcrLite.h"
#include "getopt.h"

static const struct option long_options[] = {
        {"models",    required_argument, NULL, 'd'},
        {"image",     required_argument, NULL, 'i'},
        {"numThread", required_argument, NULL, 't'},
        {"GPU",       required_argument, NULL, 'G'},
        {"loopCount", required_argument, NULL, 'l'},
        {"help",      no_argument,       NULL, '?'},
        {NULL,        no_argument,       NULL, 0}
};

const char *usageMsg = "(-d --models) (-i --image)\n"\
                       "[-t --numThread] [-l --loopCount]\n\n";

const char *requiredMsg = "-d --models: models directory.\n" \
                          "-i --image: path of target image.\n" \
                          "-G --GPU: set gpu index.CPU(-1) GPU(0) GPU(1) ...\n" \
                          "-l --loopCount: loop count of benchmark.\n\n";

const char *otherMsg = "-? --help: print this help\n\n";

const char *example1Msg = "Example1: %s --models models --image 1.jpg --loopCount 10\n";
const char *example2Msg = "Example2: %s -d models -i 1.jpg -l 10\n";

void printHelp(FILE *out, char *argv0) {
    fprintf(out, " ------- Usage -------\n");
    fprintf(out, "%s %s", argv0, usageMsg);
    fprintf(out, " ------- Required Parameters -------\n");
    fprintf(out, "%s", requiredMsg);
    fprintf(out, " ------- Other Parameters -------\n");
    fprintf(out, "%s", otherMsg);
    fprintf(out, " ------- Examples -------\n");
    fprintf(out, example1Msg, argv0);
    fprintf(out, example2Msg, argv0);
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        printHelp(stderr, argv[0]);
        return -1;
    }
    std::string modelsDir = "../models";
    std::string argImgPath = "../../test_imgs/1.jpg";
    std::string imgPath;
    std::string imgName;
    int numThread = 4;
    int loopCount = 1;
    int padding = 50;
    int imgResize = 0;
    float boxScoreThresh = 0.6f;
    float boxThresh = 0.3f;
    float minArea = 3.f;
    float unClipRatio = 2.0f;
    bool doAngle = true;
    int flagDoAngle = 1;
    bool mostAngle = true;
    int flagMostAngle = 1;
    int flagGpu = -1;

    int opt;
    int optionIndex = 0;
    while ((opt = getopt_long(argc, argv, "i:d:t:G:l:?", long_options, &optionIndex)) != -1) {
        //printf("option(-%c)=%s\n", opt, optarg);
        switch (opt) {
            case 'd':
                modelsDir = optarg;
                printf("modelsPath=%s\n", modelsDir.c_str());
                break;
            case 'i':
                argImgPath.assign(optarg);
                imgPath.assign(argImgPath.substr(0, argImgPath.find_last_of('/') + 1));
                imgName.assign(argImgPath.substr(argImgPath.find_last_of('/') + 1));
                printf("imgPath=%s, imgName=%s\n", imgPath.c_str(), imgName.c_str());
                break;
            case 't':
                numThread = (int) strtol(optarg, NULL, 10);
                //printf("numThread=%d\n", numThread);
                break;
            case 'G':
                flagGpu = (int) strtol(optarg, NULL, 10);
                printGpuInfo();
                break;
            case 'l':
                loopCount = (int) strtol(optarg, NULL, 10);
                //printf("loopCount=%d\n", loopCount);
                break;
            case '?':
                printHelp(stdout, argv[0]);
                return 0;
            default:
                printf("other option %c :%s\n", opt, optarg);
        }
    }

    omp_set_num_threads(numThread);
    OcrLite ocrLite;
    ocrLite.setNumThread(numThread);
    ocrLite.initLogger(
            false,//isOutputConsole
            false,//isOutputPartImg
            false);//isOutputResultImg

    ocrLite.enableResultTxt(imgPath.c_str(), imgName.c_str());
    ocrLite.setGpuIndex(flagGpu);
    printf("=====Input Params=====\n");
    printf(
            "numThread(%d),padding(%d),imgResize(%d),boxScoreThresh(%f),boxThresh(%f),minArea(%f),unClipRatio(%f),doAngle(%d),mostAngle(%d),GPU(%d)\n",
            numThread, padding, imgResize, boxScoreThresh, boxThresh, minArea, unClipRatio, doAngle, mostAngle,
            flagGpu);

    ocrLite.initModels(modelsDir.c_str());
    printf("=====warmup=====\n");
    OcrResult result = ocrLite.detect(imgPath.c_str(), imgName.c_str(),
                                      padding, imgResize,
                                      boxScoreThresh, boxThresh, minArea,
                                      unClipRatio, doAngle, mostAngle);
    printf("dbNetTime(%f) detectTime(%f)\n", result.dbNetTime, result.detectTime);
    double dbTime = 0.0f;
    double detectTime = 0.0f;
    for (int i = 0; i < loopCount; ++i) {
        printf("=====loop:%d=====\n", i + 1);
        OcrResult ocrResult = ocrLite.detect(imgPath.c_str(), imgName.c_str(),
                                             padding, imgResize,
                                             boxScoreThresh, boxThresh, minArea,
                                             unClipRatio, doAngle, mostAngle);
        printf("dbNetTime(%f) detectTime(%f)\n", ocrResult.dbNetTime, ocrResult.detectTime);
        dbTime += ocrResult.dbNetTime;
        detectTime += ocrResult.detectTime;
    }
    printf("=====result=====\n");
    printf("average dbNetTime=%fms, average detectTime=%fms\n", dbTime / loopCount,
           detectTime / loopCount);
    return 0;
}
