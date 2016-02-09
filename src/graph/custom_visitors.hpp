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
class layer_filter_vertex_predicate_c {
public:
  layer_filter_vertex_predicate_c() : graph_m(0) {}
  layer_filter_vertex_predicate_c(const Timing_Graph& g, Layer l) : graph_m(&g), layer(l){}
  bool operator()(const vertex_t& vertex_id) const;
private:
  const Timing_Graph* graph_m;
  Layer layer;
};


class lv4_vertex_predicate_c {
public:
  lv4_vertex_predicate_c() : graph_m(0) {}
  lv4_vertex_predicate_c(const Timing_Graph& g) : graph_m(&g) {}
  bool operator()(const vertex_t& vertex_id) const;
private:
  const Timing_Graph* graph_m;
};

class true_edge_predicate {
public:
  true_edge_predicate() : graph_m(0) {}
  true_edge_predicate(const Timing_Graph& g) : graph_m(&g) {}
  bool operator()(const edge_t& edge_id) const;
private:
  const Timing_Graph* graph_m;
};

struct masters_task_research_visitor :public boost::default_dfs_visitor{
  std::vector<std::string>* discovered_tasks;
  masters_task_research_visitor(std::vector<std::string>& discovered)
  {
      discovered_tasks = &discovered;
  }
  using colormap = boost::property_map<Timing_Graph, boost::vertex_color_t>::type;
    colormap vertex_coloring;

    template<typename Vertex, typename Graph>
        void discover_vertex(Vertex v, Graph const& g) {
            std::cout<<"calling discover_vertex"<< g[v].name <<std::endl;
            if(g[v].layer == Layer::TASK)
                discovered_tasks->push_back(g[v].name); //o qualcosa del genere
            //vertex_coloring[v] = boost::default_color_type::black_color;
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



struct masters_task_setter_visitor :public boost::default_dfs_visitor{
  std::vector<timing_vertex_t>* processor_master_tasks;
  std::map<std::string,timing_vertex_t>* reference_map;
  std::set<timing_vertex_t> lockers;
  mutable bool flag;
  masters_task_setter_visitor (std::vector<timing_vertex_t>& results, std::map<std::string,timing_vertex_t>& references_map)
  {
      processor_master_tasks = &results;
      reference_map = &references_map;
  }
  using colormap = boost::property_map<Timing_Graph, boost::vertex_color_t>::type;
    colormap vertex_coloring;

    template<typename Vertex, typename Graph>
        void discover_vertex(Vertex v, Graph const& g) {
            std::cout<<"masters_task_setter_visitor: calling discover_vertex "<< g[v].name <<std::endl;
            if(g[v].is_master && flag)
            {
                flag = false;
                //nodo è const: non posso inserire master task
                processor_master_tasks->push_back(v);
                PRINT_DEBUG("masters_task_setter_visitor: adding node: "+ g[v].name);
                if(g[v].associate_port_name != "")
                {
                    vertex_coloring[reference_map->at(g[v].associate_port_name)] = boost::default_color_type::black_color; ///vero problema. 
                    lockers.insert(v);
                    PRINT_DEBUG("masters_task_setter_visitor: locking");
                }
                
            }
            else if (!g[v].is_master && !flag)
            {
                flag = true;
            }
            //else continue;
            boost::default_dfs_visitor::discover_vertex(v,g);
        }
    template<typename Vertex, typename Graph>
        void start_vertex(Vertex v, Graph const& g) 
        {
            std::cout<<"masters_task_setter_visitor: calling start_vertex "<< g[v].name <<std::endl;
            flag = true;
            boost::default_dfs_visitor::start_vertex(v,g);
        }
    template<typename Vertex, typename Graph>
        void finish_vertex(Vertex v, Graph const& g) {
            std::cout<<"calling finish_vertex"<< g[v].name <<std::endl;
            if (!g[v].is_master && flag)
            {
                flag = false;
            }
            if (lockers.count(v) != 0)
            {
                lockers.erase(v);
                vertex_coloring[reference_map->at(g[v].associate_port_name)] = boost::default_color_type::white_color;
                PRINT_DEBUG("masters_task_setter_visitor: unlocking");
            }
            boost::default_dfs_visitor::finish_vertex(v,g);
        }
};

struct exploration_from_interferes_with_to_visitor :public boost::default_dfs_visitor{
  timing_vertex_t to; 
  std::map<std::string,timing_vertex_t>* reference_map;
  std::set<timing_vertex_t> lockers;
  mutable bool flag;
  exploration_from_interferes_with_to_visitor (timing_vertex_t init_to, std::map<std::string,timing_vertex_t>& references_map)
  {
      to = init_to;
      reference_map = &references_map;
  }
  using colormap = boost::property_map<Timing_Graph, boost::vertex_color_t>::type;
    colormap vertex_coloring;

    template<typename Vertex, typename Graph>
        void discover_vertex(Vertex v, Graph const& g) {
            std::cout<<"exploration_from_interferes_with_to_visitor: calling discover_vertex "<< g[v].name <<std::endl;
            if(g[v].is_master && flag)
            {
                flag = false;
                //nodo da ignorare. devo pero scurire nodo associato.
                PRINT_DEBUG("exploration_from_interferes_with_to_visitor: adding node: "+ g[v].name);
                if(g[v].associate_port_name != "")
                {
                    vertex_coloring[reference_map->at(g[v].associate_port_name)] = boost::default_color_type::black_color; ///vero problema. 
                    lockers.insert(v);
                    PRINT_DEBUG("exploration_from_interferes_with_to_visitor: locking");
                }
                
            }
            else if (!g[v].is_master && !flag)
            {
                flag = true;
            }
            else if (g[v].is_master && !flag)
            {
                if (g[v].master_tasks.count(g[to].name) != 0)
                    throw std::runtime_error ("trovato");
            }
            //else continue;
            boost::default_dfs_visitor::discover_vertex(v,g);
        }
    template<typename Vertex, typename Graph>
        void start_vertex(Vertex v, Graph const& g) 
        {
            std::cout<<"exploration_from_interferes_with_to_visitor: calling start_vertex "<< g[v].name <<std::endl;
            flag = true;
            boost::default_dfs_visitor::start_vertex(v,g);
        }
    template<typename Vertex, typename Graph>
        void finish_vertex(Vertex v, Graph const& g) {
            std::cout<<"calling exploration_from_interferes_with_to_visitor"<< g[v].name <<std::endl;
            if (!g[v].is_master && flag)
            {
                flag = false;
            }
            if (lockers.count(v) != 0)
            {
                lockers.erase(v);
                vertex_coloring[reference_map->at(g[v].associate_port_name)] = boost::default_color_type::white_color;
                PRINT_DEBUG("exploration_from_interferes_with_to_visitor: unlocking");
            }
            boost::default_dfs_visitor::finish_vertex(v,g);
        }
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
