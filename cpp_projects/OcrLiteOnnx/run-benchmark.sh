#!/usr/bin/env bash

function PrepareVar(){
EXE_PATH=${sysOS}-BIN
}

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

PrepareVar

echo "请输入循环次数"
read -p "" LOOP_COUNT

TARGET_IMG=images/1.jpg
if [ ! -f "$TARGET_IMG" ]; then
echo "找不到待识别的目标图片：${TARGET_IMG}，请打开本文件并编辑TARGET_IMG"
exit
fi

./${EXE_PATH}/benchmark --version
./${EXE_PATH}/benchmark --models models \
--det dbnet.onnx \
--cls angle_net.onnx \
--rec crnn_lite_lstm.onnx \
--keys keys.txt \
--image $TARGET_IMG \
--numThread $NUM_THREADS \
--padding 50 \
--maxSideLen 1024 \
--boxScoreThresh 0.6 \
--boxThresh 0.3 \
--unClipRatio 2.0 \
--doAngle 1 \
--mostAngle 1 \
--loopCount $LOOP_COUNT