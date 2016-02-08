#include "timing_internal_graph.hpp"
#include "custom_visitors.hpp"

timing_internal_graph::timing_internal_graph(Source_Graph g){
    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = vertices(g); vp.first != vp.second; ++vp.first)
    {
        PRINT_DEBUG("timing graph construction: vertex loop, layer is: "+boost::lexical_cast<std::string>(g[*vp.first].layer) +" and name is: "+boost::lexical_cast<std::string>(g[*vp.first].name));
        g[*vp.first].explode_component_timing(ig,components_map);
        
  
    }

    //da rifare. da risistemare i 4th lvl. non eliminare brutalmente ma collegarli alla "priorita" desiderata. conviene farlo qua? credo di si. 
    //risistemare la parte dopo: non mi servono piu i 3 to 4 edges, basta lasciare ogni os collegato al suo so. mantenere intra layer, solo spostarli su priorità desiderata.
    //per ogni edge, crea andata e ritorno nel bidirezionale. tranne che per gli intra layer e i link tra 4 e 5th livello. 3 to 4th lvl vanno creati dopo aver collassato
    //il 4th lvl guardando le priorita delle porte. 4 to 5 vanno mappati subito dopo il collasso a causa di possibile presenza di componenti mappati su stesso nodo collassato presenti su piu board
    std::ofstream myfile;
    myfile.open ("/home/emanuele/Documents/tmp_graph/aaafiltrato.dot");
    boost::write_graphviz(myfile, ig,make_vertex_writer(boost::get(&Timing_Node::layer, ig),boost::get (&Timing_Node::name, ig)));
    myfile.close();
    
    PRINT_DEBUG("components map size is: "+boost::lexical_cast<std::string>(components_map.size()));
    
    edge_iter ei, ei_end;

    for (boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei)
    {
        timing_edge_t e; bool b;
	vertex_t old_graph_source,old_graph_target;
	old_graph_source = boost::source(*ei,g);
	old_graph_target = boost::target(*ei,g);
        timing_vertex_t new_source, new_target;
	PRINT_DEBUG("the source component is: "+g[old_graph_source].name+" and its port is: "+boost::lexical_cast<std::string>(g[*ei].from_port));
        PRINT_DEBUG("the target component is: "+g[old_graph_target].name+" and its port is: "+boost::lexical_cast<std::string>(g[*ei].to_port));
        //mancano 4 to 5: da moltiplicare.
        //all edges are in 1:1 between the physical graph to the internal representation, exept the edges from l4 to l5. 
        //for that layer i have to build one edge for each new resource to the physical component
        if (g[old_graph_source].layer != PHYSICAL && g[old_graph_target].layer != PHYSICAL)
        {
            if (components_map.count(g[old_graph_source].name) != 0)
            {
                new_source = get_node_reference(components_map.at(g[old_graph_source].name).at(g[*ei].from_port !=NO_PORT? g[*ei].from_port:1));
            }
            else
            {
                new_source = get_node_reference(g[old_graph_source].name);
            }
            if (components_map.count(g[old_graph_target].name) != 0)
            {
                new_target = get_node_reference(components_map.at(g[old_graph_target].name).at(g[*ei].to_port !=NO_PORT? g[*ei].to_port:1));
            }
            else
            {
                new_target = get_node_reference(g[old_graph_target].name);
            }
            
            boost::tie(e,b) = boost::add_edge(new_source,new_target,ig);
        }
        else //layer 4-5
        {
            
            bool l4_is_source = g[old_graph_source].layer == RESOURCE;
            for (std::map<int,std::string>::iterator l4_to_l5_iter = components_map.at(g[l4_is_source?old_graph_source:old_graph_target].name).begin();l4_to_l5_iter != components_map.at(g[l4_is_source?old_graph_source:old_graph_target].name).end();++l4_to_l5_iter)
            {
                PRINT_DEBUG("components map at: "+g[l4_is_source?old_graph_source:old_graph_target].name+ " size is: "+boost::lexical_cast<std::string>(components_map.at(g[l4_is_source?old_graph_source:old_graph_target].name).size()));
                new_source = get_node_reference(l4_is_source? (*l4_to_l5_iter).second:g[old_graph_source].name );
                new_target = get_node_reference(l4_is_source? g[old_graph_target].name : (*l4_to_l5_iter).second);
                boost::tie(e,b) = boost::add_edge(new_source,new_target,ig);
            }
        }
        
#if 0
        //fai con uno switch su somma layers. caso base: andata e ritorno, vale per 1
        //3: layer 2to3, priority?custom else caso base
        //5: 3 sdoppiato? 4 sdoppiato? custom.
        //6: layer 4 to 4 priority?custom else caso base
        //7: devo riprender i 4 to 4 sdoppiati e propagare. usa una mappa variabile interna.
        switch(g[source].layer+g[target].layer)
	{
	  case 1:
	  {
	      //boost::tie(e,b) = boost::add_edge(source,target,ig); sbagliatissimo. i vertex_t identifier sono diversi tra i due grafi ora!
	    boost::tie(e,b) = boost::add_edge(get_node_reference(g[source].name),get_node_reference(g[target].name),ig);
	    PRINT_DEBUG("function layer edge target: "+ig[boost::target(e,ig)].name);
	    boost::tie(e,b) = boost::add_edge(get_node_reference(g[target].name),get_node_reference(g[source].name),ig);
	    PRINT_DEBUG("function layer edge target: "+ig[boost::target(e,ig)].name);
	    break;
	  }
	  case 3:
	  {
	    vertex_t third_layer_vertex = ((g[source].layer == CONTROLLER)?source:target);
	    vertex_t other_vertex = ((g[source].layer == CONTROLLER)?target:source);
	    if(g[third_layer_vertex].priority_category == PRIORITY)
	    {
	      
	     boost::tie(e,b) = boost::add_edge(get_node_reference(g[third_layer_vertex].name+"$$"+boost::lexical_cast<std::string>(g[*ei].priority)),get_node_reference(g[other_vertex].name),ig);
	     boost::tie(e,b) = boost::add_edge(get_node_reference(g[other_vertex].name),get_node_reference(g[third_layer_vertex].name+"$$"+boost::lexical_cast<std::string>(g[*ei].priority)),ig);
	    }
	    else
	    {
	    boost::tie(e,b) = boost::add_edge(get_node_reference(g[third_layer_vertex].name),get_node_reference(g[other_vertex].name),ig);
	    boost::tie(e,b) = boost::add_edge(get_node_reference(g[other_vertex].name),get_node_reference(g[third_layer_vertex].name),ig);
	    }
	    break;
	  }
	  case 5:
	  {
	    vertex_t third_layer_vertex = ((g[source].layer == CONTROLLER)?source:target);
	    vertex_t other_vertex = ((g[source].layer == CONTROLLER)?target:source);
	    //impossible to have an OS mapped on something different than a processor. the processor cannot be splitted in more components. so the 4th layer item is never splitted here.
	    if(g[third_layer_vertex].priority_category == PRIORITY )
	    {
	      	    //the third is: TODO add third layer links with all priorities
	      for(int i = 0; i < PRIORITY_ENUM_SIZE; i++)
	      {
		boost::tie(e,b) = boost::add_edge(get_node_reference(g[third_layer_vertex].name+"$$"+boost::lexical_cast<std::string>(i)),get_node_reference(g[other_vertex].name),ig);
		boost::tie(e,b) = boost::add_edge(get_node_reference(g[other_vertex].name),get_node_reference(g[third_layer_vertex].name+"$$"+boost::lexical_cast<std::string>(i)),ig);
	      }
	    }
	    else
	    {
	    boost::tie(e,b) = boost::add_edge(get_node_reference(g[third_layer_vertex].name),get_node_reference(g[other_vertex].name),ig);
	    boost::tie(e,b) = boost::add_edge(get_node_reference(g[other_vertex].name),get_node_reference(g[third_layer_vertex].name),ig);
	    }
	    break;
	  }
	  case 6://4th to 4th, splittati qua.
	  {
	    PRINT_DEBUG("case 6");
	    if (g[source].priority_category==PRIORITY && g[target].priority_category == PRIORITY)
	    {
	      //sdoppia tutto usare source e.port[id] 
	      PRINT_DEBUG("case 6, both priority");
	      PRINT_DEBUG("case 6,source component port priority is: "+boost::lexical_cast<std::string>(g[source].ports.at(g[*ei].from_port.component_port)));
	      //PRINT_DEBUG("target component port priority is: "+boost::lexical_cast<std::string>(g[target].ports.at(g[*ei].to_port.component_port)));
	      boost::tie(e,b) = boost::add_edge(lvl_4_tracer.at(source).at(g[source].ports.at(g[*ei].from_port.component_port)),lvl_4_tracer.at(target).at(g[target].ports.at(g[*ei].to_port.component_port)),ig);
	      boost::tie(e,b) = boost::add_edge(lvl_4_tracer.at(target).at(g[target].ports.at(g[*ei].to_port.component_port)),lvl_4_tracer.at(source).at(g[source].ports.at(g[*ei].from_port.component_port)),ig);
    }
	    else if (g[source].priority_category!=PRIORITY && g[target].priority_category == PRIORITY)
	    {
	      PRINT_DEBUG("case 6, second priority. target component is: "+g[target].name);
	    //  PRINT_DEBUG("case 6,source component port priority is: "+boost::lexical_cast<std::string>(/*g[source].ports.at(*/g[*ei].to_port.component_port));
	     PRINT_DEBUG("case 6, target component port priority is: "+boost::lexical_cast<std::string>(g[target].ports.at(g[*ei].to_port.component_port)));
	     PRINT_DEBUG("case 6, target graph in internal graph is"+ig[lvl_4_tracer.at(target).at(g[target].ports.at(g[*ei].to_port.component_port))].name);
	     boost::tie(e,b) = boost::add_edge(get_node_reference(g[source].name),lvl_4_tracer.at(target).at(g[target].ports.at(g[*ei].to_port.component_port)),ig);
	     if(g[source].priority_category!= TDMA)
	       boost::tie(e,b) = boost::add_edge(lvl_4_tracer.at(target).at(g[target].ports.at(g[*ei].to_port.component_port)),get_node_reference(g[source].name),ig);
	
	    }
	    else if(g[source].priority_category==PRIORITY && g[target].priority_category != PRIORITY)
	    {
	      PRINT_DEBUG("case 6, first priority. source component is: "+g[source].name);
	     // PRINT_DEBUG("case 6,source component port priority is: "+boost::lexical_cast<std::string>(/*g[source].ports.at(*/g[*ei].from_port.component_port));
	      PRINT_DEBUG("case 6,source component port priority is: "+boost::lexical_cast<std::string>(g[source].ports.at(g[*ei].from_port.component_port)));
	      PRINT_DEBUG("case 6, source graph in internal graph is"+ig[lvl_4_tracer.at(source).at(g[source].ports.at(g[*ei].from_port.component_port))].name);
	      boost::tie(e,b) = boost::add_edge(lvl_4_tracer.at(source).at(g[source].ports.at(g[*ei].from_port.component_port)),get_node_reference(g[target].name),ig);
	      if(g[target].priority_category!= TDMA)
		boost::tie(e,b) = boost::add_edge(get_node_reference(g[target].name),lvl_4_tracer.at(source).at(g[source].ports.at(g[*ei].from_port.component_port)),ig);
	    }
	    else
	    {
	      PRINT_DEBUG("case 6, both no priority");
	      boost::tie(e,b) = boost::add_edge(get_node_reference(g[source].name),get_node_reference(g[target].name),ig);
	      boost::tie(e,b) = boost::add_edge(get_node_reference(g[target].name),get_node_reference(g[source].name),ig);
	    }
	      break;
	  }
		
	  case 7: 
	  {
	    PRINT_DEBUG("case 7, start");
	    vertex_t fourth_layer_vertex = ((g[source].layer == RESOURCE)?source:target);
	    vertex_t other_vertex = ((g[source].layer == RESOURCE)?target:source);
	    auto iter = lvl_4_tracer.find(fourth_layer_vertex);
	    if (iter != lvl_4_tracer.end())
	    {
	      PRINT_DEBUG("case 7, found, the node is: "+g[fourth_layer_vertex].name);
	      PRINT_DEBUG("case 7, found, the other node is: "+g[other_vertex].name);
	      for (auto& a : (*iter).second)
	      {
		PRINT_DEBUG("case 7, iterating, internal node is: "+ig[a.second].name);
		boost::tie(e,b) = boost::add_edge(get_node_reference(g[other_vertex].name),get_node_reference(ig[a.second].name),ig);
		boost::tie(e,b) = boost::add_edge(get_node_reference(ig[a.second].name),get_node_reference(g[other_vertex].name),ig);
	      }
	    }
	    else
	    {
	      PRINT_DEBUG("case 7, else branch");
	      boost::tie(e,b) = boost::add_edge(get_node_reference(g[source].name),get_node_reference(g[target].name),ig);
	      boost::tie(e,b) = boost::add_edge(get_node_reference(g[target].name),get_node_reference(g[source].name),ig);
	    }
	   break; 
	  }
	  default:
	  {
	    throw std::runtime_error ("ERROR, rebuilding edges layers sum not valid");
	    break;
	  }
	
	
	
	
	

	}

     /*   if((g[boost::source(*ei,g)].layer== TASK && g[boost::target(*ei,g)].layer == CONTROLLER) || (g[boost::source(*ei,g)].layer!= CONTROLLER && g[boost::target(*ei,g)].layer != TASK))
        {
	  
            boost::tie(e,b) = boost::add_edge(boost::source(*ei,g),boost::target(*ei,g),ig);
            ig[e].priority= g[*ei].priority;
	    PRINT_DEBUG(boost::target(e,ig));

            boost::tie(e,b) = boost::add_edge(boost::target(*ei,g),boost::source(*ei,g),ig);
            ig[e].priority= g[*ei].priority;
            PRINT_DEBUG(boost::target(e,ig));
	    if(g[boost::source(*ei,g)].layer== CONTROLLER && g[boost::target(*ei,g)].layer == RESOURCE)
	     {
	       lvl_4_to_3_map.insert(std::make_pair(g[boost::target(*ei,g)].name,g[boost::source(*ei,g)].name));
	     }
        }
       */
#endif 	
    }
 

    std::ofstream myfile2;
    myfile2.open ("/home/emanuele/Documents/tmp_graph/aaaedged.dot");
    boost::write_graphviz(myfile2, ig,make_vertex_writer(boost::get(&Timing_Node::layer, ig),boost::get (&Timing_Node::name, ig)));
    myfile2.close();  
      
    
  
}
timing_vertex_t timing_internal_graph::get_node_reference(std::string str)
{

  timing_vertex_iter vi, vi_end;
  boost::tie(vi, vi_end) = boost::vertices(ig);
  for (; vi != vi_end; ++vi) 
  {
    if (ig[*vi].name == str)
      return *vi;
  }
  return NULL;
}
bool timing_internal_graph::search_path(std::string from, std::string to, Layer l)
{
    #if 0
  vertex_t source_os;
  if (l != CONTROLLER)
  {
    boost::graph_traits<Timing_Graph>::out_edge_iterator edges_out, edges_out_end;
    boost::tie (edges_out,edges_out_end) = boost::out_edges(get_node_reference(from),ig);
    for(;edges_out != edges_out_end; ++edges_out)
    {
      PRINT_DEBUG("considered edge is: ("+ig[boost::source(*edges_out,ig)].name+" , "+ig[boost::target(*edges_out,ig)].name+")");
      if (ig[boost::target(*edges_out,ig)].layer == CONTROLLER)
      {
	PRINT_DEBUG("if condition is true" );
	source_os = boost::target(*edges_out,ig);
	break;
      }
    }
    PRINT_DEBUG ("starting search. source OS is: "+ig[source_os].name);
  }
  else
  {
    source_os=0;
    PRINT_DEBUG ("starting search. else branch source OS is: "+ig[source_os].name);
  }
  std::vector<vertex_t> path;
  boost::filtered_graph<Timing_Graph,inner_edge_predicate_c,inner_vertex_predicate_c> ifg (ig,inner_edge_predicate_c(ig,l,get_node_reference(to))/*doesnt really matter. can be deleted*/,inner_vertex_predicate_c(ig,l,ig[source_os].name));
      source_to_target_visitor vis = source_to_target_visitor(path,get_node_reference(to));
          
//#if 0
  std::ofstream myfile;
  myfile.open ("/home/emanuele/Documents/tmp_graph/aaafiltrato.dot");
  boost::write_graphviz(myfile, ifg,make_vertex_writer(boost::get(&Custom_Vertex::layer, ifg),boost::get (&Custom_Vertex::name, ifg),boost::get(&Custom_Vertex::ports, ifg), boost::get(&Custom_Vertex::type,ifg ), boost::get(&Custom_Vertex::priority_category,ifg))
      ,/*make_edge_writer(boost::get(&Custom_Edge::priority,ig),boost::get(&Custom_Edge::from_port,ig),boost::get(&Custom_Edge::to_port,ig))*/
      boost::make_label_writer(boost::get(&Custom_Edge::priority,ig)));
  myfile.close();
//#endif
     
    try {
      boost::depth_first_search(
        ifg, boost::root_vertex(get_node_reference(from)).visitor(vis)
      );
    }
    catch (int exception) {
      if (exception == 3) 
      {
	PRINT_DEBUG ("SEARCH: path found, and is:");
	for (vertex_t v : path)
	{
	  PRINT_DEBUG(ig[v].name); //TODO ricompattare gli "esplosi" se ne vale la pena. senti il capo.
	}
	return true;
      }
      else if (exception == 2) 
      {
	PRINT_DEBUG ("SEARCH: restarting, path not foundd");
	return false;
      }
    }
#endif    
return false;
}
#if 0
//posso usarla per entrambe le ricerche. reverse "decide" se grafo dritto (ovvero this node interferes with) oppure al contrario (this node is interfered by)
void timing_internal_graph::search_interfered_nodes (std::string source, bool reverse)
{
  PRINT_DEBUG("interfered node search: start");
  vertex_t source_os;
  boost::graph_traits<Timing_Graph>::out_edge_iterator edges_out, edges_out_end;
  boost::tie (edges_out,edges_out_end) = boost::out_edges(get_node_reference(source),ig);
  for(;edges_out != edges_out_end; ++edges_out)
  {
    PRINT_DEBUG("interfered node search considered edge is: ("+ig[boost::source(*edges_out,ig)].name+" , "+ig[boost::target(*edges_out,ig)].name+")");
    if (ig[boost::target(*edges_out,ig)].layer == CONTROLLER)
    {
    PRINT_DEBUG("interfered node search if condition is true" );
    source_os = boost::target(*edges_out,ig);
    break;
    }
  }
  PRINT_DEBUG ("interfered node search starting search. source OS is: "+ig[source_os].name);
  std::vector<vertex_t> controller_reached_tasks;
  std::vector<vertex_t> components_reached_tasks;
  std::vector<vertex_t> physical_reached_tasks;
  //Timing_Graph target_graph = reverse ?boost::make_reverse_graph(ig) :ig;
  boost::filtered_graph<Timing_Graph,inner_edge_predicate_c,inner_vertex_predicate_c> ifg (target_graph,inner_edge_predicate_c(target_graph,LAYER_ERROR,get_node_reference(0))/*doesnt really matter. can be deleted*/,inner_vertex_predicate_c(target_graph,l,ig[source_os].name));
  interference_visitor vis_con = interference_visitor(controller_reached_tasks);
  interference_visitor vis_com = interference_visitor(components_reached_tasks);
  interference_visitor vis_phy = interference_visitor(physical_reached_tasks);
  try {
      boost::depth_first_search(
        ifg, boost::root_vertex(get_node_reference(source)).visitor(vis_con)
      );
    }
    catch (int exception) {
      if (exception == 2) 
      {
	PRINT_DEBUG ("SEARCH: restarting, path not foundd");
	for (vertex_t v : controller_reached_tasks)
	{
	  PRINT_DEBUG(target_graph[v].name); //TODO ricompattare gli "esplosi" se ne vale la pena. senti il capo.
	}
      }
    }
    try {
      boost::depth_first_search(
        ifg, boost::root_vertex(get_node_reference(source)).visitor(vis_com)
      );
    }
    catch (int exception) {
      if (exception == 2) 
      {
	PRINT_DEBUG ("SEARCH: restarting, path not foundd");
	for (vertex_t v : components_reached_tasks)
	{
	  PRINT_DEBUG(target_graph[v].name); //TODO ricompattare gli "esplosi" se ne vale la pena. senti il capo.
	}
      }
    }
    try {
      boost::depth_first_search(
        ifg, boost::root_vertex(get_node_reference(source)).visitor(vis_phy)
      );
    }
    catch (int exception) {
      if (exception == 2) 
      {
	PRINT_DEBUG ("SEARCH: restarting, path not foundd");
	for (vertex_t v : physical_reached_tasks)
	{
	  PRINT_DEBUG(target_graph[v].name); //TODO ricompattare gli "esplosi" se ne vale la pena. senti il capo.
	}
      }
    }

}
#endif