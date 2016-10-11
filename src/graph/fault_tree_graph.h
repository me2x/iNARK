#ifndef FAULT_TREE_GRAPH_H
#define FAULT_TREE_GRAPH_H

#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/copy.hpp>
#include "graph_common.hpp"
#include <memory>

class fault_tree_graph
{
    public:
    fault_tree_graph ();
    void build_graph (std::shared_ptr<Source_Graph> g);
    void print_FTAs ();

private:
    FT_Graph ig;
    ft_vertex_t get_node_reference(std::string str);
    
};

#endif // FAULT_TREE_GRAPH_H
