# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
include_directories(include)
file(GLOB mytext_SRC *.h
        *.cpp)
set(mytext_COMPILE_CODE ${mytext_SRC})

add_library( # Sets the name of the library.
        mytext ## 为生成.so的文字最好直接和.c名字一样,需要更改

        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        ${mytext_COMPILE_CODE})##cpp文件的name


## opencv 库
add_library(libopencv_java3 SHARED IMPORTED)
set_target_properties(libopencv_java3 PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libopencv_java3.so)

## ncnn 库
add_library (ncnn_lib STATIC IMPORTED)
set_target_properties(ncnn_lib PROPERTIES IMPORTED_LOCATION  ${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI}/libncnn.a)

#add_library( # Sets the name of the library.
#        ##native-lib
#
#        # Sets the library as a shared library.
#        SHARED
#
#        # Provides a relative path to your source file(s).
#        native-lib.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.
find_library(

        android-lib

        android

)
target_link_libraries( # Specifies the target library.
        ##native-lib
        mytext
        ncnn_lib
        libopencv_java3
        android
        jnigraphics
        # Links the target library to the log library
        # included in the NDK.
        ${android-lib}
        ${log-lib})