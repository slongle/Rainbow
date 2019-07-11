#${TBB_INCLUDE_DIR}
# Embree
#${EMBREE_INCLUDE_DIR}
# OpenVDB
#${OPENVDB_INCLUDE_DIR}

#${EMBREE_LIBRARY}
#${TBB_LIBRARY}
#${OPENVDB_LIBRARY}

########################################
# Set source file

# SET(GLOB SOURCE
#     #"filesystem/filesystem/*.h"
#     #"halton/*.h"
#     #"pcg32/*.h"
#     "pugixml/*.hpp"
#     "pugixml/*.cpp"
#     #"stb_images/*.h"
#     #"tinyformat/*.h"
#     #"tinyobjloader/*.h"
#     #"tinyobjloader/*.cc"
#     )
# MESSAGE(STATUS ${SOURCE})
# SET(${EXT_SOURCE} ${SOURCE} PARENT_SCOPE)

########################################
# Set Embree3, TBB and OpenVDB

# Embree3
#find_package(embree3 CONFIG REQUIRED)