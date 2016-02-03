#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/copy.hpp>
#include "graph_common.hpp"

class timing_internal_graph
       {
public:
    timing_internal_graph (const Source_Graph& g);
    bool search_path (std::string from, std::string to, Layer l);
    //void search_interfered_nodes (std::string source, bool reverse);
private:
    Timing_Graph ig;
    vertex_t get_node_reference (std::string);
    std::map <std::string,std::string> lvl_4_to_3_map; //4th lvl is key. 3rd is value. 1 processor can be related to 1 os, no more. an os can have more processors
    std::map<int,std::vector<int> > priority_to_ports_map; //the priority is the key, the vector of ports with that priority is the value. more ports with the same priority are handled as round robin. has to be cleaned after every component use.
};
