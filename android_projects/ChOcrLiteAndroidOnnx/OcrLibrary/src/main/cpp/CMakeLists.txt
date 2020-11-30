cmake_minimum_required(VERSION 3.4.1)

option(OCR_LITE_OPENMP "OcrLite Enable OpenMP" ON)

#ONNX
set(ONNX_DIR "${CMAKE_SOURCE_DIR}/../onnx")
find_package(ONNX REQUIRED)
if (ONNX_FOUND)
    message(STATUS "ONNX_LIBS: ${ONNX_LIBS}")
    message(STATUS "ONNX_INCLUDE_DIRS: ${ONNX_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "onnxruntime Not Found!")
endif (ONNX_FOUND)

## opencv
add_library(libopencv_java3 SHARED IMPORTED)
set_target_properties(libopencv_java3 PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/../sharedLibs/${ANDROID_ABI}/libopencv_java3.so)

# openmp
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fopenmp")

if(DEFINED ANDROID_NDK_MAJOR AND ${ANDROID_NDK_MAJOR} GREATER 20)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-openmp")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fomit-frame-pointer -fstrict-aliasing -ffast-math")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer -fstrict-aliasing -ffast-math")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")

# disable rtti and exceptions
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti -fno-exceptions")

include_directories(include)
file(GLOB OCR_SRC *.cpp)
set(OCR_COMPILE_CODE ${OCR_SRC})

add_library(OcrLite SHARED ${OCR_COMPILE_CODE})

if (OCR_LITE_OPENMP)
    target_compile_definitions(OcrLite PRIVATE __OPENMP__)
endif ()

find_library( # Sets the name of the path variable.
        log-lib
        log)

find_library(
        android-lib
        android
)

target_link_libraries(
        OcrLite
        ${ONNX_LIBS}
        libopencv_java3
        android
        z
        ${log-lib}
        ${android-lib}
        jnigraphics)
