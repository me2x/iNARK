cmake_minimum_required (VERSION 2.8)
project (iNARK)


option(DEBUG "debug option" OFF)

#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Compiler Options
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++0x -Wall -Wextra -Werror")

#include_directories($(FOO_SOURCE_DIR)/include)

add_subdirectory(src)
