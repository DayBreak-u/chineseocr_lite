#include "Logger.h"
#include <time.h>
#include <cstdio>

long getCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

long printTime(char *tag, long start, long end) {
    long time = end - start;
    LOGI("%s = %ldms", tag, time);
    return time;
}