cmake_minimum_required (VERSION 3.0.2)
project (iNARK)

#set(Boost_INCLUDE_DIR /usr/include)
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
#link_directories(${Boost_LIBRARY_DIR})


SET(GRAPH_SRCS
graph.hpp
graph.cpp
timing_internal_graph.hpp
timing_internal_graph.cpp
graph_common.hpp
graph_common.cpp
custom_visitors.hpp
custom_visitors.cpp
debug_msg_handler.hpp
)

add_library (graph ${GRAPH_SRCS})

target_link_libraries( graph ${Boost_LIBRARIES})
