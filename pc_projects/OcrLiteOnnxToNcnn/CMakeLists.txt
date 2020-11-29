cmake_minimum_required(VERSION 2.8)
project(OcrLiteOnnxToNcnn)

option(OCR_LITE_LIB "OcrLite Jni Support" OFF)
option(OCR_LITE_OPENMP "OcrLite Enable OpenMP" ON)

#set(OCR_LITE_LIB ON)
if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif ()

add_definitions(-std=c++11)
add_definitions("-Wall")
add_definitions(-DUNICODE -D_UNICODE)

#OpenMP flags for MACOS
if (APPLE)
    if (CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(OpenMP_C "${CMAKE_C_COMPILER}")
        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include")
        set(OpenMP_C_LIB_NAMES "omp")
        set(OpenMP_omp_LIBRARY ${OpenMP_C_LIB_NAMES})
    endif ()
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include")
        set(OpenMP_CXX_LIB_NAMES "omp")
        set(OpenMP_omp_LIBRARY ${OpenMP_C_LIB_NAMES})
    endif ()
    link_directories("/usr/local/opt/libomp/lib")
endif ()

#OpenMP
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else ()
    message(FATAL_ERROR "OpenMP Not Found!")
endif ()

#NCNN
set(NCNN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ncnn")
find_package(NCNN REQUIRED)
if (NCNN_FOUND)
    message(STATUS "NCNN_LIBS: ${NCNN_LIBS}")
    message(STATUS "NCNN_INCLUDE_DIRS: ${NCNN_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "ncnn Not Found!")
endif (NCNN_FOUND)

#OpenCV
if (WIN32)
    message("配置WINDOWS OpenCV 路径，请把opencv库放到${CMAKE_CURRENT_SOURCE_DIR}/opencv")
    set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/opencv/build")
endif ()
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
    message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "opencv Not Found!")
endif (OpenCV_FOUND)

#JNI
if (OCR_LITE_LIB)
    find_package(JNI REQUIRED)
    if (JNI_FOUND)
        message("JNI FOUND")
        message(STATUS "JNI_LIBS: ${JNI_LIBS}")
        message(STATUS "JNI_INCLUDE_DIRS: ${JNI_INCLUDE_DIRS}")
        include_directories(${JNI_INCLUDE_DIRS})
    else ()
        message(FATAL_ERROR "JNI Not Found!")
    endif ()
endif ()

#project include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#source
file(GLOB OCR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(OCR_COMPILE_CODE ${OCR_SRC})

if (OCR_LITE_LIB)
    add_library(OcrLiteOnnxToNcnn SHARED ${OCR_COMPILE_CODE})
    target_compile_definitions(OcrLiteOnnxToNcnn PRIVATE __JNI__)
    target_link_libraries(OcrLiteOnnxToNcnn ${NCNN_LIBS} ${OpenCV_LIBS} ${JNI_LIBS} ${OpenMP_CXX_LIB_NAMES})
else ()
    add_executable(OcrLiteOnnxToNcnn ${OCR_COMPILE_CODE})
    target_link_libraries(OcrLiteOnnxToNcnn ${NCNN_LIBS} ${OpenCV_LIBS} ${OpenMP_CXX_LIB_NAMES})
endif ()

if (OCR_LITE_OPENMP)
    target_compile_definitions(OcrLiteOnnxToNcnn PRIVATE __OPENMP__)
endif ()