#include "internal_graph.h"
#include "custom_visitors.h"

internal_graph::internal_graph(const Graph& g){
    //collassa grafo
    
    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = vertices(g); vp.first != vp.second; ++vp.first)
    {
        vertex_t vt = boost::add_vertex(ig);
        ig[vt].layer=g[*vp.first].layer;
        ig[vt].name=g[*vp.first].name;
	ig[vt].ports=g[*vp.first].ports;
	ig[vt].type=g[*vp.first].type;
	ig[vt].priority_category=g[*vp.first].priority_category;
        PRINT_DEBUG(ig[vt].name);

    }

    //per ogni edge, crea andata e ritorno nel bidirezionale. tranne che per gli intra layer e i link tra 4 e 5th livello. 3 to 4th lvl vanno creati dopo aver collassato
    //il 4th lvl guardando le priorita delle porte. 4 to 5 vanno mappati subito dopo il collasso a causa di possibile presenza di componenti mappati su stesso nodo collassato presenti su piu board
    edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei)
    {
        inner_edge_t e; bool b;
        if(g[boost::source(*ei,g)].layer!=g[boost::target(*ei,g)].layer && g[boost::source(*ei,g)].layer!= PHYSICAL && g[boost::target(*ei,g)].layer != PHYSICAL)
        {
            boost::tie(e,b) = boost::add_edge(boost::source(*ei,g),boost::target(*ei,g),ig);
            ig[e].priority= g[*ei].priority;
	    PRINT_DEBUG(boost::target(e,ig));

            boost::tie(e,b) = boost::add_edge(boost::target(*ei,g),boost::source(*ei,g),ig);
            ig[e].priority= g[*ei].priority;
            PRINT_DEBUG(boost::target(e,ig));
	    if(g[boost::source(*ei,g)].layer== CONTROLLER && g[boost::target(*ei,g)].layer == RESOURCE)
	     {
	       lvl_4_to_3_map.insert(std::make_pair<std::string,std::string>(g[boost::target(*ei,g)].name,g[boost::source(*ei,g)].name));
	     }
        }
       
	
    }

    //filtra grafo esterno: tieni solo 4 livello e edge interni.
    //itera su vertici. se lay ==4 && edges.count == 1 then collassa.
    

#ifdef DEBUG
    const char* name = "0123456789abcdefghilmnopqrstuvz";
#endif
    Graph fg2; 
 
    boost::copy_graph(g, fg2);
    boost::filtered_graph<Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> fg(fg2,extern_edge_predicate_c(fg2),extern_vertex_predicate_c(fg2));
       PRINT_DEBUG("fg edges number is: ");
#ifdef DEBUG
       boost::print_edges(fg,name);
