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
    "src/scenes/*.cpp"
    "src/scenes/*.h"
    "src/shapes/*.cpp"
    "src/shapes/*.h"
    "src/shapes/meshloaders/*.cpp"
    "src/shapes/meshloaders/*.h"
    "src/utility/*.cpp"
    "src/utility/*.h"
    "src/volumes/*.cpp"
    "src/volumes/*.h")
    
SET(ALL_FILES 
    ${RAINBOW_SOURCE})

########################################
# Sort the file into different folders

foreach(_source_file IN ITEMS ${RAINBOW_SOURCE})
	get_filename_component(_source_path "${_source_file}" PATH)
	string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
	string(REPLACE "/" "\\" _group_path "${_group_path}")
	source_group("${_group_path}" FILES "${_source_file}")
endforeach()

########################################
# Aligned memory allocation

CHECK_CXX_SOURCE_COMPILES ( "
#include <malloc.h>
int main() { void * ptr = _aligned_malloc(1024, 32); }
" HAVE__ALIGNED_MALLOC )

CHECK_CXX_SOURCE_COMPILES ( "
#include <stdlib.h>
int main() {
  void *ptr;
  posix_memalign(&ptr, 32, 1024);
} " HAVE_POSIX_MEMALIGN )

CHECK_CXX_SOURCE_COMPILES ( "
#include <malloc.h>
int main() {
    void *ptr = memalign(32, 1024);
} " HAVE_MEMALIGN )

IF ( HAVE__ALIGNED_MALLOC )
  ADD_DEFINITIONS ( -D RAINBOW_HAVE__ALIGNED_MALLOC )
ELSEIF ( HAVE_POSIX_MEMALIGN )
  ADD_DEFINITIONS ( -D RAINBOW_HAVE_POSIX_MEMALIGN )
ELSEIF ( HAVE_MEMALIGN )
  ADD_DEFINITIONS ( -D RAINBOW_HAVE_MEMALIGN )
ELSE ()
  MESSAGE ( SEND_ERROR "Unable to find a way to allocate aligned memory" )
ENDIF ()

########################################
    
ADD_EXECUTABLE(Rainbow
    ${RAINBOW_SOURCE})

TARGET_LINK_LIBRARIES(Rainbow 
    ${EMBREE_LIBRARY}
    tbb_static) 
