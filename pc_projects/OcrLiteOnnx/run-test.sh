#!/usr/bin/env bash

##### run test on MacOS or Linux
pushd build
./OcrLiteOnnx ../../test_imgs/1.jpg ../models 0 0.6 0.3 3 1.3 1.3 1.6 1.6
popd