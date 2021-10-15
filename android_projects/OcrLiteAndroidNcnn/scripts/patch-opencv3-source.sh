#!/usr/bin/env bash
# base on https://github.com/nihui/opencv-mobile/blob/master/.github/workflows/release.yml

# patch opencv
truncate -s 0 cmake/OpenCVFindLibsGrfmt.cmake
patch -p1 -i ../opencv-3.4.15-no-rtti.patch
patch -p1 -i ../opencv-3.4.15-no-zlib.patch
rm -rf modules/highgui
cp -r ../highgui modules/
