#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "debug_msg_handler.hpp"

#ifndef GRAPH_COMMON_H
#define GRAPH_COMMON_H
enum Layer{
    FUNCTION,
    TASK,
    CONTROLLER,
    RESOURCE,
    PHYSICAL,
    LAYER_ERROR
};

enum Priority{
    NO_PRIORITY,
    MISSION_CRITICAL,
    SAFETY_CRITICAL,
    PRIORITY_ERROR
};

enum Component_Type{
    ROUND_ROBIN,
    PRIORITY,
    TDMA,
    TYPE_ERROR
};

class Custom_Vertex
{
public:
    Layer layer;
    std::string name;
    std::map<int,Priority> ports; //TO REFINE tipo aggiungere solo per Lay 4.
    Component_Type type;
};
class Custom_Edge
{
public:
    Custom_Edge(void) : priority(NO_PRIORITY){}
    Custom_Edge(Priority p) : priority(p){}
    Priority priority;
};


typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,Custom_Vertex,Custom_Edge> Graph;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,Custom_Vertex,Custom_Edge> Internal_Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Internal_Graph>::edge_descriptor inner_edge_t;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
typedef boost::graph_traits<Internal_Graph>::vertex_iterator inner_vertex_iter;
typedef boost::graph_traits<Graph>::edge_iterator edge_iter;
typedef boost::graph_traits<Internal_Graph>::edge_iterator inner_edge_iter;
#endif
