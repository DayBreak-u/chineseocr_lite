#!/usr/bin/env bash

sysOS=`uname -s`
NUM_THREADS=1
if [ $sysOS == "Darwin" ];then
    #echo "I'm MacOS"
    NUM_THREADS=$(sysctl -n hw.ncpu)
elif [ $sysOS == "Linux" ];then
    #echo "I'm Linux"
    NUM_THREADS=$(grep ^processor /proc/cpuinfo | wc -l)
else
    echo "Other OS: $sysOS"
fi

echo "Setting the Number of Threads=$NUM_THREADS Using an OpenMP Environment Variable"
set OMP_NUM_THREADS=$NUM_THREADS

##### run test on MacOS or Linux
java -Djava.library.path=. -jar OcrLiteOnnxJvm.jar models test_imgs/1.jpg \
$NUM_THREADS \
50 \
0 \
0.6 \
0.3 \
3 \
2.0 \
1 \
1

#numThread
#padding
#imgResize
#boxScoreThresh
#boxThresh
#minArea
#unClipRatio
#doAngle
#mostAngle