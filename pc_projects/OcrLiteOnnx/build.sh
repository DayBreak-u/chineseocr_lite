#!/usr/bin/env bash

function buildOcrLite() {
mkdir -p build
pushd build
cmake -DCMAKE_BUILD_TYPE=Release -DOCR_LITE_OPENMP=ON ..
make -j4
popd
}

function buildOcrLiteLib() {
mkdir -p build-lib
pushd build-lib
cmake -DCMAKE_BUILD_TYPE=Release -DOCR_LITE_LIB=ON -DOCR_LITE_OPENMP=ON ..
make -j4
popd
}

echo "请选择编译选项并回车: 1)编译可执行文件，2)编译动态库"
echo "Please Select Build Type: 1)Executable，2)Dynamic Library"
read -t 30 -p "" BUILD_TYPE
if [ $BUILD_TYPE == 1 ]; then
    buildOcrLite
elif [ $BUILD_TYPE == 2 ]; then
    buildOcrLiteLib
else
  echo -e "输入错误！Input Error!"
fi

