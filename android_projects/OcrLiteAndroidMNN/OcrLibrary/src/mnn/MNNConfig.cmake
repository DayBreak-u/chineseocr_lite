set(MNN_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
message("配置MNN include: ${MNN_INCLUDE_DIRS}")
include_directories(${MNN_INCLUDE_DIRS})

set(MNN_LIBS libMNNruntime)
add_library(${MNN_LIBS} SHARED IMPORTED)
set_target_properties(${MNN_LIBS} PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../mnn/${ANDROID_ABI}/libMNN.so)

set(MNN_FOUND TRUE)