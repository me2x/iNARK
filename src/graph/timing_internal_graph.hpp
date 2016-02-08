//#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/copy.hpp>
#include "graph_common.hpp"

class timing_internal_graph
       {
public:
    timing_internal_graph (Source_Graph g);
    bool search_path (std::string from, std::string to, Layer l);
    //void search_interfered_nodes (std::string source, bool reverse);
private:
    Timing_Graph ig;
    timing_vertex_t get_node_reference (std::string);
    std::map<std::string, std::map< int, std::string> > components_map;
    
};
