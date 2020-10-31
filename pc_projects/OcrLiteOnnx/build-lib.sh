#!/usr/bin/env bash

##### build jni for MacOS or Linux
mkdir -p build-lib
pushd build-lib
cmake -DCMAKE_BUILD_TYPE=Release -DOCR_LITE_LIB=ON ..
make
popd
