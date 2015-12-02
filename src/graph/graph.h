
#include <boost/graph/graph_utility.hpp>
#include "graph_common.h"
#include "custom_visitor.h"


class custom_edge_predicate_c;
class my_visitor;

class custom_graph {
public:
    custom_graph() {}

    bool filter_and_explore_graph(Priority p,std::string from, std::string to);
    bool create_graph(std::string xml_location);


private:
    Graph local_graph;
    std::map<std::string, vertex_t> vertex_map;
};
