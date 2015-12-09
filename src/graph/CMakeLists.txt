cmake_minimum_required (VERSION 2.8)
project (iNARK)

set(Boost_INCLUDE_DIR /usr/include)
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})


SET(GRAPH_SRCS
graph.h
graph.cpp
internal_graph.h
internal_graph.cpp
graph_common.h
custom_visitors.h
custom_visitors.cpp
debug_msg_handler.hpp
)

add_library (graph ${GRAPH_SRCS})

target_link_libraries( graph ${Boost_LIBRARIES})
