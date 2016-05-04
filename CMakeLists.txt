cmake_minimum_required (VERSION 3.0.2)
project (iNARK)

option(DEBUG "debug option" OFF)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})



set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Compiler Options
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++0x -Wall -Wextra -Werror")

#include_directories($(FOO_SOURCE_DIR)/include)

add_subdirectory(src)
