#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>


#ifndef GRAPH_COMMON_H
#define GRAPH_COMMON_H
enum Layer{
    FUNCTION,
    TASK,
    CONTROLLER,
    RESOURCE,
    PHYSICAL
};

enum Priority{
    NO_PRIORITY,
    MISSION_CRITICAL,
    SAFETY_CRITICAL
};

class Custom_Vertex
{
public:
    Layer layer;
    std::string name;
};
class Custom_Edge
{
public:
    Custom_Edge(void) : priority(NO_PRIORITY){}
    Custom_Edge(Priority p) : priority(p){}
    Priority priority;
};


typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,Custom_Vertex,Custom_Edge> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
#endif
