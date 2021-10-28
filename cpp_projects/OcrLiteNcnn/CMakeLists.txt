if (WIN32)
    cmake_minimum_required(VERSION 3.12)
elseif (APPLE)
    cmake_minimum_required(VERSION 3.17)
elseif (UNIX)
    cmake_minimum_required(VERSION 3.17)
endif ()
project(OcrLiteNcnn)

if (NOT DEFINED OCR_OUTPUT)
    set(OCR_OUTPUT "BIN")
    message(STATUS "No OCR_OUTPUT, defaulting to BIN")
endif ()
if (NOT DEFINED OCR_VULKAN)
    set(OCR_VULKAN "CPU")
    message(STATUS "No OCR_VULKAN, defaulting to CPU")
endif ()
option(OCR_BENCHMARK "build benchmark" ON)
set(OCR_BENCHMARK ON)
#set(OCR_OUTPUT "BIN")

set(CMAKE_CXX_STANDARD 11)
add_definitions(-DUNICODE -D_UNICODE)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions("-Wall -g -O0")
else ()
    add_definitions("-Wall")
endif ()

# OpenMP flags for MACOS
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

# OpenMP
find_package(OpenMP REQUIRED)
if (OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
else ()
    message(FATAL_ERROR "OpenMP Not Found!")
endif ()

# NCNN
if (OCR_VULKAN STREQUAL "GPU")
    include(${CMAKE_CURRENT_SOURCE_DIR}/ncnn-vulkan-static/NcnnWrapperConfig.cmake)
else ()
    include(${CMAKE_CURRENT_SOURCE_DIR}/ncnn-static/NcnnWrapperConfig.cmake)
endif ()
find_package(ncnn REQUIRED)
if (ncnn_FOUND)
    message(STATUS "ncnn Found!")
else ()
    message(FATAL_ERROR "ncnn Not Found!")
endif (ncnn_FOUND)

# OpenCV
set(BUILD_SHARED_LIBS false)
include(${CMAKE_CURRENT_SOURCE_DIR}/opencv-static/OpenCVWrapperConfig.cmake)
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
    message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "opencv Not Found!")
endif (OpenCV_FOUND)

# project include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# source
file(GLOB OCR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(OCR_COMPILE_CODE ${OCR_SRC})

# JNI
if (OCR_OUTPUT STREQUAL "JNI")
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

if (OCR_OUTPUT STREQUAL "JNI") # JNI
    add_library(OcrLiteNcnn SHARED ${OCR_COMPILE_CODE})
    target_compile_definitions(OcrLiteNcnn PRIVATE __JNI__)
    target_link_libraries(OcrLiteNcnn ncnn ${OpenCV_LIBS} ${JNI_LIBS} ${OpenMP_CXX_LIB_NAMES})
elseif(OCR_OUTPUT STREQUAL "CLIB") # CLIB
    add_library(OcrLiteNcnn SHARED ${OCR_COMPILE_CODE})
    target_compile_definitions(OcrLiteNcnn PRIVATE __CLIB__)
    target_link_libraries(OcrLiteNcnn ncnn ${OpenCV_LIBS} ${OpenMP_CXX_LIB_NAMES})
elseif(OCR_OUTPUT STREQUAL "BIN") # BIN
    add_executable(OcrLiteNcnn ${OCR_COMPILE_CODE})
    target_compile_definitions(OcrLiteNcnn PRIVATE __EXEC__)
    target_link_libraries(OcrLiteNcnn ncnn ${OpenCV_LIBS} ${OpenMP_CXX_LIB_NAMES})
endif ()

install(TARGETS OcrLiteNcnn EXPORT OcrLiteNcnn)
if (OCR_OUTPUT STREQUAL "CLIB") # CLIB
    file(GLOB OCR_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
    install(FILES ${OCR_INCLUDE} DESTINATION include)
endif ()

if (OCR_VULKAN STREQUAL "GPU")
    target_compile_definitions(OcrLiteNcnn PRIVATE __VULKAN__)
endif ()

# benchmark
if (OCR_BENCHMARK AND (OCR_OUTPUT STREQUAL "BIN"))
    add_executable(benchmark benchmark/benchmark.cpp
            src/AngleNet.cpp
            src/clipper.cpp
            src/CrnnNet.cpp
            src/DbNet.cpp
            src/getopt.cpp
            src/OcrLite.cpp
            src/OcrUtils.cpp)
    target_link_libraries(benchmark ncnn ${OpenCV_LIBS} ${OpenMP_CXX_LIB_NAMES})
    target_compile_definitions(benchmark PRIVATE __EXEC__)

    if (OCR_VULKAN STREQUAL "GPU")
        target_compile_definitions(benchmark PRIVATE __VULKAN__)
    endif ()
    install(TARGETS benchmark EXPORT benchmark
            ARCHIVE DESTINATION staticlib
            LIBRARY DESTINATION sharedlib
            RUNTIME DESTINATION bin)
endif ()
