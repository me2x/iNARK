#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/undirected_dfs.hpp>
#include "graph_common.h"


class inner_edge_predicate_c {
public:
  inner_edge_predicate_c() : graph_m(0) ,search_layer(LAYER_ERROR), target_os(0){}
  inner_edge_predicate_c(Internal_Graph& ig,Layer search_layer, vertex_t target_os) : graph_m(&ig), search_layer (search_layer),target_os(target_os) {}
  bool operator()(const inner_edge_t& edge_id) const;
private:
  Internal_Graph* graph_m;
  Layer search_layer;
  vertex_t target_os;
};

class inner_vertex_predicate_c {
public:
  inner_vertex_predicate_c() : graph_m(0) ,threshold(PHYSICAL){}
  inner_vertex_predicate_c(Internal_Graph& ig,Layer threshold) : graph_m(&ig), threshold (threshold) {}
  bool operator()(const vertex_t& vertex_id) const;
private:
  Internal_Graph* graph_m;
  Layer threshold;
};
class inner_visitor :public boost::default_bfs_visitor{
protected:
  vertex_t destination_vertex_m;
public:
  inner_visitor(vertex_t destination_vertex_l);
  void initialize_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void discover_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g)const ;
  void examine_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void examine_edge(const inner_edge_t e, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void edge_relaxed(const inner_edge_t e, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void edge_not_relaxed(const inner_edge_t e, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void finish_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
};

class extern_vertex_predicate_c {
public:
  extern_vertex_predicate_c() : graph_m(0) {}
  extern_vertex_predicate_c(const Internal_Graph& g) : graph_m(&g) {}
  bool operator()(const vertex_t& vertex_id) const;
private:
  const Internal_Graph* graph_m;
};

class extern_edge_predicate_c {
public:
  extern_edge_predicate_c() : graph_m(0) {}
  extern_edge_predicate_c(const Internal_Graph& g) : graph_m(&g) {}
  bool operator()(const inner_edge_t& edge_id) const;
private:
  const Internal_Graph* graph_m;
};


class extern_visitor :public boost::default_dfs_visitor{
protected:
  std::vector<vertex_t>* discovered_vertexes;
  vertex_t* source;
public:
  extern_visitor(std::vector<vertex_t>& discovered,vertex_t& source_par);
  void initialize_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const ;
  void discover_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g)const ;
  void examine_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const ;
  void examine_edge(const inner_edge_t e, const boost::filtered_graph<Internal_Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const ;
  void edge_relaxed(const inner_edge_t e, const boost::filtered_graph<Internal_Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const ;
  void edge_not_relaxed(const inner_edge_t e, const boost::filtered_graph<Internal_Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const ;
  void finish_vertex(const vertex_t s, const boost::filtered_graph<Internal_Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const ;
};
