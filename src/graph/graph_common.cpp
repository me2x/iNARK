#include "graph_common.hpp"


void First_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) const {
    timing_vertex_t vt = boost::add_vertex(graph);
    graph[vt].name = this->name;
    graph[vt].layer = this->layer;
    
}

void Second_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) const {
    timing_vertex_t vt = boost::add_vertex(graph);
    graph[vt].name = this->name;
    graph[vt].layer = this->layer;
    
}