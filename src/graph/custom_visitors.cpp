#include "custom_visitors.hpp"



bool lv4_vertex_predicate_c::operator()(const vertex_t& vertex_id) const
{
    return (*graph_m)[vertex_id].layer == Layer::RESOURCE ;
}

bool true_edge_predicate::operator()(const edge_t& edge_id) const
{
    return true; 
}


bool layer_filter_vertex_predicate_c::operator()(const vertex_t& vertex_id) const
{
    return (*graph_m)[vertex_id].layer <= layer ;
}




#if 0
inner_vertex_predicate_c::inner_vertex_predicate_c(Timing_Graph& ig, Layer threshold_input, std::string source_os)
{
  graph_m = &ig; 
  threshold = threshold_input;
  std::string match = source_os;
  
  PRINT_DEBUG("matching: match is" + match + " and layer is" + boost::lexical_cast<std::string>(threshold));
  std::string delimiter = "$$";
  match_name = match.substr(0, match.find(delimiter));
  match.erase(0, match_name.length()+delimiter.length());
  match_priority = match;
  PRINT_DEBUG("matching: name is: "+match_name);
  PRINT_DEBUG("matching: priority is: "+match_priority);
  PRINT_DEBUG("matching: match strings built");
}
//in verita non serve. per l'idea nuova di grafo basta tagliare i vertici. resta piu complessa la costruzione.
bool inner_edge_predicate_c::operator()(const inner_edge_t& edge_id) const{
      //Priority type = (*graph_m)[edge_id].priority;
      //inedge recupera entranti nel target, recupera vertici che è porta di un os. una volta fatto questo su ogni edge if dest == os && layer == 2 then cut else keep.
      return (1);//(*graph_m)[boost::source(edge_id,(*graph_m))].layer< (*graph_m)[boost::target(edge_id,(*graph_m))].layer && type >= threshold )|| (*graph_m)[boost::source(edge_id,(*graph_m))].layer > (*graph_m)[boost::target(edge_id,(*graph_m))].layer;
    }

bool inner_vertex_predicate_c::operator()(const vertex_t& vertex_id) const{
    Layer layer = (*graph_m)[vertex_id].layer;
    std::string match = (*graph_m)[vertex_id].name;
  
    PRINT_DEBUG("executing inner vertex predicate: match is" + match + " and layer is" + boost::lexical_cast<std::string>(threshold));
    std::string delimiter = "$$";
    std::string component_match_name = match.substr(0, match.find(delimiter));
    match.erase(0, match_name.length()+delimiter.length());
    std::string component_match_priority = match;
    PRINT_DEBUG("executing inner vertex predicate: name is: "+component_match_name);
    PRINT_DEBUG("executing inner vertex predicate: priority is: "+component_match_priority);
    PRINT_DEBUG("executing inner vertex predicate: match strings built");
    return (layer <= threshold && !(threshold >= CONTROLLER && component_match_name == match_name && component_match_priority != match_priority));
}
source_to_target_visitor::source_to_target_visitor(std::vector<vertex_t>& discovered,vertex_t destination_vertex_l): path_vertexes(&discovered), destination_vertex_m(destination_vertex_l) {
    PRINT_DEBUG("prova");
    PRINT_DEBUG (destination_vertex_m);
    start_flag = true;
}
void source_to_target_visitor::initialize_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
      PRINT_DEBUG("exploring, initialize vertex: "<<g[s].name);

}
void source_to_target_visitor::start_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
      PRINT_DEBUG("exploring, start vertex: "<<g[s].name);
      if (start_flag == false) 
      {
	PRINT_DEBUG("exploring, start flag is ddfalse");
	throw (2);
      }
      else
      {
	PRINT_DEBUG("exploring, start flag is true");
      }
      start_flag = false;
      
}
void source_to_target_visitor::discover_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const 
{
      PRINT_DEBUG("exploring, found vertex: "<<g[s].name);
      path_vertexes->push_back(s);
    if (destination_vertex_m == s)
      throw(3);
}
void source_to_target_visitor::examine_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
      PRINT_DEBUG("exploring, examining edge: "<<g[boost::source<>(e,g)].name << "-" <<g[boost::target<>(e,g)].name);

}
void source_to_target_visitor::tree_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
  PRINT_DEBUG("exploring, tree edge: "<<g[boost::source<>(e,g)].name << "-" <<g[boost::target<>(e,g)].name);
}
void source_to_target_visitor::back_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
  PRINT_DEBUG("exploring, back edge: "<<g[boost::source<>(e,g)].name << "-" <<g[boost::target<>(e,g)].name);
}
void source_to_target_visitor::forward_or_cross_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
  PRINT_DEBUG("exploring, forward_or_cross edge: "<<g[boost::source<>(e,g)].name << "-" <<g[boost::target<>(e,g)].name);
}
void source_to_target_visitor::finish_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
  PRINT_DEBUG("exploring, finalize vertex: "<<g[s].name);
  path_vertexes->pop_back();
}



interference_visitor::interference_visitor(std::vector<vertex_t>& discovered): discovered_vertexes(&discovered) {
    PRINT_DEBUG("prova");
    start_flag = true;
}
void interference_visitor::initialize_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
      PRINT_DEBUG("exploring, initialize vertex: "<<g[s].name);

}
void interference_visitor::start_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
      PRINT_DEBUG("exploring, start vertex: "<<g[s].name);
      if (start_flag == false) 
      {
	PRINT_DEBUG("exploring, start flag is false");
	throw (2);
      }
      else
      {
	PRINT_DEBUG("exploring, start flag is true");
      }
      start_flag = false;
      
}
void interference_visitor::discover_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const 
{
    PRINT_DEBUG("exploring, found vertex: "<<g[s].name);
    if (g[s].layer == TASK)
      discovered_vertexes->push_back(s);
}
void interference_visitor::examine_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
      PRINT_DEBUG("exploring, examining edge: "<<g[boost::source<>(e,g)].name << "-" <<g[boost::target<>(e,g)].name);

}
void interference_visitor::tree_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
  PRINT_DEBUG("exploring, tree edge: "<<g[boost::source<>(e,g)].name << "-" <<g[boost::target<>(e,g)].name);
}
void interference_visitor::back_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
  PRINT_DEBUG("exploring, back edge: "<<g[boost::source<>(e,g)].name << "-" <<g[boost::target<>(e,g)].name);
}
void interference_visitor::forward_or_cross_edge(const inner_edge_t e, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
  PRINT_DEBUG("exploring, forward_or_cross edge: "<<g[boost::source<>(e,g)].name << "-" <<g[boost::target<>(e,g)].name);
}
void interference_visitor::finish_vertex(const vertex_t s, const boost::filtered_graph<Timing_Graph, inner_edge_predicate_c,inner_vertex_predicate_c>  g) const {
  PRINT_DEBUG("exploring, finalize vertex: "<<g[s].name);
}




bool extern_vertex_predicate_c::operator()(const vertex_t& vertex_id) const{
    Layer layer = (*graph_m)[vertex_id].layer;
    return (layer == RESOURCE);
}

//se layer from o layer to è diverso da 4, buttalo. da rifare
bool extern_edge_predicate_c::operator()(const inner_edge_t& edge_id) const{
    //Priority type = (*graph_m)[edge_id].priority;
    
    Layer l_src = (*graph_m)[boost::source(edge_id,*graph_m)].layer;
    Layer l_dst = (*graph_m)[boost::target(edge_id,*graph_m)].layer;
    return (l_src==l_dst);
}

#endif