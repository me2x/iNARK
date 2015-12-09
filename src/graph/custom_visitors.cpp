#include "custom_visitors.h"


bool inner_edge_predicate_c::operator()(const inner_edge_t& edge_id) const{
      Priority type = (*graph_m)[edge_id].priority;
      return (type >= threshold);
    }


inner_visitor::inner_visitor(vertex_t destination_vertex_l): destination_vertex_m(destination_vertex_l) {
    PRINT_DEBUG("prova");
    PRINT_DEBUG (destination_vertex_m);
}
void inner_visitor::initialize_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g) const {}
void inner_visitor::discover_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g)const {
    PRINT_DEBUG("exploring, found vertex: "<<s);

    if (destination_vertex_m == s)
      throw(2);
}

void inner_visitor::examine_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g) const {}
void inner_visitor::examine_edge(const inner_edge_t e, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g) const {}
void inner_visitor::edge_relaxed(const inner_edge_t e, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g) const {}
void inner_visitor::edge_not_relaxed(const inner_edge_t e, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g) const {}
void inner_visitor::finish_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g) const {}







bool extern_vertex_predicate_c::operator()(const vertex_t& vertex_id) const{
    Layer layer = (*graph_m)[vertex_id].layer;
    return (layer == RESOURCE);
}

//se layer from o layer to è diverso da 4, buttalo. da rifare
bool extern_edge_predicate_c::operator()(const edge_t& edge_id) const{
    //Priority type = (*graph_m)[edge_id].priority;
    
    Layer l_src = (*graph_m)[boost::source(edge_id,*graph_m)].layer;
    Layer l_dst = (*graph_m)[boost::target(edge_id,*graph_m)].layer;
    return (l_src==l_dst);
}

//cercare se vertice ha una sola connessione.
//prova a vedere se puo aver senso un visitor??????? penso possa bastare vertex.get_edges.count o qualcosa del genere
extern_visitor::extern_visitor(vertex_t destination_vertex_l): destination_vertex_m(destination_vertex_l) {
    PRINT_DEBUG (destination_vertex_m);

}
void extern_visitor::initialize_vertex(const vertex_t s, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const {}
void extern_visitor::discover_vertex(const vertex_t s, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g)const {}
void extern_visitor::examine_vertex(const vertex_t s, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const {}
void extern_visitor::examine_edge(const edge_t e, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const {}
void extern_visitor::edge_relaxed(const edge_t e, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const {}
void extern_visitor::edge_not_relaxed(const edge_t e, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const {}
void extern_visitor::finish_vertex(const vertex_t s, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const {}

