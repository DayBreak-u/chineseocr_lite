#!/usr/bin/env bash

function PrepareVar(){
echo "Gpu版本测试前请先安装Vulkan SDK v1.2.162.0，https://vulkan.lunarg.com/sdk/home"
echo "请输入测试选项并回车: 1)CPU, 2)GPU"
read -p "" RUN_ARCH
if [ $RUN_ARCH == 1 ]; then
    EXE_PATH=${sysOS}-BIN-CPU
    GPU_INDEX=-1
elif [ $RUN_ARCH == 2 ]; then
    EXE_PATH=${sysOS}-BIN-GPU
    GPU_INDEX=0
else
  echo -e "输入错误！Input Error!"
fi
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

TARGET_IMG=images/1.jpg
if [ ! -f "$TARGET_IMG" ]; then
echo "找不到待识别的目标图片：${TARGET_IMG}，请打开本文件并编辑TARGET_IMG"
exit
fi

##### run test on MacOS or Linux
./${EXE_PATH}/OcrLiteNcnn --version
./${EXE_PATH}/OcrLiteNcnn --models models \
--det dbnet_op \
--cls angle_op \
--rec crnn_lite_op \
--keys keys.txt \
--image $TARGET_IMG \
--numThread $NUM_THREADS \
--padding 50 \
--maxSideLen 0 \
--boxScoreThresh 0.6 \
--boxThresh 0.3 \
--unClipRatio 2.0 \
--doAngle 1 \
--mostAngle 1 \
--GPU $GPU_INDEX
