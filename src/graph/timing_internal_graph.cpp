#include "timing_internal_graph.h"
#include "custom_visitors.h"

timing_internal_graph::timing_internal_graph(const Source_Graph& g){
    //collassa grafo
    priority_to_ports_map.clear();
    lvl_4_to_3_map.clear();
    std::map<vertex_t,std::map<int,vertex_t> > lvl_3_tracer,lvl_4_tracer;
    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = vertices(g); vp.first != vp.second; ++vp.first)
    {
	if (g[*vp.first].layer == CONTROLLER && g[*vp.first].priority_category == PRIORITY)
	{  
	  vertex_t vtx_vect[PRIORITY_ENUM_SIZE];
	  std::map<int,vertex_t> tmp;
	  
	  for(int i = 0; i < PRIORITY_ENUM_SIZE; i++) 
	  {
	    vertex_t vt =  boost::add_vertex(ig);
	    ig[vt].layer=g[*vp.first].layer;
	    ig[vt].name=g[*vp.first].name+"$$"+boost::lexical_cast<std::string>(i);
	    ig[vt].type=g[*vp.first].type;
	    tmp.insert(std::make_pair(i,vt));
	//    ig[vt].priority_category=g[*vp.first].priority_category;
	    PRINT_DEBUG(ig[vt].name);
	    if (i!= 0)
	    {
	      inner_edge_t e; bool b;
	      boost::tie(e,b) = boost::add_edge(vt,get_node_reference(g[*vp.first].name+"$$"+boost::lexical_cast<std::string>(i-1)),ig);
	    }
	  }
	  lvl_3_tracer.insert(std::make_pair(*vp.first,tmp));
	}
	else if (g[*vp.first].layer == RESOURCE && g[*vp.first].priority_category == PRIORITY)
	{
	  int different_priorities = 0;
	  for (auto& x : g[*vp.first].ports)
	  {
	    if (priority_to_ports_map.count(x.second)== 0)
	    {
	      std::vector<int> tmp;
	      tmp.push_back(x.first);
	      priority_to_ports_map.insert(std::make_pair(x.second,tmp));
	    }
	    else 
	    {
	      priority_to_ports_map.at(x.second).push_back(x.first);
	    }
	  }
	  std::map<int,vertex_t> tmp;
	  for (auto& x: priority_to_ports_map)
	  {
	    vertex_t vt =  boost::add_vertex(ig);
	    ig[vt].layer=g[*vp.first].layer;
	    ig[vt].name=g[*vp.first].name+"$$"+boost::lexical_cast<std::string>(x.first);
	    ig[vt].type=g[*vp.first].type;
	    tmp.insert(std::make_pair(x.first,vt));
	    //ig[vt].priority_category=g[*vp.first].priority_category;
	    PRINT_DEBUG(ig[vt].name);//edge di priorita su porte da creare qua.
	    for (auto y: priority_to_ports_map)
	      if (y.first < x.first)
	      {
		 inner_edge_t e; bool b;
		 boost::tie(e,b) = boost::add_edge(get_node_reference(g[*vp.first].name+"$$"+boost::lexical_cast<std::string>(x.first)),get_node_reference(g[*vp.first].name+"$$"+boost::lexical_cast<std::string>(y.first)),ig);
	      }
	  }
	  lvl_4_tracer.insert(std::make_pair(*vp.first,tmp));
	  priority_to_ports_map.clear();
	}
	else
	{
	  vertex_t vt =  boost::add_vertex(ig);
	  ig[vt].layer=g[*vp.first].layer;
	  ig[vt].name=g[*vp.first].name;
	    //ig[vt].ports = g[*vp.first].ports;
	  ig[vt].type=g[*vp.first].type;
	 // ig[vt].priority_category=g[*vp.first].priority_category;
	  PRINT_DEBUG(ig[vt].name);
	}
    }

    //da rifare. da risistemare i 4th lvl. non eliminare brutalmente ma collegarli alla "priorita" desiderata. conviene farlo qua? credo di si. 
    //risistemare la parte dopo: non mi servono piu i 3 to 4 edges, basta lasciare ogni os collegato al suo so. mantenere intra layer, solo spostarli su priorità desiderata.
    //per ogni edge, crea andata e ritorno nel bidirezionale. tranne che per gli intra layer e i link tra 4 e 5th livello. 3 to 4th lvl vanno creati dopo aver collassato
    //il 4th lvl guardando le priorita delle porte. 4 to 5 vanno mappati subito dopo il collasso a causa di possibile presenza di componenti mappati su stesso nodo collassato presenti su piu board
    edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei)
    {
        inner_edge_t e; bool b;
	vertex_t source,target;
	source = boost::source(*ei,g);
	target = boost::target(*ei,g);
	PRINT_DEBUG("PROVAAAAAAAAAAAAAAAAAA" +boost::lexical_cast<std::string>(g[source].layer+g[target].layer));
	PRINT_DEBUG("PROVAAAAAAAAAAAAAAAAAA" +boost::lexical_cast<std::string>(lvl_3_tracer.size()));
	PRINT_DEBUG("PROVAAAAAAAAAAAAAAAAAA" +boost::lexical_cast<std::string>(lvl_4_tracer.size()));
		      PRINT_DEBUG("the from component is: "+g[*ei].from_port.component_name);
	      PRINT_DEBUG("the source component is: "+g[source].name);
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
	
    }

    //filtra grafo esterno: tieni solo 4 livello e edge interni.
    //itera su vertici. se lay ==4 && edges.count == 1 then collassa.
    

