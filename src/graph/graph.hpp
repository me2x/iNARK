
#include <boost/graph/graph_utility.hpp>
#include "graph_common.hpp"
#include "custom_visitors.hpp"


class extern_edge_predicate_c;
class extern_vertex_predicate_c;
class extern_visitor;

class source_graph {
public:
    source_graph() {}

    //bool search_component_dependences(std::string from, std::string to);
    bool create_graph(std::string xml_location);
    const Source_Graph get_graph();
    
private:
    Source_Graph local_graph;
    std::map<std::string, vertex_t> vertex_map;

};
