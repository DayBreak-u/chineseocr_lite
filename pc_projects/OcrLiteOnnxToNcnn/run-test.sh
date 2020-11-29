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
pushd build
./OcrLiteOnnxToNcnn --models ../models --image ../../test_imgs/long1.jpg \
--numThread $NUM_THREADS --padding 50 --imgResize 0 \
--boxScoreThresh 0.6 --boxThresh 0.3 --minArea 3 \
--unClipRatio 2.0 --doAngle 1 --mostAngle 1
popd