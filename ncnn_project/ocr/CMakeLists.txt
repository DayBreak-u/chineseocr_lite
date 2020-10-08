cmake_minimum_required(VERSION 3.0)

project(chocrlite_ncnn)
add_definitions("-Wall")


FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
    message("OPENMP FOUND")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


IF (WIN32)
	MESSAGE(STATUS "Now is windows")
    add_definitions(-DWIN32)
    add_definitions(-D__COL_DLL_EXPORT_LIBRARY)
ELSEIF (APPLE)
	MESSAGE(STATUS "Now is Apple systens.")
ELSEIF (UNIX)
	MESSAGE(STATUS "Now is UNIX-like OS's.")
     add_definitions(-DUNIX)
ENDIF ()


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ncnn/include/ncnn)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/ncnn/lib)



# need to modify this to fit in your environment.
include_directories(E:\\ocr4win\\opencv\\build-x86\\win-install\\include)  # for opencv direcotry
include_directories(C:\\VulkanSDK\\1.2.148.1\\Include)                     # for  vulkandk include dir , if no, comment it

link_directories(E:\\ocr4win\\opencv\\build-x86\\win-install\\x86\\vc16\\lib) # library dir for opencv
link_directories(C:\\VulkanSDK\\1.2.148.1\\Lib32)                              #library dir for vulkan





set(SRC_LIB
        src/chocrlite.cpp
        src/ocr.cpp
        src/ZUtil.cpp
        src/clipper.cpp
        )


add_library(chocrlite SHARED ${SRC_LIB} "include/precomp.h" "include/chocrlite_warning.h")

target_link_libraries(chocrlite ncnn )
target_link_libraries(chocrlite vulkan-1)  #if there's no vaulkan, please comment it
target_link_libraries(chocrlite opencv_world3411)

set_target_properties(chocrlite PROPERTIES SOVERSION 1)

set(SRC
        src/main.cpp
         )

add_executable(chocrlite_ncnn ${SRC} "include/precomp.h" "include/chocrlite_warning.h")


target_link_libraries(chocrlite_ncnn  chocrlite)
