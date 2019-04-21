# Marco for build fiter for VS
macro(source_group_by_dir source_files)
    if(MSVC)
        set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
        foreach(sgbd_file ${${source_files}})
            string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(MSVC)
endmacro(source_group_by_dir)

cmake_minimum_required (VERSION 3.1)

# The project name
PROJECT (Rainbow)

# EXT
ADD_SUBDIRECTORY(src/ext ext_build)
# Embree
SET(EMBREE_ROOT_DIR ${PROJECT_SOURCE_DIR}/src/ext/embree3)
SET(EMBREE_INCLUDE_DIR ${EMBREE_ROOT_DIR}/include)
SET(EMBREE_LIBRARY_DIR ${EMBREE_ROOT_DIR}/lib)
MESSAGE(STATUS ${EMBREE_ROOT_DIR})
MESSAGE(STATUS ${EMBREE_INCLUDE_DIR})
MESSAGE(STATUS ${EMBREE_LIBRARY_DIR})

FILE(GLOB EMBREE_LIBRARY
    ${EMBREE_LIBRARY_DIR}/embree3.lib)  

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/src
    # Intel Thread Building Blocks
    ${TBB_INCLUDE_DIR}
    # Embree
    ${EMBREE_INCLUDE_DIR}) 

# Use folder
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

FILE(GLOB RAINBOW_SOURCE
    "src/rainbow.cpp"
    "src/ext/halton/*.h"
    "src/ext/*.cpp"
    "src/ext/*.h"
    "src/ext/*.hpp"
    "src/accelerators/*.cpp"
    "src/accelerators/*.h"
    "src/cameras/*.cpp"
    "src/cameras/*.h"
    "src/core/*.cpp"
    "src/core/*.h"
    "src/filters/*.cpp"
    "src/filters/*.h"
    "src/integrators/*.cpp"
    "src/integrators/*.h"
    "src/lights/*.cpp"
    "src/lights/*.h"
    "src/lights/ies/*.cpp"
    "src/lights/ies/*.h"
    "src/materials/*.cpp"
    "src/materials/*.h" 
    "src/media/*.cpp"
    "src/media/*.h"
    "src/samplers/*.cpp"
    "src/samplers/*.h"
    "src/shapes/*.cpp"
    "src/shapes/*.h"
    "src/shapes/meshloaders/*.cpp"
    "src/shapes/meshloaders/*.h")
    
SET(ALL_FILES 
    ${RAINBOW_SOURCE})

source_group_by_dir(RAINBOW_SOURCE)
    
ADD_EXECUTABLE(Rainbow
    ${RAINBOW_SOURCE})

TARGET_LINK_LIBRARIES(Rainbow 
    ${EMBREE_LIBRARY}
    tbb_static) 