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
fault_tree_graph.cpp
fault_tree_graph.h
)

add_library (graph ${GRAPH_SRCS})
add_dependencies(graph demo) #force the dependency solution
target_link_libraries( graph ${binary_dir}/libinark_commons.a)#linking is performed here. the name is the one in the CMakeLIst of the project.
target_link_libraries( graph ${Boost_LIBRARIES})
