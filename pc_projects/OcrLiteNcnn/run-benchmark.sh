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

echo "请输入循环次数:"
read -p "" LOOP_COUNT

##### run test on MacOS or Linux
pushd build
./benchmark --models ../models --image ../../test_imgs/1.jpg \
--numThread $NUM_THREADS --loopCount $LOOP_COUNT -G 0
popd