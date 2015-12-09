#include "internal_graph.h"
#include "custom_visitors.h"

internal_graph::internal_graph(const Graph& g){
    //collassa grafo
    std::map<vertex_t,vertex_t> tmp_correspondence_map;
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
        tmp_correspondence_map.insert(std::make_pair<vertex_t,vertex_t>(*vp.first,vt));
        PRINT_DEBUG(ig[vt].name);

    }

    //per ogni edge, crea andata e ritorno nel bidirezionale. tranne che per gli intra layer. verranno aggiunti inter layer nel pezzo del "collasso"
    edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei)
    {
        inner_edge_t e; bool b;
        if(g[boost::source(*ei,g)].layer!=g[boost::target(*ei,g)].layer)
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
    const boost::filtered_graph<const Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> fg(g,extern_edge_predicate_c(g),extern_vertex_predicate_c(g));
       PRINT_DEBUG("fg edges number is: ");
#ifdef DEBUG
       boost::print_edges(fg,name);
#endif
    //PRINT_DEBUG(vertex_map.at(from));
    
    bool flag_for_collapse = true;
    typedef boost::graph_traits<boost::filtered_graph<const Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> >::vertex_iterator filter_vtx_iter;
    std::pair<filter_vtx_iter, filter_vtx_iter> fvp;
    while(flag_for_collapse)
    {
      flag_for_collapse = false;
      filter_vtx_iter vi, vi_end,next;
      boost::tie (vi,vi_end) = boost::vertices(fg);
      for (next = vi; vi != vi_end; vi = next)
		/*
	 * graph_traits<Graph>::vertex_iterator vi, vi_end, next;
  tie(vi, vi_end) = vertices(G);
  for (next = vi; vi != vi_end; vi = next) {
    ++next;
    remove_vertex(*vi, G);
  }
	 * 
	 * 
	 * 
	 */
      {
	
	++next;
#ifdef DEBUG
	std::string str = "out edges size for node: "+ boost::lexical_cast<std::string>(*vi)+ " is: " + boost::lexical_cast<std::string>(boost::out_degree(*vi,fg));
	PRINT_DEBUG (str);
	str = "out edges size for node: "+ boost::lexical_cast<std::string>(*vi)+ " on the original graph is: " + boost::lexical_cast<std::string>(boost::out_degree(*vi,g));
	PRINT_DEBUG (str);
	if (fg[*vi].priority_category != 0)
	  PRINT_DEBUG(fg[*vi].priority_category);
#endif	
	if(boost::out_degree(*vi,fg)==1 && fg[*vi].type !=PROCESSOR)
	{
	  PRINT_DEBUG("collassing component "+ boost::lexical_cast<std::string>(tmp_correspondence_map.at(*vi)));
	  PRINT_DEBUG(ig[tmp_correspondence_map.at(*vi)].name);
	  boost::clear_vertex(tmp_correspondence_map.at(*vi),ig);
	  boost::remove_vertex(tmp_correspondence_map.at(*vi),ig); //non va bene. rimuove nodo ma non toglielndolo su entrambi fa casino. va trovato modo di rimuoverlo dal source. oppure prendere il descrittore giusto
	  //problema che const graph ? 
	  PRINT_DEBUG(g[*vi].name);
	  vertex_t vt = *vi;
	 //boost::clear_vertex(*vi,g);
	 //boost::remove_vertex(vt,g);
	  //flag_for_collapse = true;
	  //break;
	}
	  
      }
    }
    
    
    #ifdef DEBUG
       boost::print_vertices(ig,name);
#endif
    
    
  
}
