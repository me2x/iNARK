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

class task_search_filter_c {
public:
  task_search_filter_c() : graph_m(0) {}
  task_search_filter_c(const Timing_Graph& g) : graph_m(&g){}
  bool operator()(const vertex_t& vertex_id) const;
private:
  const Timing_Graph* graph_m;
};

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
            PRINT_DEBUG("calling discover_vertex"<< g[v].name );
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
  std::map<timing_vertex_t, std::set<timing_vertex_t>> to_be_whited_on_callback ;
  timing_vertex_t actual_master;
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
            PRINT_DEBUG("masters_task_setter_visitor: calling discover_vertex "<< g[v].name );
            if(g[v].is_master && flag)
            {
                flag = false;
                //nodo è const: non posso inserire master task
                actual_master = v;
                std::set<timing_vertex_t> temp;
                to_be_whited_on_callback.insert(std:: make_pair(v,temp));
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
            if(g[v].is_master && !flag)
            {
                to_be_whited_on_callback.at(actual_master).insert(v);
            }
            //else continue;
            boost::default_dfs_visitor::discover_vertex(v,g);
        }
    template<typename Vertex, typename Graph>
        void start_vertex(Vertex v, Graph const& g) 
        {
            PRINT_DEBUG("masters_task_setter_visitor: calling start_vertex "+ g[v].name );
            flag = true;
            boost::default_dfs_visitor::start_vertex(v,g);
        }
    template<typename Vertex, typename Graph>
        void finish_vertex(Vertex v, Graph const& g) {
            PRINT_DEBUG("calling finish_vertex"+ g[v].name);
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
            if (to_be_whited_on_callback.count(v) != 0)
            {
                for (std::set<timing_vertex_t>::iterator it = to_be_whited_on_callback.at(v).begin();it != to_be_whited_on_callback.at(v).end(); ++it)
                    vertex_coloring[*it] = boost::default_color_type::white_color ;
            }
            boost::default_dfs_visitor::finish_vertex(v,g);
        }
};

struct exploration_from_interferes_with_to_visitor :public boost::default_dfs_visitor{
  timing_vertex_t to; 
  std::map<std::string,timing_vertex_t>* reference_map;
  std:: map <timing_vertex_t, std::set<timing_vertex_t>> to_be_whited_on_callback;
  std::map <timing_vertex_t, timing_vertex_t> slave_master;
  timing_vertex_t curr_slave ;
  timing_vertex_t curr_OS = Timing_Graph::null_vertex();
  std::vector<timing_vertex_t> slaves_stack;
  Layer layer;
  exploration_from_interferes_with_to_visitor (timing_vertex_t init_to, std::map<std::string,timing_vertex_t>& references_map, Layer l)
  {
      to = init_to;
      reference_map = &references_map;
      layer = l;
      to_be_whited_on_callback = std:: map <timing_vertex_t, std::set<timing_vertex_t>> ();
      slave_master = std::map <timing_vertex_t, timing_vertex_t> ();
      slaves_stack = std::vector<timing_vertex_t>();
      curr_slave = Timing_Graph::null_vertex();
  }
  using colormap = boost::property_map<Timing_Graph, boost::vertex_color_t>::type;
    colormap vertex_coloring;