#endif
  
    bool flag_for_collapse = true;
    std::map <std::string, std::vector<std::string> > collapse_maps;
    // init collapse maps with all the lay4 to lay5 edges
    vertex_iter vi_init, vi_end_init;
    boost::tie(vi_init, vi_end_init) = boost::vertices(fg2);
    for (; vi_init != vi_end_init; ++vi_init) 
    {
      if (fg2[*vi_init].layer == RESOURCE)
      {
	collapse_maps.insert(std::make_pair<std::string,std::vector<std::string> >(fg2[*vi_init].name,std::vector<std::string>()));
	//out edges
	ext_out_edge_iter ext_e,ext_e_end;
	boost::tie(ext_e,ext_e_end)=boost::out_edges(*vi_init, fg2);
	for (; ext_e != ext_e_end; ++ext_e)
	{
	  vertex_t source, dest;
	  source = boost::source(*ext_e, fg2);
	  dest = boost::target(*ext_e, fg2);
	  vertex_t tmp_vtx = (source == *vi_init)?dest:source;
	  if (fg2[tmp_vtx].layer == PHYSICAL)
	  (collapse_maps.at(fg2[*vi_init].name)).push_back(fg2[tmp_vtx].name);
	}
      }
    }
    
    
    //collapse vertices and update the map.
    while(flag_for_collapse)
    {
      flag_for_collapse = false;
      boost::graph_traits<boost::filtered_graph<Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> >::vertex_iterator vi, vi_end,next;
      boost::tie (vi,vi_end) = boost::vertices(fg);
      for (next = vi; vi != vi_end; vi = next) //iteratori invalidati. non posso tenere valore iteratore come chiave mappe!
      {
	++next;
#ifdef DEBUG
	std::string str = "out edges size for node: "+ boost::lexical_cast<std::string>(*vi)+ " is: " + boost::lexical_cast<std::string>(boost::out_degree(*vi,fg));
	PRINT_DEBUG (str);
	str = "out edges size for node: "+ ((get_node_reference(fg[*vi].name)!= NULL) ?(boost::lexical_cast<std::string>( get_node_reference(fg[*vi].name))):"NULL") + " on the original graph is: " + boost::lexical_cast<std::string>(boost::out_degree(*vi,g));
	PRINT_DEBUG (str);
#endif	
	if(boost::out_degree(*vi,fg)==1 && fg[*vi].type !=PROCESSOR)
	{
	  PRINT_DEBUG("collassing component "+ boost::lexical_cast<std::string>(*vi));
	  vertex_t vt = get_node_reference(fg[*vi].name);
	  if (vt != NULL) //la seconda run non entra perchè non trova il nodo in get component refernce. 
	  {
	    //prendi edge
	    boost::graph_traits<boost::filtered_graph<Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> >::out_edge_iterator e,e_end;
	    vertex_t source, dest,supernode;
	    boost::tie(e,e_end)=boost::out_edges(*vi, fg);
	    for (; e != e_end; ++e)
	    {
	      source = boost::source(*e, fg);
	      dest = boost::target(*e, fg);
	      supernode = (source == *vi)?dest:source;
	    }
	    std::map <std::string, std::vector<std::string> >::iterator collapse_maps_iterator = collapse_maps.find(fg2[*vi].name);
	    std::vector<std::string> tmp;
	    if( collapse_maps_iterator != collapse_maps.end())
	    {
	      PRINT_DEBUG("found the iterator reference to vertex_t");
	       tmp = collapse_maps.at(fg2[*vi].name);
	       collapse_maps.erase(collapse_maps_iterator);
	    }
	    else
	    {
	      PRINT_DEBUG("ERROR node not found in collapse map");
	      throw std::runtime_error ("ERROR, node not found in collapse map");
	    }
	    //recupera altri edges
	    std::vector<std::string>::iterator vt_vect_iter,vt_vect_iter_end;
	    vt_vect_iter_end = tmp.end();
	    for (vt_vect_iter = tmp.begin(); vt_vect_iter!= vt_vect_iter_end; ++vt_vect_iter)
	    {
	      PRINT_DEBUG("check for equality, supernode is: "+fg2[supernode].name+" while the iterator value is: "+*vt_vect_iter);
	      if (std::find(collapse_maps.at(fg2[supernode].name).begin(),collapse_maps.at(fg2[supernode].name).end(),*vt_vect_iter)==collapse_maps.at(fg2[supernode].name).end())
	      {
		PRINT_DEBUG("check passed, adding the node");
		collapse_maps.at(fg2[supernode].name).push_back(*vt_vect_iter);
	      }
	    }
	    
	    //spostali
	    //piu che clear_vertex, check if some remains
	    
	    boost::clear_vertex(vt,ig);
	    boost::remove_vertex(vt,ig);
	     vertex_t aaaaa = *vi;
	    boost::clear_vertex(aaaaa,fg2);
	    boost::remove_vertex(aaaaa,fg2);
	    flag_for_collapse = true;
	    break;
	    
	  }
	  
	}
	PRINT_DEBUG("-- ++ --");
	PRINT_DEBUG(boost::lexical_cast<std::string>(collapse_maps.size()));  
	
      }
#ifdef DEBUG
      std::map <std::string, std::vector<std::string> >::iterator debug_begin, debug_end;
      debug_end = collapse_maps.end();
      for (debug_begin = collapse_maps.begin(); debug_begin != debug_end; ++debug_begin)
      {
	std::vector<std::string>::iterator debug_vect_iter_begin, debug_vect_iter_end;
	debug_vect_iter_end=(*debug_begin).second.end();
	std::cout<< "for node: "<<(*debug_begin).first <<std::endl;
	for (debug_vect_iter_begin = (*debug_begin).second.begin(); debug_vect_iter_begin!=debug_vect_iter_end; ++debug_vect_iter_begin)
	  std::cout <<"debug collapse map print: "<< *debug_vect_iter_begin << std::endl;
	std::cout<< "-------" <<std::endl;
      }
#endif
    }
    
    //build layer 4 to 5 connections with the collapse maps
    std::map <std::string, std::vector<std::string> >::iterator connection_building_iter,connection_building_iter_end;
    connection_building_iter_end=collapse_maps.end();
    for (connection_building_iter = collapse_maps.begin(); connection_building_iter != connection_building_iter_end; ++connection_building_iter)
    {
      vertex_t vt = get_node_reference((*connection_building_iter).first);
	  if (vt != NULL) //la seconda run non entra perchè non trova il nodo in get component refernce. 
	  {
	    PRINT_DEBUG("  vt   ------"+ig[vt].name);
	    std::vector<std::string>::iterator vect_iter_begin, vect_iter_end;
	    vect_iter_end=(*connection_building_iter).second.end();
	    for (vect_iter_begin = (*connection_building_iter).second.begin(); vect_iter_begin!=vect_iter_end; ++vect_iter_begin)
	    {
	      vertex_t vt2 = get_node_reference((*vect_iter_begin));
	      PRINT_DEBUG("  vt2 ++++   ------"+ig[vt2].name);
	      inner_edge_t e; bool b;
              boost::tie(e,b) = boost::add_edge(vt,vt2,ig);
	      ig[e].priority = SAFETY_CRITICAL;  //not sure though. 
	      boost::tie(e,b) = boost::add_edge(vt2,vt,ig);
	      ig[e].priority = SAFETY_CRITICAL;
	    }
	  }
	  else
	  {
	    PRINT_DEBUG("ERROR: node not found when building ");
	    throw std::runtime_error(boost::lexical_cast<std::string>("ERROR: node not found when building "));
	  }
    }
    
    //build lay 3 to 4 connections with the ports algorithm che hai pensato qualche giorno fa e gia scordato :D
    //for every processing unit (type = PROCESSOR)
    //create a new graph that is a copy of fg2.
    //remove all other PU
    //do the search and report the results on the new graph
    boost::graph_traits<boost::filtered_graph<Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> >::vertex_iterator vi, vi_end,next;
    boost::tie(vi, vi_end) = boost::vertices(fg);
    for (next = vi; vi != vi_end; vi = next) 
      {
	PRINT_DEBUG("starting edge 3-4 loop build");
      	++next;
      if(fg[*vi].type == PROCESSOR)  //do the following (the real research) only for all the processing units in the system.
      {
	PRINT_DEBUG("processing unit identified");
	Graph tmp_graph_copy_for_research;
	boost::copy_graph(fg,tmp_graph_copy_for_research);
	vertex_iter tmp_iter, tmp_iter_end, tmp_iter_next;
	boost::tie(tmp_iter,tmp_iter_end) = boost::vertices(tmp_graph_copy_for_research);
	for(tmp_iter_next=tmp_iter; tmp_iter!=tmp_iter_end; tmp_iter=tmp_iter_next)
	{
	  PRINT_DEBUG("inside the remove processors loop");
	  ++tmp_iter_next;
	  if(tmp_graph_copy_for_research[*tmp_iter].type == PROCESSOR && tmp_graph_copy_for_research[*tmp_iter].name != fg[*vi].name) //remove all OTHER processing units
	  {
	    vertex_t aaaaa = *tmp_iter;
	    boost::clear_vertex(aaaaa,tmp_graph_copy_for_research);
	    boost::remove_vertex(aaaaa,tmp_graph_copy_for_research);
	    PRINT_DEBUG("removed");
	  }
	}

	std::map<vertex_t,Priority> string_results_map;
	//already iterating on the PROCESSOR. iterate on remaining vertices
	boost::tie(tmp_iter,tmp_iter_end) = boost::vertices(tmp_graph_copy_for_research);
	for(tmp_iter_next=tmp_iter; tmp_iter!=tmp_iter_end; tmp_iter=tmp_iter_next)
	{
	  ++tmp_iter_next;
	  PRINT_DEBUG("RESULT MAP BUILDING: iterate on vertexes, the vertex is:"+boost::lexical_cast<std::string>(*tmp_iter));
	  if(tmp_graph_copy_for_research[*tmp_iter].type == PROCESSOR) continue;
	  std::map<vertex_t,Priority> results_map;
	  Graph inner_copy; 
	  boost::copy_graph(tmp_graph_copy_for_research,inner_copy);
	  

	
	  edge_iter tmp_edge_iter, tmp_edge_iter_end;
	  boost::tie(tmp_edge_iter, tmp_edge_iter_end) = boost::edges(inner_copy);
	  for(; tmp_edge_iter!=tmp_edge_iter_end; ++tmp_edge_iter)
	  {
	    if(inner_copy[boost::source(*tmp_edge_iter,inner_copy)].name == tmp_graph_copy_for_research[*tmp_iter].name)//removed vtx tmp iter pero è di esterno. 
	    {
	      PRINT_DEBUG("RESULT MAP BUILDING: searching adjacent nodes, is source. dest is: "+ inner_copy[boost::target(*tmp_edge_iter,inner_copy)].name+" and the port is: "+boost::lexical_cast<std::string>(inner_copy[*tmp_edge_iter].from_port.component_port)); //identify adjacent nodes.
	      results_map.insert(std::make_pair<vertex_t,Priority>(boost::target(*tmp_edge_iter,inner_copy),inner_copy[boost::source(*tmp_edge_iter,inner_copy)].ports.at(inner_copy[*tmp_edge_iter].from_port.component_port)));
	    }
	    else if(inner_copy[boost::target(*tmp_edge_iter,inner_copy)].name == tmp_graph_copy_for_research[*tmp_iter].name)
	    {
	      PRINT_DEBUG("RESULT MAP BUILDING: searching adjacent nodes, is target. source is: "+inner_copy[boost::source(*tmp_edge_iter,inner_copy)].name+" and the port is: "+boost::lexical_cast<std::string>(inner_copy[*tmp_edge_iter].to_port.component_port)); //identify adjacent nodes.
	      results_map.insert(std::make_pair<vertex_t,Priority>(boost::source(*tmp_edge_iter,inner_copy),inner_copy[boost::target(*tmp_edge_iter,inner_copy)].ports.at(inner_copy[*tmp_edge_iter].to_port.component_port)));
	    }
	  }
	  PRINT_DEBUG("RESULT MAP BUILDING: print size of results_map, that is: "+boost::lexical_cast<std::string>(results_map.size()));
	  //the research has to be done HERE
	  vertex_t aaaaa = *tmp_iter;
	  boost::clear_vertex(aaaaa,inner_copy);
	  //boost::remove_vertex(aaaaa,inner_copy);
	  // mappa vertici annullata per via del remove !
	  // prova solo clear, non dovrebbe modificare la mappa dei vertici ma solo togliere i collegamenti. dovrebbe andar bene uguale
#ifdef DEBUG
	  PRINT_DEBUG("neighbor map: for vertex: "+inner_copy[*tmp_iter].name);
	  std::map<vertex_t,Priority>::iterator debug_vertex_priority, debug_vertex_priority_end;
	  debug_vertex_priority_end = results_map.end();
	  for(debug_vertex_priority=results_map.begin();debug_vertex_priority!=debug_vertex_priority_end; ++debug_vertex_priority)
	  {
	    PRINT_DEBUG("neighbor map: neighbour is: "+inner_copy[(*debug_vertex_priority).first].name+" and its priority is: "+boost::lexical_cast<std::string>((*debug_vertex_priority).second));
	  }
	  
#endif
	  vertex_t source;
	  vertex_iter source_iter, source_iter_end;

	  boost::tie(source_iter,source_iter_end) = boost::vertices(inner_copy);
	  for(; source_iter!=source_iter_end; ++source_iter)
	  {
	    PRINT_DEBUG("searching the source node");
	    if(inner_copy[*source_iter].name == fg[*vi].name)
	    {
	      source = *source_iter;
	      PRINT_DEBUG("found");
	      break;//identify the node of thee processing unit
	    }
	  }
	  //build fg (no much sense i think i can just modify the visitor in order to work with inner copy)
	  boost::filtered_graph<Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> tmp_fg(inner_copy,extern_edge_predicate_c(inner_copy),extern_vertex_predicate_c(inner_copy));
	  std::vector<vertex_t> discovered;
	  extern_visitor vis(discovered, source);
	  try {
	    using namespace boost;
	    PRINT_DEBUG("perform the search");
	    depth_first_search(tmp_fg, root_vertex(source).visitor(vis) //research
                 );
	  
	  }
	  catch (int x) {
	    PRINT_DEBUG("found an int in search, the result is: " + boost::lexical_cast<std::string>(x)); //should return 0 when finalizes the source vertex
	  }
	  PRINT_DEBUG("size of discovered is: "+boost::lexical_cast<std::string>(discovered.size()));
	  Priority p = NO_PRIORITY;
	  std::vector<vertex_t>::iterator discovered_it_end= discovered.end();
	  for(std::vector<vertex_t>::iterator discovered_it= discovered.begin();discovered_it!=discovered_it_end; ++discovered_it)
	    {
	      if (results_map.find(*discovered_it) != results_map.end())
		if (p < results_map.at(*discovered_it)) //gets the max priority between the discovered neighbors
		  p = results_map.at(*discovered_it);
	    }
	  PRINT_DEBUG("priority of the component "+tmp_graph_copy_for_research[*tmp_iter].name+" toward the processing unit "+ inner_copy[source].name+" is: "+boost::lexical_cast<std::string>(p));
	  //add edges. works.
	  inner_edge_t e; bool b;
	  boost::tie(e,b) = boost::add_edge(get_node_reference(lvl_4_to_3_map.at(inner_copy[source].name)),get_node_reference(tmp_graph_copy_for_research[*tmp_iter].name),ig);
          ig[e].priority= p;
	  boost::tie(e,b) = boost::add_edge(get_node_reference(tmp_graph_copy_for_research[*tmp_iter].name),get_node_reference(lvl_4_to_3_map.at(inner_copy[source].name)),ig);
          ig[e].priority= p;
	  
	  //TODO add controls on component type in order to avoid back edges if tdma.
	  }
	
	
	

        std::map<vertex_t,Priority>::iterator map_iterator, map_iterator_end;
	for(map_iterator = (string_results_map).begin(), map_iterator_end = (string_results_map).end(); map_iterator!=map_iterator_end; ++map_iterator)
	{
	  PRINT_DEBUG("priority is: "+boost::lexical_cast<std::string>((*map_iterator).second));
	}
      }
    }
    
