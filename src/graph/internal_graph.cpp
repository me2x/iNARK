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
       /* if(*vp.first.ports){
            ig[vt].ports.insert(std::make_pair<int,Priority>(i_v.second.get_child("id").get_value<int>(),int_to_Priority(i_v.second.get_child("priority").get_value<int>())));
            }
        }
        if(v.second.get_child_optional("type")){
            local_graph[vt].type=int_To_Type(v.second.get_child("type").get_value<int>());
        }
        vertex_map.insert(std::make_pair(local_graph[vt].name, vt));*/
        
        PRINT_DEBUG(ig[vt].name);

    }

    //per ogni edge, crea andata e ritorno nel bidirezionale. tranne che per gli intra layer. verranno aggiunti inter layer nel pezzo del "collasso"
    edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei)
    {
        inner_edge_t e; bool b;
        if(g[boost::source(*ei,g)].layer!=g[boost::target(*ei,g)].layer && g[boost::source(*ei,g)].layer!= PHYSICAL && g[boost::target(*ei,g)].layer != PHYSICAL)
        {
            boost::tie(e,b) = boost::add_edge(boost::source(*ei,g),boost::target(*ei,g),ig);
            ig[e].priority= g[*ei].priority;
            PRINT_DEBUG(boost::target(e,ig));

        /*if (v.second.get_child_optional("priority")){
            local_graph[e].priority=int_to_Priority(v.second.get_child("priority").get_value<int>());
        }
        else
        {
            local_graph[e].priority=int_to_Priority(NO_PRIORITY);
        }
        if (v.second.get_child_optional("to.port")){
         edge_to_port_map.insert(std::make_pair<edge_t,int>(e,v.second.get_child("to.port").get_value<int>()));
        }*/
            boost::tie(e,b) = boost::add_edge(boost::target(*ei,g),boost::source(*ei,g),ig);
            ig[e].priority= g[*ei].priority;
            PRINT_DEBUG(boost::target(e,ig));
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
    //PRINT_DEBUG(vertex_map.at(from));
    //TODO copia fg su tmp_graph su cui si possano eliminare vtx. se no l'algoritmo non funziona xk nel layer sottostante sempre trova grafo vecchio e non riesce a collassare in modo ricorsivo.
       
       
       
       
    bool flag_for_collapse = true;
    std::map <std::string, std::vector<std::string> > collapse_maps;
    // init collapse maps with all the lay4 to lay5 edges
    inner_vertex_iter vi, vi_end;
  boost::tie(vi, vi_end) = boost::vertices(fg2);
  for (; vi != vi_end; ++vi) 
  {
    if (fg2[*vi].layer == RESOURCE)
    {
      collapse_maps.insert(std::make_pair<std::string,std::vector<std::string> >(fg2[*vi].name,std::vector<std::string>()));
      //out edges
      ext_out_edge_iter ext_e,ext_e_end;
      boost::tie(ext_e,ext_e_end)=boost::out_edges(*vi, fg2);
      for (; ext_e != ext_e_end; ++ext_e)
      {
	vertex_t source, dest;
	source = boost::source(*ext_e, fg2);
	dest = boost::target(*ext_e, fg2);
	vertex_t tmp_vtx = (source == *vi)?dest:source;
	if (fg2[tmp_vtx].layer == PHYSICAL)
	(collapse_maps.at(fg2[*vi].name)).push_back(fg2[tmp_vtx].name);
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
	if (fg[*vi].priority_category != 0)
	  PRINT_DEBUG(fg[*vi].priority_category);
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
    
    
#ifdef DEBUG
  std::ofstream myfile;
  myfile.open ("/home/emanuele/Documents/tmp_graph/aaa.dot");
  boost::write_graphviz(myfile, ig);
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