    template<typename Vertex, typename Graph>
        void discover_vertex(Vertex v, Graph const& g) {
            PRINT_DEBUG("exploration_from_interferes_with_to_visitor: calling discover_vertex "<< g[v].name );
            switch (g[v].layer)
            {
                case CONTROLLER:
                {
                    if (layer != CONTROLLER)
                    {
                        if (curr_OS == Timing_Graph::null_vertex()) //NO se sono gia dentro non blocca. devo spostarla sull edge. fatto. questo è il primo OS.
                        {
                            std::set<timing_vertex_t> tmp;
                            to_be_whited_on_callback.insert(std::make_pair(v,tmp));
                            curr_OS = v;
                        }
                    }
                    break;
                }
                case RESOURCE:
                {
                    if(g[v].is_master && slave_master.count(curr_slave) == 0)
                    {
                        //nodo da ignorare. devo pero scurire nodo associato.
                        if(g[v].associate_port_name != "")
                        {
                            vertex_coloring[reference_map->at(g[v].associate_port_name)] = boost::default_color_type::black_color; 
                            std::set<timing_vertex_t> tmp ;
                            tmp.insert(reference_map->at(g[v].associate_port_name));
                            to_be_whited_on_callback.insert(std::make_pair(v,tmp));
                            PRINT_DEBUG("exploration_from_interferes_with_to_visitor: locking "+ g[v].associate_port_name);
                        }
                        slave_master.insert(std::make_pair<>(curr_slave,v));
                        to_be_whited_on_callback.at(curr_slave).insert(v);
                    }
                    else if (!g[v].is_master)
                    {
                        //context switch:
                        PRINT_DEBUG("exploration_from_interferes_with_to_visitor: context switch at node: "+g[v].name);
                        slaves_stack.push_back(curr_slave);
                        curr_slave = v;
                        std::set<timing_vertex_t> tmp;
                        if(g[v].associate_port_name != "") //è una cosa che non sono sicuro serva, dato che dovrebbe essere scurita la slave port associata dall'altro lato della linea
                        {
                            vertex_coloring[reference_map->at(g[v].associate_port_name)] = boost::default_color_type::black_color;
                            tmp.insert(reference_map->at(g[v].associate_port_name));
                        }                        
                        to_be_whited_on_callback.insert(std::make_pair(curr_slave,tmp));
                        
                    }
                    else if (g[v].is_master && slave_master.count(curr_slave) != 0)
                    {
                        PRINT_DEBUG("exploration_from_interferes_with_to_visitor: controllo master tasks di "+ g[v].name +"passando dallo slave: "+ g[curr_slave].name + "cercando: "+g[to].name);
                        
                        if (g[v].master_tasks.count(g[to].name) != 0)
                            throw std::runtime_error ("trovato in master task");
                        to_be_whited_on_callback.at(curr_slave).insert(v); //sarebbe else xD
                    }
                    
                    break;
                }
                case PHYSICAL:
                {
                    std::set<timing_vertex_t> tmp;
                    to_be_whited_on_callback.insert(std::make_pair(v,tmp));
                    break;
                }
                case TASK:
                {
                    if (v == to)
                        throw std::runtime_error ("trovato stesso nodo");
                    break;
                }
                case FUNCTION:
                {
        #ifdef DEBUG
                    std::cout << "exploration_from_interferes_with_to_visitor: function node"<<std::endl;
                
        #endif
                    break;
                }
            }
            //else continue;
            boost::default_dfs_visitor::discover_vertex(v,g);
        }
        template<typename Edge, typename Graph>
        void examine_edge(Edge e, Graph const& g) 
        {
#ifdef DEBUG
            std::cout<<"exploration_from_interferes_with_to_visitor: examine edge "<< g[boost::source<>(e,g)].name << "-" <<g[boost::target<>(e,g)].name <<std::endl;
#endif          
            if(g[boost::source<>(e,g)].layer == PHYSICAL && g[boost::target<>(e,g)].layer == RESOURCE)
            {
                if (g[boost::target<>(e,g)].master_tasks.count(g[to].name) != 0)
                    throw std::runtime_error ("trovato in phys to resource edge");
                
                vertex_coloring[boost::target<>(e,g)] = boost::default_color_type::black_color;
                to_be_whited_on_callback.at(boost::source<>(e,g)).insert(boost::target<>(e,g));
                
            }
            else if (g[boost::target<>(e,g)].layer == CONTROLLER && curr_OS != Timing_Graph::null_vertex())
            {
                if (vertex_coloring[boost::target<>(e,g)] == boost::default_color_type::white_color)
                {
                    vertex_coloring[boost::target<>(e,g)] =  boost::default_color_type::black_color;
                    to_be_whited_on_callback.at(curr_OS).insert(boost::target<>(e,g));
                }
                
            }
            boost::default_dfs_visitor::examine_edge(e,g);
        }
    template<typename Vertex, typename Graph>
        void finish_vertex(Vertex v, Graph const& g) {
            PRINT_DEBUG("calling exploration_from_interferes_with_to_visitor finish vertex"<< g[v].name );
            if (to_be_whited_on_callback.count(v) != 0)
            {
                PRINT_DEBUG("exploration_from_interferes_with_to_visitor: finish vertex found the node inside the to_be_whited_on_callback map " +g[v].name );
                for (auto vtx: to_be_whited_on_callback.at(v))
                {
                    vertex_coloring[vtx] = boost::default_color_type::white_color;
                    PRINT_DEBUG("exploration_from_interferes_with_to_visitor: whited node " +g[vtx].name );
                }
                PRINT_DEBUG("exploration_from_interferes_with_to_visitor: size of the map before remove is: " +boost::lexical_cast<std::string>(to_be_whited_on_callback.size()));
                to_be_whited_on_callback.erase(v);
                PRINT_DEBUG("exploration_from_interferes_with_to_visitor: size of the map after remove is: " +boost::lexical_cast<std::string>(to_be_whited_on_callback.size()));
            }
            if (v == curr_slave)
            {
                PRINT_DEBUG("exploration_from_interferes_with_to_visitor: curr_slave found, stack size before is: " +boost::lexical_cast<std::string>(slaves_stack.size()));
                slaves_stack.pop_back();
                curr_slave = slaves_stack.back();
                PRINT_DEBUG("exploration_from_interferes_with_to_visitor: curr_slave found, stack size after is: " +boost::lexical_cast<std::string>(slaves_stack.size()));
            }
            if (v == curr_OS)
            {
                curr_OS = Timing_Graph::null_vertex();
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
