#include "custom_visitor.h"
#include <iostream>

bool custom_edge_predicate_c::operator()(const edge_t& edge_id) const {
  Priority type = (*graph_m)[edge_id].priority;
  return (type >= threshold);
}


my_visitor::my_visitor(vertex_t destination_vertex_l): destination_vertex_m(destination_vertex_l) {
    std::cout<<"blah"<<std::endl;
    std::cout<<destination_vertex_m<<std::endl;
}
void my_visitor::initialize_vertex(const vertex_t s, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const {}
void my_visitor::discover_vertex(const vertex_t s, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g)const {
    std::cout<<"exploring, found vertex: "<<s<<std::endl;

    if (destination_vertex_m == s)
      throw(2);
}

void my_visitor::examine_vertex(const vertex_t s, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const {}
void my_visitor::examine_edge(const edge_t e, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const {}
void my_visitor::edge_relaxed(const edge_t e, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const {}
void my_visitor::edge_not_relaxed(const edge_t e, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const {}
void my_visitor::finish_vertex(const vertex_t s, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const {}
