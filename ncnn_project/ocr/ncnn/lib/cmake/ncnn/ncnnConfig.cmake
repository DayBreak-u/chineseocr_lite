set(NCNN_OPENMP ON)
set(NCNN_VULKAN OFF)

if(NCNN_OPENMP)
    find_package(OpenMP)
endif()

if(NCNN_VULKAN)
    find_package(Vulkan REQUIRED)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/ncnn.cmake)