#ifdef DEBUG
    const char* name = "0123456789abcdefghilmnopqrstuvz";
#endif
    Timing_Graph fg2; 
 
    boost::copy_graph(ig, fg2);
    boost::filtered_graph<Timing_Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> fg(fg2,extern_edge_predicate_c(fg2),extern_vertex_predicate_c(fg2));
       PRINT_DEBUG("fg edges number is: ");
#ifdef DEBUG
       boost::print_edges(fg,name);
#endif
  
#ifdef DEBUG
  std::ofstream myfile2;
  myfile2.open ("/home/emanuele/Documents/tmp_graph/aaafiltrato.dot");
  boost::write_graphviz(myfile2, fg,make_vertex_writer(boost::get(&Custom_Vertex::layer, fg),boost::get (&Custom_Vertex::name, fg),boost::get(&Custom_Vertex::ports, fg), boost::get(&Custom_Vertex::type,fg ), boost::get(&Custom_Vertex::priority_category,fg))
      ,/*make_edge_writer(boost::get(&Custom_Edge::priority,ig),boost::get(&Custom_Edge::from_port,ig),boost::get(&Custom_Edge::to_port,ig))*/
      boost::make_label_writer(boost::get(&Custom_Edge::priority,ig)));
  myfile2.close();
#endif
    bool flag_for_collapse = true;
    std::map <std::string, std::vector<std::string> > collapse_maps;
    // init collapse maps with all the lay4 to lay5 edges
   
    vertex_iter vi_init, vi_end_init;
    boost::tie(vi_init, vi_end_init) = boost::vertices(fg2);
    PRINT_DEBUG("collapse map init");
    for (; vi_init != vi_end_init; ++vi_init) 
    {
      if (fg2[*vi_init].layer == RESOURCE)
      {
	PRINT_DEBUG("collapse map resource layer recognized");
	collapse_maps.insert(std::make_pair(fg2[*vi_init].name,std::vector<std::string>()));
	//out edges
	PRINT_DEBUG("collapse map in edge iterator");
	boost::graph_traits<Timing_Graph>::in_edge_iterator ext_e,ext_e_end;
	PRINT_DEBUG("collapse map in edge iterator created");
	boost::tie(ext_e,ext_e_end)=boost::in_edges(*vi_init, fg2);
	PRINT_DEBUG("collapse map in edge iterator created and tied");
	for (; ext_e != ext_e_end; ++ext_e)
	{
	  PRINT_DEBUG("collapse map inside loop");
	  vertex_t source, dest;
	  source = boost::source(*ext_e, fg2);
	  dest = boost::target(*ext_e, fg2);
	  vertex_t tmp_vtx = (source == *vi_init)?dest:source;
	  if (fg2[tmp_vtx].layer == PHYSICAL)
	  {
	    PRINT_DEBUG("collapse map inside if");
	    (collapse_maps.at(fg2[*vi_init].name)).push_back(fg2[tmp_vtx].name);
	    boost::remove_edge(get_node_reference(fg2[*vi_init].name),get_node_reference(fg2[tmp_vtx].name),ig);
	    boost::remove_edge(get_node_reference(fg2[tmp_vtx].name),get_node_reference(fg2[*vi_init].name),ig);
	  }
	}
      }
    }
    PRINT_DEBUG("collapse map init end");
    
    
    //collapse vertices and update the map.
    while(flag_for_collapse)
    {
      flag_for_collapse = false;
      boost::graph_traits<boost::filtered_graph<Source_Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> >::vertex_iterator vi, vi_end,next;
      boost::tie (vi,vi_end) = boost::vertices(fg);
      for (next = vi; vi != vi_end; vi = next) //iteratori invalidati. non posso tenere valore iteratore come chiave mappe!
      {
	++next;
#ifdef DEBUG
	std::string str = "collapse process: out edges size for node: "+ fg[(*vi)].name+ " is: " + boost::lexical_cast<std::string>(boost::in_degree(*vi,fg));
	PRINT_DEBUG (str);
	str = "collapse process: out edges size for node: "+ ((get_node_reference(fg[*vi].name)!= NULL) ?(ig[get_node_reference(fg[*vi].name)].name):"NULL") + " on the original graph is: " + boost::lexical_cast<std::string>(boost::in_degree(*vi,g));
	PRINT_DEBUG (str);
#endif	
	if(boost::in_degree(*vi,fg)==1 && boost::out_degree(*vi,fg)==1 && fg[*vi].type !=PROCESSOR)
	{
	  PRINT_DEBUG("collapse process: collassing component "+ fg2[(*vi)].name);
	  vertex_t vt = get_node_reference(fg[*vi].name);
	  if (vt != NULL) //la seconda run non entra perchè non trova il nodo in get component refernce. 
	  {
	    //prendi edge
	    boost::graph_traits<boost::filtered_graph<Timing_Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> >::in_edge_iterator e,e_end;
	    vertex_t source, dest,supernode;
	    boost::tie(e,e_end)=boost::in_edges(*vi, fg);
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
	      PRINT_DEBUG("collapse process: found the iterator reference to vertex_t");
	       tmp = collapse_maps.at(fg2[*vi].name);
	       collapse_maps.erase(collapse_maps_iterator);
	    }
	    else
	    {
	      PRINT_DEBUG("node not found in collapse map");
	      //throw std::runtime_error ("ERROR, node not found in collapse map");
	      tmp.push_back(fg2[*vi].name);
	      collapse_maps.insert(std::make_pair(fg2[*vi].name,tmp));
	      
	    }
	    //recupera altri edges
	    std::vector<std::string>::iterator vt_vect_iter,vt_vect_iter_end;
	    vt_vect_iter_end = tmp.end();
	    for (vt_vect_iter = tmp.begin(); vt_vect_iter!= vt_vect_iter_end; ++vt_vect_iter)
	    {
	      PRINT_DEBUG("collapse process: check for equality, supernode is: "+fg2[supernode].name+" while the iterator value is: "+*vt_vect_iter);
	      if (std::find(collapse_maps.at(fg2[supernode].name).begin(),collapse_maps.at(fg2[supernode].name).end(),*vt_vect_iter)==collapse_maps.at(fg2[supernode].name).end())
	      {
		PRINT_DEBUG("collapse process: check passed, adding the node");
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
	PRINT_DEBUG("collapse process: -- ++ --");
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
	      //ig[e].priority = SAFETY_CRITICAL;  //not sure though. 
	      boost::tie(e,b) = boost::add_edge(vt2,vt,ig);
	      //ig[e].priority = SAFETY_CRITICAL;
	    }
	  }
	  else
	  {
	    PRINT_DEBUG("ERROR: node not found when building ");
	    throw std::runtime_error(boost::lexical_cast<std::string>("ERROR: node not found when building "));
	  }
    }
        /*  ----------- old --------------
//questa parte penso che sia da eliminare
    //for every processing unit (type = PROCESSOR)
    //create a new graph that is a copy of fg2.
    //remove all other PU
    //do the search and report the results on the new graph
    boost::graph_traits<boost::filtered_graph<Source_Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> >::vertex_iterator vi, vi_end,next;
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
	  std::map<vertex_t,int> results_map;
	 Source_Graph inner_copy; 
	  boost::copy_graph(tmp_graph_copy_for_research,inner_copy);
	  

	
	  edge_iter tmp_edge_iter, tmp_edge_iter_end;
	  boost::tie(tmp_edge_iter, tmp_edge_iter_end) = boost::edges(inner_copy);
	  for(; tmp_edge_iter!=tmp_edge_iter_end; ++tmp_edge_iter)
	  {
	    if(inner_copy[boost::source(*tmp_edge_iter,inner_copy)].name == tmp_graph_copy_for_research[*tmp_iter].name)//removed vtx tmp iter pero è di esterno. 
	    {
	      PRINT_DEBUG("RESULT MAP BUILDING: searching adjacent nodes, is source. dest is: "+ inner_copy[boost::target(*tmp_edge_iter,inner_copy)].name+" and the port is: "+boost::lexical_cast<std::string>(inner_copy[*tmp_edge_iter].from_port.component_port)); //identify adjacent nodes.
	      results_map.insert(std::make_pair(boost::target(*tmp_edge_iter,inner_copy),inner_copy[boost::source(*tmp_edge_iter,inner_copy)].ports.at(inner_copy[*tmp_edge_iter].from_port.component_port)));
	    }
	    else if(inner_copy[boost::target(*tmp_edge_iter,inner_copy)].name == tmp_graph_copy_for_research[*tmp_iter].name)
	    {
	      PRINT_DEBUG("RESULT MAP BUILDING: searching adjacent nodes, is target. source is: "+inner_copy[boost::source(*tmp_edge_iter,inner_copy)].name+" and the port is: "+boost::lexical_cast<std::string>(inner_copy[*tmp_edge_iter].to_port.component_port)); //identify adjacent nodes.
	      results_map.insert(std::make_pair(boost::source(*tmp_edge_iter,inner_copy),inner_copy[boost::target(*tmp_edge_iter,inner_copy)].ports.at(inner_copy[*tmp_edge_iter].to_port.component_port)));
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
	  std::map<vertex_t,int>::iterator debug_vertex_priority, debug_vertex_priority_end;
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
	  boost::filtered_graph<Source_Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> tmp_fg(inner_copy,extern_edge_predicate_c(inner_copy),extern_vertex_predicate_c(inner_copy));
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
	  int p = NO_PRIORITY;
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
          //ig[e].priority= p;
	  if(!(ig[get_node_reference(tmp_graph_copy_for_research[*tmp_iter].name)].priority_category == TDMA || ig[get_node_reference(tmp_graph_copy_for_research[*tmp_iter].name)].type == BRIDGE))
	    boost::tie(e,b) = boost::add_edge(get_node_reference(tmp_graph_copy_for_research[*tmp_iter].name),get_node_reference(lvl_4_to_3_map.at(inner_copy[source].name)),ig);
          //ig[e].priority= p;
	  
	  //TODO add controls on component type in order to avoid back edges if tdma.
	  }
	
	
	

        std::map<vertex_t,Priority>::iterator map_iterator, map_iterator_end;
	for(map_iterator = (string_results_map).begin(), map_iterator_end = (string_results_map).end(); map_iterator!=map_iterator_end; ++map_iterator)
	{
	  PRINT_DEBUG("priority is: "+boost::lexical_cast<std::string>((*map_iterator).second));
	}
      }
    }
    */
#ifdef DEBUG
  std::ofstream myfile;
  myfile.open ("/home/emanuele/Documents/tmp_graph/aaa.dot");
  boost::write_graphviz(myfile, ig,make_vertex_writer(boost::get(&Custom_Vertex::layer, ig),boost::get (&Custom_Vertex::name, ig),boost::get(&Custom_Vertex::ports, ig), boost::get(&Custom_Vertex::type,ig ), boost::get(&Custom_Vertex::priority_category,ig))
      ,/*make_edge_writer(boost::get(&Custom_Edge::priority,ig),boost::get(&Custom_Edge::from_port,ig),boost::get(&Custom_Edge::to_port,ig))*/
      boost::make_label_writer(boost::get(&Custom_Edge::priority,ig)));
  myfile.close();
#endif
        
    
  
}
vertex_t timing_internal_graph::get_node_reference(std::string str)
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
bool timing_internal_graph::search_path(std::string from, std::string to, Layer l)
{
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
          
#if 1
  std::ofstream myfile;
  myfile.open ("/home/emanuele/Documents/tmp_graph/aaafiltrato.dot");
  boost::write_graphviz(myfile, ifg,make_vertex_writer(boost::get(&Custom_Vertex::layer, ifg),boost::get (&Custom_Vertex::name, ifg),boost::get(&Custom_Vertex::ports, ifg), boost::get(&Custom_Vertex::type,ifg ), boost::get(&Custom_Vertex::priority_category,ifg))
      ,/*make_edge_writer(boost::get(&Custom_Edge::priority,ig),boost::get(&Custom_Edge::from_port,ig),boost::get(&Custom_Edge::to_port,ig))*/
      boost::make_label_writer(boost::get(&Custom_Edge::priority,ig)));
  myfile.close();
#endif
     
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