#ifdef DEBUG
  std::ofstream myfile;
  myfile.open ("/home/emanuele/Documents/tmp_graph/aaa.dot");
  boost::write_graphviz(myfile, ig,make_vertex_writer(boost::get(&Custom_Vertex::layer, ig),boost::get (&Custom_Vertex::name, ig),boost::get(&Custom_Vertex::ports, ig), boost::get(&Custom_Vertex::type,ig ), boost::get(&Custom_Vertex::priority_category,ig))
      ,/*make_edge_writer(boost::get(&Custom_Edge::priority,ig),boost::get(&Custom_Edge::from_port,ig),boost::get(&Custom_Edge::to_port,ig))*/
      boost::make_label_writer(boost::get(&Custom_Edge::priority,ig)));
  myfile.close();
#endif
        
    
  
}
vertex_t internal_graph::get_node_reference(std::string str)
{
  inner_vertex_iter vi, vi_end;
  boost::tie(vi, vi_end) = boost::vertices(ig);
  for (; vi != vi_end; ++vi) 
  {
    if (ig[*vi].name == str)
      return *vi;
  }
  return NULL;
}
bool internal_graph::search_path(std::__cxx11::string from, std::__cxx11::string to, Priority p)
{
  boost::filtered_graph<Internal_Graph,inner_edge_predicate_c> ifg (ig,inner_edge_predicate_c(ig,p));
      inner_visitor vis = inner_visitor(get_node_reference(to));
    try {
      boost::breadth_first_search(
        ifg, get_node_reference(from),boost::visitor(vis)
      );
    }
    catch (int exception) {
      return true;
    }
return false;
}

