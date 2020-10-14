cmake_minimum_required(VERSION 3.13)

project(chocrlite_ncnn)
add_definitions("-Wall")

if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-std=c++11)
    message(STATUS "optional:-std=c++11")   
endif(CMAKE_COMPILER_IS_GNUCXX)


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
# include_directories(C:\\VulkanSDK\\1.2.148.1\\Include)                     # for  vulkandk include dir , if no, comment it

link_directories(E:\\ocr4win\\opencv\\build-x86\\lib\\Release) # library dir for opencv

link_directories(E:\\ocr4win\\opencv\\build-x86\\3rdparty\\lib\\Release)




set(GPU false)  #switch for gpu

if(GPU)
add_definitions(-DUSE_GPU=1)
target_link_libraries(chocrlite vulkan-1)  
#link_directories(C:\\VulkanSDK\\1.2.148.1\\Lib32)                              #library dir for vulkan
endif()

set(SRC_LIB
        src/chocrlite.cpp
        src/ocr.cpp
        src/ZUtil.cpp
        src/clipper.cpp
        src/RRLib.cpp
        )


add_library(chocrlite SHARED ${SRC_LIB} "include/precomp.h" "include/chocrlite_warning.h")

target_link_libraries(chocrlite ncnn )

target_link_libraries(chocrlite opencv_world3411)

#set_target_properties(chocrlite PROPERTIES LINK_FLAGS "/VERSION:5.1")
#target_link_options(chocrlite PRIVATE  /VERSION:5.1)

#target_link_libraries(chocrlite opencv_imgcodecs440)
#target_link_libraries(chocrlite opencv_core440)




set_target_properties(chocrlite PROPERTIES
       VERSION    5.1
)


#for being compatible with windows xp

#if(CMAKE_CXX_COMPILER_ID)
set_target_properties(chocrlite
    PROPERTIES
    LINK_FLAGS /VERSION:"5.1"
    )
#endif()

set(SRC
        src/main.cpp
         )

add_executable(chocrlite_ncnn ${SRC} "include/precomp.h" "include/chocrlite_warning.h")


target_link_libraries(chocrlite_ncnn  chocrlite)

#if(CMAKE_CXX_COMPILER_ID)
set_target_properties(chocrlite_ncnn
    PROPERTIES
    LINK_FLAGS /VERSION:"5.1"
    )
#endif()
