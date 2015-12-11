#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/graphviz.hpp>
#include "graph_common.h"

class internal_graph
       {
public:
    internal_graph (const Graph& g);

private:
    Internal_Graph ig;
    vertex_t get_node_reference (std::string);
};
