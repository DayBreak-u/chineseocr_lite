#ifndef __MAIN_H__
#define __MAIN_H__

#include "getopt.h"

static const struct option long_options[] = {
        {"models",         required_argument, NULL, 'd'},
        {"image",          required_argument, NULL, 'i'},
        {"numThread",      required_argument, NULL, 't'},
        {"padding",        required_argument, NULL, 'p'},
        {"imgResize",      required_argument, NULL, 's'},
        {"boxScoreThresh", required_argument, NULL, 'b'},
        {"boxThresh",      required_argument, NULL, 'o'},
        {"minArea",        required_argument, NULL, 'm'},
        {"unClipRatio",    required_argument, NULL, 'u'},
        {"noAngle",        no_argument,       NULL, 'n'},
        {"help",           no_argument,       NULL, '?'},
        {NULL,             no_argument,       NULL, 0}
};

const char *usageMsg = "(-d --models) (-i --image)\n"\
                       "[-t --numThread] [-p --padding] [-s --imgResize]\n" \
                       "[-b --boxScoreThresh] [-o --boxThresh] [-m --minArea]\n" \
                       "[-u --unClipRatio]  [-n --noAngle]\n\n";

const char *requiredMsg = "-d --models: models directory.\n" \
                          "-i --image: path of target image.\n\n";

const char *optionalMsg = "-t --numThread: value of numThread(int), default: 4\n" \
                          "-p --padding: value of padding(int), default: 50\n" \
                          "-s --imgResize: value of imgResize(int), default: 0\n" \
                          "-b --boxScoreThresh: value of boxScoreThresh(float), default: 0.6\n" \
                          "-o --boxThresh: value of boxThresh(float), default: 0.3\n" \
                          "-m --minArea: value of minArea(float), default: 3.0\n" \
                          "-u --unClipRatio: value of unClipRatio(float), default: 2.0\n" \
                          "-n --noAngle: Disable Angle Net, default: Enable\n\n";

const char *otherMsg = "-? --help: print this help\n\n";


const char *example1Msg = "Example1: %s --models models --image 1.jpg\n";
const char *example2Msg = "Example1: %s -d models -i 1.jpg -t 4 -p 50 -s 0 -b 0.6 -o 0.3 -m 3 -u 2.0\n";

#endif //__MAIN_H__
