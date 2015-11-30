cmake_minimum_required (VERSION 2.8)
project (iNARK)

# The version number.
set (iNARK_VERSION_MAJOR 0)
set (iNARK_VERSION_MINOR 0)



# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/iNARKConfig.h.in"
  "${PROJECT_BINARY_DIR}/iNARKConfig.h"
  )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}")
add_subdirectory (graph)

add_executable(iNARK iNARK.cpp)


target_link_libraries( iNARK graph)
