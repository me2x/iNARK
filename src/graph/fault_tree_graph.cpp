#include "fault_tree_graph.h"


fault_tree_graph::fault_tree_graph()
{
    ig = FT_Graph();
}
//as it is now, no modification are required. only the copy of every node. the class has been created to divide from other specialized views and to support in the future further work on the integration of FTA.
void fault_tree_graph::build_graph(std::shared_ptr< Source_Graph > g)
{
    ig.clear();
    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = boost::vertices(*g); vp.first != vp.second; ++vp.first)
    {
        PRINT_DEBUG("timing graph construction: vertex loop, layer is: "+boost::lexical_cast<std::string>((*g)[*vp.first].get_layer()) +" and name is: "+boost::lexical_cast<std::string>((*g)[*vp.first].get_name()));
        (*g)[*vp.first].explode_component_FTA(ig);
    }
 
    edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(*g); ei != ei_end; ++ei)
    {
         ft_edge_t e; bool b;
         //vertex_t oldsource = boost::source(*ei, *g);
         ft_vertex_t new_source = get_node_reference((*g)[boost::source(*ei,*g)].get_name());
         ft_vertex_t new_target = get_node_reference((*g)[boost::target(*ei,*g)].get_name());
         boost::tie(e,b) = boost::add_edge(new_source,new_target,ig);
    }
    
    
    std::ofstream myfile;
    myfile.open ("/home/emanuele/Documents/tmp_graph/FTsource.dot");
    boost::write_graphviz(myfile, ig,make_vertex_writer(boost::get(&FT_Node::layer, ig),boost::get (&FT_Node::name, ig)));
    myfile.close();  
   
    
}

void fault_tree_graph::print_FTAs()
{
    std::set<ft_vertex_t> tasks;
    std::pair< ft_vertex_iter,ft_vertex_iter> ft_vtxs;
    for (ft_vtxs = boost::vertices(*g); ft_vtxs.first != ft_vtxs.second; ++ft_vtxs.first)
        if (ig[*ft_vtxs.first].layer == Layer::TASK)
            tasks.insert(*ft_vtxs.first);
        
    //loop:     
        //visit
        
        
        //filter
        
        
        
        
        //print
    
}








ft_vertex_t fault_tree_graph::get_node_reference(std::string str)
{

  ft_vertex_iter vi, vi_end;
  boost::tie(vi, vi_end) = boost::vertices(ig);
  for (; vi != vi_end; ++vi) 
  {
    if (ig[*vi].name == str)
      return *vi;
  }
  throw std::runtime_error ("node "+str+" does not exist in the graph");
  return FT_Graph::null_vertex();
}