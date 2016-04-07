cmake_minimum_required (VERSION 3.0.2)
project (iNARK)

# The version number.
set (iNARK_VERSION_MAJOR 0)
set (iNARK_VERSION_MINOR 0)

set (DEBUG OFF CACHE BOOL "Some user specified option")
#copiato non ho idea del funzionamento
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
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
add_subdirectory(graphical)

add_executable(iNARK iNARK.cpp)


target_link_libraries( iNARK graph graphical)

