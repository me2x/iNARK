#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include "graph_common.h"


class inner_edge_predicate_c {
public:
  inner_edge_predicate_c() : graph_m(0) ,threshold(NO_PRIORITY){}
  inner_edge_predicate_c(Internal_Graph& ig,Priority threshold) : graph_m(&ig), threshold (threshold) {}
  bool operator()(const inner_edge_t& edge_id) const;
private:
  Internal_Graph* graph_m;
  Priority threshold;
};


class inner_visitor :public boost::default_bfs_visitor{
protected:
  vertex_t destination_vertex_m;
public:
  inner_visitor(vertex_t destination_vertex_l);
  void initialize_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g) const ;
  void discover_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g)const ;
  void examine_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g) const ;
  void examine_edge(const inner_edge_t e, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g) const ;
  void edge_relaxed(const inner_edge_t e, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g) const ;
  void edge_not_relaxed(const inner_edge_t e, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g) const ;
  void finish_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c>  g) const ;
};

class extern_vertex_predicate_c {
public:
  extern_vertex_predicate_c() : graph_m(0) {}
  extern_vertex_predicate_c(const Graph& g) : graph_m(&g) {}
  bool operator()(const vertex_t& vertex_id) const;
private:
  const Graph* graph_m;
};

class extern_edge_predicate_c {
public:
  extern_edge_predicate_c() : graph_m(0) {}
  extern_edge_predicate_c(const Graph& g) : graph_m(&g) {}
  bool operator()(const edge_t& edge_id) const;
private:
  const Graph* graph_m;
};


class extern_visitor :public boost::default_bfs_visitor{
protected:
  vertex_t destination_vertex_m;
public:
  extern_visitor(vertex_t destination_vertex_l);
  void initialize_vertex(const vertex_t s, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const ;
  void discover_vertex(const vertex_t s, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g)const ;
  void examine_vertex(const vertex_t s, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const ;
  void examine_edge(const edge_t e, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const ;
  void edge_relaxed(const edge_t e, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const ;
  void edge_not_relaxed(const edge_t e, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const ;
  void finish_vertex(const vertex_t s, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const ;
};
