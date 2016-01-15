#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/undirected_dfs.hpp>
#include "graph_common.h"
class extern_vertex_predicate_c {
public:
  extern_vertex_predicate_c() : graph_m(0) {}
  extern_vertex_predicate_c(const Timing_Graph& g) : graph_m(&g) {}
  bool operator()(const vertex_t& vertex_id) const;
private:
  const Timing_Graph* graph_m;
};

class extern_edge_predicate_c {
public:
  extern_edge_predicate_c() : graph_m(0) {}
  extern_edge_predicate_c(const Timing_Graph& g) : graph_m(&g) {}
  bool operator()(const inner_edge_t& edge_id) const;
private:
  const Timing_Graph* graph_m;
};

class inner_edge_predicate_c {
public:
  inner_edge_predicate_c() : graph_m(0) ,search_layer(LAYER_ERROR), source(0){}
  inner_edge_predicate_c(Timing_Graph& ig, Layer search_layer_input, vertex_t source_os): graph_m(&ig) ,search_layer(search_layer_input), source(source_os){}
  bool operator()(const inner_edge_t& edge_id) const;
private:
  Timing_Graph* graph_m;
  Layer search_layer;
  vertex_t source;
};

class inner_vertex_predicate_c {
public:
  inner_vertex_predicate_c() : graph_m(0) ,threshold(PHYSICAL),match_name(""),match_priority(""){}
  inner_vertex_predicate_c(Timing_Graph& ig,Layer threshold_input,std::string source_os);
  bool operator()(const vertex_t& vertex_id) const;
private:
  Timing_Graph* graph_m;
  Layer threshold;
  std::string match_name;
  std::string match_priority;
};
class source_to_target_visitor :public boost::default_dfs_visitor{
protected:
  mutable bool start_flag;
  vertex_t destination_vertex_m;
  std::vector<vertex_t>* path_vertexes;
public:
  source_to_target_visitor(std::vector<vertex_t>& discovered,vertex_t destination_vertex_l);
  void initialize_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void start_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void discover_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void examine_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void tree_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void back_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void forward_or_cross_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void finish_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
};
//can be used both in search 2 and 3 (that are both one to many). the underlying graph is the only thing that changes between those two searches
class interference_visitor :public boost::default_dfs_visitor{
protected:
  std::vector<vertex_t>* discovered_vertexes;
  mutable bool start_flag;
public:
  interference_visitor(std::vector<vertex_t>& discovered);
  void initialize_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void start_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void discover_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void examine_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void tree_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void back_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void forward_or_cross_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
  void finish_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const ;
};
