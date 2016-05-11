cmake_minimum_required (VERSION 3.0.2)
project (iNARK)

# The version number.
set (iNARK_VERSION_MAJOR 0)
set (iNARK_VERSION_MINOR 0)

set (DEBUG OFF CACHE BOOL "Some user specified option")
set (TIME OFF CACHE BOOL "Some user specified option")
set (SEM_CHECK OFF CACHE BOOL "Some user specified option")
if (SEM_CHECK)
    set (SEM_CHECK_BOOL "true")
else(SEM_CHECK)
    set (SEM_CHECK_BOOL "false")
endif (SEM_CHECK)
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


include(ExternalProject)

set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_SOURCE_DIR}/src/commons) #folder where the project will be imported. 

ExternalProject_Add(demo
  GIT_REPOSITORY https://github.com/me2x/iNARK_Commons.git
  GIT_TAG master
  UPDATE_COMMAND ""
  INSTALL_COMMAND ""
)

ExternalProject_Get_Property(demo source_dir) #gets automatically the main directory of the imported project
include_directories(${source_dir})
message(${source_dir})


ExternalProject_Get_Property(demo binary_dir) #automatically gets the build directory
message(${binary_dir})


add_subdirectory (graph)

add_executable(iNARK iNARK.cpp)


target_link_libraries( iNARK graph ${binary_dir}/libinark_commons.a)

