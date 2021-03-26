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


TARGET_IMG=../images/1.jpg
if [ ! -f "$TARGET_IMG" ]; then
echo "找不到待识别的目标图片：${TARGET_IMG}，请打开本文件并编辑TARGET_IMG"
exit
fi

##### run test on MacOS or Linux
./build/OcrLiteMnn --version
./build/OcrLiteMnn --models models \
--det dbnet.mnn \
--cls angle_net.mnn \
--rec crnn_lite_lstm.mnn \
--keys keys.txt \
--image $TARGET_IMG \
--numThread $NUM_THREADS \
--padding 50 \
--maxSideLen 1024 \
--boxScoreThresh 0.6 \
--boxThresh 0.3 \
--unClipRatio 2.0 \
--doAngle 1 \
--mostAngle 1