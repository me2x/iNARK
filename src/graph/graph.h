
#include <boost/graph/graph_utility.hpp>
#include "graph_common.h"
#include "custom_visitors.h"


class extern_edge_predicate_c;
class extern_vertex_predicate_c;
class extern_visitor;

class custom_graph {
public:
    custom_graph() {}

    bool filter_and_explore_graph(Priority p,std::string from, std::string to);
    bool create_graph(std::string xml_location);
    const Graph get_graph();
    std::map<edge_t,int> edge_to_port_map;

private:
    Graph local_graph;
    std::map<std::string, vertex_t> vertex_map;

};
