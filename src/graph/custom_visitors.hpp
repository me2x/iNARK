#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/undirected_dfs.hpp>
#include "graph_common.hpp"
#if 0
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


#endif
struct masters_task_research_visitor :public boost::default_dfs_visitor{
  std::vector<vertex_t>* discovered_tasks;
  masters_task_research_visitor(std::vector<vertex_t>& discovered)
  {
      discovered_tasks = &discovered;
  }
  using colormap = boost::property_map<Timing_Graph, boost::vertex_color_t>::type;
    colormap vertex_coloring;

    template<typename Vertex, typename Graph>
        void discover_vertex(Vertex v, Graph const& g) {
            std::cout<<"calling discover_vertex"<< v <<std::endl;
            if(g[v].layer == Layer::TASK)
                discovered_tasks->push_back(v); //o qualcosa del genere
            vertex_coloring[v] = boost::default_color_type::black_color;
            boost::default_dfs_visitor::discover_vertex(v,g);
        }
        
    /*template<typename Vertex, typename Graph>
        void finish_vertex(Vertex v, Graph const& g) {
            std::cout<<"calling finish_vertex"<< v <<std::endl;
            default_color_type color = vertex_coloring[v];
            (void) color; // suppress unused warning
            vertex_coloring[v] = boost::default_color_type::red_color;
            boost::default_dfs_visitor::finish_vertex(v,g);
        }
        */
     /*template<typename Vertex, typename Graph>
        void start_vertex(Vertex v, Graph const& g) {
            std::cout<<"calling start_vertex"<< v <<std::endl;
            boost::default_dfs_visitor::start_vertex(v,g);
        }*/    
     /*       template<typename Vertex, typename Graph>
        void initialize_vertex(Vertex v, Graph const& g) {
            std::cout<<"calling initialize_vertex"<< v <<std::endl;
            boost::default_dfs_visitor::initialize_vertex(v,g);
        } 
    */
  
  
};
//can be used both in search 2 and 3 (that are both one to many). the underlying graph is the only thing that changes between those two searches
#if 0
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
#endif
