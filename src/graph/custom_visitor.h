#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include "graph_common.h"


class custom_edge_predicate_c {
public:
  custom_edge_predicate_c() : graph_m(0) ,threshold(NO_PRIORITY){}
  custom_edge_predicate_c(Graph& graph,Priority threshold) : graph_m(&graph), threshold (threshold) {}
  bool operator()(const edge_t& edge_id) const;
private:
  Graph* graph_m;
  Priority threshold;
};


class my_visitor :public boost::default_bfs_visitor{
protected:
  vertex_t destination_vertex_m;
public:
  my_visitor(vertex_t destination_vertex_l);
  void initialize_vertex(const vertex_t s, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const ;
  void discover_vertex(const vertex_t s, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g)const ;
  void examine_vertex(const vertex_t s, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const ;
  void examine_edge(const edge_t e, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const ;
  void edge_relaxed(const edge_t e, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const ;
  void edge_not_relaxed(const edge_t e, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const ;
  void finish_vertex(const vertex_t s, const boost::filtered_graph<Graph, custom_edge_predicate_c>  g) const ;
};
