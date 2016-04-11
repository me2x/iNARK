
#include <boost/graph/graph_utility.hpp>
#include "graph_common.hpp"
#include "custom_visitors.hpp"
#include <memory>
//TODO controlla che le forward servano a qualcosa.
class extern_edge_predicate_c;
class extern_vertex_predicate_c;
class extern_visitor;

class source_graph {
public:
    source_graph() { local_graph.reset(new Source_Graph());}

    //bool search_component_dependences(std::string from, std::string to);
    bool create_graph(std::string xml_location);
    bool create_graph_from_xml(std::string xml_location);
    std::shared_ptr<Source_Graph> get_source_graph_ref();
    void add_L1_node(std::string name);
    void add_L2_node(std::string name);
    void add_L3_node(std::string name, std::shared_ptr<std::map<int,Scheduler_Slot>> scheduler, Component_Priority_Category handling); //sched slot e port handler li faccio costruire a monte
    void add_L4_node(std::string name, std::shared_ptr<std::map<int,Port>> ports, Component_Type type,Component_Priority_Category handling); //se no troppe cose da passare come "int struct int"
    void add_L5_node(std::string name);
    
    
private:
    std::shared_ptr<Source_Graph> local_graph;
    std::map<std::string, vertex_t> vertex_map;

};
