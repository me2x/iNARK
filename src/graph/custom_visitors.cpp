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


extern_visitor::extern_visitor(std::vector<vertex_t>& discovered,vertex_t& source_par): discovered_vertexes(&discovered),source(&source_par) {
    PRINT_DEBUG ("using extern visitor" );

}
void extern_visitor::initialize_vertex(const vertex_t s, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const {
  PRINT_DEBUG("init vertex: "+boost::lexical_cast<std::string>(s));
}
void extern_visitor::discover_vertex(const vertex_t s, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g)const {
  discovered_vertexes->push_back(s);
  PRINT_DEBUG("found vertex: "+boost::lexical_cast<std::string>(s));
}
void extern_visitor::examine_vertex(const vertex_t s, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const {
  PRINT_DEBUG("examine vertex: "+boost::lexical_cast<std::string>(s));
}
void extern_visitor::examine_edge(const edge_t e, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const { 
  { //non va bene xk essendo il grafo non diretto perdo collegamento tra porta e componente: l'edge infatti viene girato a piacimento mentre il to e il from restano gli stessi.
  PRINT_DEBUG("examining edge: start. edge is: "+boost::lexical_cast<std::string>(e)+"target node is: "+g[(boost::target(e,g))].name+
  " and the target name is: "+boost::lexical_cast<std::string>(g[e].to_port.component_name)+
  "and the target port is: "+boost::lexical_cast<std::string>(g[e].to_port.component_port)+
  + " source node is: "+g[(boost::source(e,g))].name+
  " and the source name is: "+boost::lexical_cast<std::string>(g[e].from_port.component_name)+
  "and the source port is: "+boost::lexical_cast<std::string>(g[e].from_port.component_port)
  
    
  
  );
/*  if (g[(boost::target(e,g))].type == PROCESSOR) return;
   if ((results_map_attr)->count(boost::target(e,g))==0)
   {
     PRINT_DEBUG("examining edge: step1");
     (results_map_attr)->insert(std::make_pair<vertex_t,Priority>(boost::target(e,g),g[(boost::target(e,g))].ports.at(g[e].to_port)));
     PRINT_DEBUG("examining edge: step2");
   }
   else if (((results_map_attr)->at(boost::target(e,g))) > g[(boost::target(e,g))].ports.at(g[e].to_port))
   {
     PRINT_DEBUG("examining edge: step3");
     ((results_map_attr)->at(boost::target(e,g))) = g[(boost::target(e,g))].ports.at(g[e].to_port);
     PRINT_DEBUG("examining edge: step4");
   }
  PRINT_DEBUG("examining edge: end");   
 */ 
}
}
void extern_visitor::edge_relaxed(const edge_t e, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const {
  PRINT_DEBUG("edge edge_relaxed");
}
void extern_visitor::edge_not_relaxed(const edge_t e, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const {
  PRINT_DEBUG("edge not edge_not_relaxed");
}
void extern_visitor::finish_vertex(const vertex_t s, const boost::filtered_graph<Graph, extern_edge_predicate_c,extern_vertex_predicate_c>  g) const {
  PRINT_DEBUG("finalize vvertex "+boost::lexical_cast<std::string>(s));
  if (s == *source) throw 0;
}

