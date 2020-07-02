cmake_minimum_required(VERSION 2.8)

project(OCR)

add_definitions(-std=c++11)
add_definitions("-Wall")


FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ncnn/include/ncnn)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ncnn/lib)

add_definitions(-DUNICODE -D_UNICODE)

find_package( OpenCV REQUIRED )

if (NOT OpenCV_FOUND)
    message(FATAL_ERROR "opencv Not Found!")
else()
    message(STATUS "OpenCV_LIBS: ${OpenCV_LIBS}")
    message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
endif (NOT OpenCV_FOUND)


set(SRC
        main.cpp
        )

add_executable(Text_detext ${SRC})
target_link_libraries(Text_detext ncnn ${OpenCV_LIBS})

