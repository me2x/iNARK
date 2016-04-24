#include "timing_internal_graph.hpp"
#include "custom_visitors.hpp"
timing_internal_graph::timing_internal_graph()
{
    ig = Timing_Graph();
}

void timing_internal_graph::build_graph(std::shared_ptr<Source_Graph> g){
    std::cout<<"print called"<<std::endl;
    PRINT_DEBUG("print called");
    ig.clear();
    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = boost::vertices(*g); vp.first != vp.second; ++vp.first)
    {
        PRINT_DEBUG("timing graph construction: vertex loop, layer is: "+boost::lexical_cast<std::string>((*g)[*vp.first].get_layer()) +" and name is: "+boost::lexical_cast<std::string>((*g)[*vp.first].get_name()));
        (*g)[*vp.first].explode_component_timing(ig,components_map);
        
  
    }

    PRINT_DEBUG("components map size is: "+boost::lexical_cast<std::string>(components_map.size()));
    
    edge_iter ei, ei_end;

    for (boost::tie(ei, ei_end) = boost::edges(*g); ei != ei_end; ++ei)
    {
        timing_edge_t e; bool b;
	vertex_t old_graph_source,old_graph_target;
	old_graph_source = boost::source(*ei,*g);
	old_graph_target = boost::target(*ei,*g);
        timing_vertex_t new_source, new_target;
	PRINT_DEBUG("the source component is: "+(*g)[old_graph_source].get_name()+" and its port is: "+boost::lexical_cast<std::string>((*g)[*ei].from_port));
        PRINT_DEBUG("the target component is: "+(*g)[old_graph_target].get_name()+" and its port is: "+boost::lexical_cast<std::string>((*g)[*ei].to_port));
        //mancano 4 to 5: da moltiplicare.
        //all edges are in 1:1 between the physical graph to the internal representation, exept the edges from l4 to l5. 
        //OS to processor have to be multiplied
        //task to os has to be fixed. to port dovrebbe mappare allo scheduler slot. ed infatti funziona.
        //for that layer i have to build one edge for each new resource to the physical component
        //!((*g)[old_graph_source].get_layer() != PHYSICAL && (*g)[old_graph_target].get_layer() == PHYSICAL)&& !((*g)[old_graph_source].get_layer() == PHYSICAL && (*g)[old_graph_target].get_layer() != PHYSICAL)
        
        
        //fai con uno switch su somma layers. caso base: edges 1:1 
        //cases: 0 func to func, 1 func to task, 2 tast to task, 3 task to os, 4 os to os, 5 os to processor, 6 resource to resource, 7 resource to physical
        switch((*g)[old_graph_source].get_layer()+(*g)[old_graph_target].get_layer())
        {
            case 4: //all to all. è un po' overkill ma non dovrebbe creare dipendenze supplementari. aggiunge un sacco di edges e non so se ciò possa rallentare esplorazione.
            {
                PRINT_DEBUG("edge creation: inside switch, case 4");
                
                if (components_map.count((*g)[old_graph_source].get_name()) != 0)
                {
                    if (components_map.count((*g)[old_graph_target].get_name()) != 0)
                    {
                        //doppio for
                        for (std::map<int,std::string>::iterator many_to_one = components_map.at((*g)[old_graph_source].get_name()).begin();many_to_one != components_map.at((*g)[old_graph_source].get_name()).end();++many_to_one)
                        {
                            for (std::map<int,std::string>::iterator one_to_many_iter = components_map.at((*g)[old_graph_target].get_name()).begin();one_to_many_iter != components_map.at((*g)[old_graph_target].get_name()).end();++one_to_many_iter)
                                {
                                    new_source = get_node_reference((*many_to_one).second);
                                    new_target = get_node_reference((*one_to_many_iter).second);
                                    boost::tie(e,b) = boost::add_edge(new_source,new_target,ig);
                                }
                        }
                                
                    }
                    else
                    {
                        //singolo for
                        for (std::map<int,std::string>::iterator many_to_one = components_map.at((*g)[old_graph_source].get_name()).begin();many_to_one != components_map.at((*g)[old_graph_source].get_name()).end();++many_to_one)
                        {
                         new_target = get_node_reference((*g)[old_graph_target].get_name());
                         new_source = get_node_reference((*many_to_one).second);
                         boost::tie(e,b) = boost::add_edge(new_source,new_target,ig);
                        }
                    }
                }
                else
                {
                    if (components_map.count((*g)[old_graph_target].get_name()) != 0)
                    {
                        //singolo for
                        for (std::map<int,std::string>::iterator one_to_many_iter = components_map.at((*g)[old_graph_target].get_name()).begin();one_to_many_iter != components_map.at((*g)[old_graph_target].get_name()).end();++one_to_many_iter)
                        {
                         new_source = get_node_reference((*g)[old_graph_source].get_name());
                         new_target = get_node_reference((*one_to_many_iter).second);
                         boost::tie(e,b) = boost::add_edge(new_source,new_target,ig);
                        }
                    }
                    else
                    {
                        //niente for
                        boost::tie(e,b) = boost::add_edge(get_node_reference((*g)[old_graph_source].get_name()),get_node_reference((*g)[old_graph_target].get_name()),ig);
                    }
                    
                }
                break;
            }
            case 5:
            {
                PRINT_DEBUG("edge creation: inside switch, case 5");
                bool l4_is_source = (*g)[old_graph_source].get_layer() == RESOURCE;
                if (components_map.count((*g)[l4_is_source?old_graph_target:old_graph_source].get_name()) != 0)
                {
                    for (std::map<int,std::string>::iterator l3_to_l4_iter = components_map.at((*g)[l4_is_source?old_graph_target:old_graph_source].get_name()).begin();l3_to_l4_iter != components_map.at((*g)[l4_is_source?old_graph_target:old_graph_source].get_name()).end();++l3_to_l4_iter)
                    {
                        PRINT_DEBUG("edge creation: components map at: "+(*g)[l4_is_source?old_graph_target:old_graph_source].get_name()+ " size is: "+boost::lexical_cast<std::string>(components_map.at((*g)[l4_is_source?old_graph_target:old_graph_source].get_name()).size()));
                        new_source = get_node_reference(l4_is_source? (*g)[old_graph_source].get_name()+"$$1" : (*l3_to_l4_iter).second);
                        if (l4_is_source)
                        {
                            PRINT_DEBUG("edge creation: old graph source name is: "+(*g)[old_graph_source].get_name() +"but the retrieved node is: "+ig[new_source].name);
                            PRINT_DEBUG("the result of get node reference is: "+ boost::lexical_cast<std::string>(get_node_reference((*g)[old_graph_source].get_name())));
                        }
                        
                        new_target = get_node_reference(l4_is_source? (*l3_to_l4_iter).second:(*g)[old_graph_target].get_name()+"$$1"); //the $$1 is added only to processors.
                        
                        PRINT_DEBUG("edge creation: source node is: "+ig[new_source].name+ "while old graph source is: "+(*g)[old_graph_source].get_name()+" and target is: "+ig[new_target].name+" while old graph target is: "+(*g)[old_graph_target].get_name());
                        boost::tie(e,b) = boost::add_edge(new_source,new_target,ig);
                    }
                }
                else
                {
                    bool l4_is_source = (*g)[old_graph_source].get_layer() == RESOURCE;
                    new_source = get_node_reference((*g)[old_graph_source].get_name()+(l4_is_source?"$$1":""));
                    new_target = get_node_reference((*g)[old_graph_target].get_name()+(l4_is_source?"":"$$1"));
                    boost::tie(e,b) = boost::add_edge(new_source,new_target,ig);
                }
                break;
            }
            case 7:
            {
                bool l4_is_source = (*g)[old_graph_source].get_layer() == RESOURCE;
                for (std::map<int,std::string>::iterator l4_to_l5_iter = components_map.at((*g)[l4_is_source?old_graph_source:old_graph_target].get_name()).begin();l4_to_l5_iter != components_map.at((*g)[l4_is_source?old_graph_source:old_graph_target].get_name()).end();++l4_to_l5_iter)
                {
                    PRINT_DEBUG("components map at: "+(*g)[l4_is_source?old_graph_source:old_graph_target].get_name()+ " size is: "+boost::lexical_cast<std::string>(components_map.at((*g)[l4_is_source?old_graph_source:old_graph_target].get_name()).size()));
                    new_source = get_node_reference(l4_is_source? (*l4_to_l5_iter).second:(*g)[old_graph_source].get_name() );
                    new_target = get_node_reference(l4_is_source? (*g)[old_graph_target].get_name() : (*l4_to_l5_iter).second);
                    boost::tie(e,b) = boost::add_edge(new_source,new_target,ig);
                }
                break;
            }
            default: 
            {
                if (components_map.count((*g)[old_graph_source].get_name()) != 0)
                {
                    new_source = get_node_reference(components_map.at((*g)[old_graph_source].get_name()).at((*g)[*ei].from_port !=NO_PORT? (*g)[*ei].from_port:1)); //pos 1 se non specificato serve per prendere componenti unici che sono stati in qualche modo toccati nella funzione di explode.
                }
                else
                {
                    new_source = get_node_reference((*g)[old_graph_source].get_name());
                }
                if (components_map.count((*g)[old_graph_target].get_name()) != 0)
                {
                    new_target = get_node_reference(components_map.at((*g)[old_graph_target].get_name()).at((*g)[*ei].to_port !=NO_PORT? (*g)[*ei].to_port:1));
                }
                else
                {
                    new_target = get_node_reference((*g)[old_graph_target].get_name());
                }
                
                boost::tie(e,b) = boost::add_edge(new_source,new_target,ig);
                break;
            }
            
        }


    }
 

    std::ofstream myfile2;
    myfile2.open ("/home/emanuele/Documents/tmp_graph/aaaedged.dot");
    boost::write_graphviz(myfile2, ig,make_vertex_writer(boost::get(&Timing_Node::layer, ig),boost::get (&Timing_Node::name, ig)));
    myfile2.close();  
      
    //search for master tasks.
    //preparation: get processors
    std::pair<timing_vertex_iter, timing_vertex_iter> tvp;
    std::vector <timing_vertex_t> processor_vertex_t;
    for (tvp = vertices(ig); tvp.first != tvp.second; ++tvp.first)
    {
        if(ig[*tvp.first].type == PROCESSOR)
        {
            processor_vertex_t.push_back(*tvp.first);
        }
    }
    //search tasks for every processor
            
    boost::filtered_graph<Timing_Graph,true_edge_predicate,task_search_filter_c> task_per_processor_fg (ig,true_edge_predicate(ig),task_search_filter_c(ig));   
    masters_task_research_visitor::colormap map = get(boost::vertex_color, task_per_processor_fg);
    std::pair<boost::filtered_graph<Timing_Graph,true_edge_predicate,task_search_filter_c>::vertex_iterator, boost::filtered_graph<Timing_Graph,true_edge_predicate,task_search_filter_c>::vertex_iterator> processor_to_task_f_vp;
    std::map<timing_vertex_t, std::vector <std::string>> processor_to_task_map;
    for(std::vector<timing_vertex_t>::iterator processor_iter = processor_vertex_t.begin();processor_iter != processor_vertex_t.end();++processor_iter)
    {
        std::vector <std::string> vtxes;
        masters_task_research_visitor vis = masters_task_research_visitor(vtxes);
        vis.vertex_coloring = map; //should be passed by copy, so it should be ok. no. it is modified by reference so has to be resetted every time. note that it also has to be re_blackened
        //filtered graph?
        boost::depth_first_visit(task_per_processor_fg, *processor_iter,vis, map);
        processor_to_task_map.insert(std::make_pair(*processor_iter, vtxes));
        for (processor_to_task_f_vp = vertices(task_per_processor_fg); processor_to_task_f_vp.first != processor_to_task_f_vp.second; ++processor_to_task_f_vp.first)
            map[*processor_to_task_f_vp.first] = boost::default_color_type::white_color;
    }
#ifdef DEBUG
    PRINT_DEBUG("processor to task mapping done. size of map is: "+boost::lexical_cast<std::string>(processor_to_task_map.size()));
    for(std::map<timing_vertex_t, std::vector <std::string>>::iterator debug_iter = processor_to_task_map.begin(); debug_iter != processor_to_task_map.end(); ++debug_iter)
    {
        PRINT_DEBUG("processor to task mapping done. size of vector is: "+boost::lexical_cast<std::string>((*debug_iter).second.size()));
        PRINT_DEBUG("processor to task mapping done. name of processor is is: "+ig[(*debug_iter).first].name);
        for(std::vector <std::string>::iterator debug_iter2 =  (*debug_iter).second.begin();debug_iter2 !=  (*debug_iter).second.end();++debug_iter2)
        {
            PRINT_DEBUG("processor to task mapping done. names of the vector are: "+*debug_iter2);
        }
    }
#endif

    //filter graph (keep only 4th level)
    boost::filtered_graph<Timing_Graph,true_edge_predicate,lv4_vertex_predicate_c> ifg (ig,true_edge_predicate(ig),lv4_vertex_predicate_c(ig));
    
    std::ofstream myfile;
    myfile.open ("/home/emanuele/Documents/tmp_graph/aaafiltrato.dot");
    boost::write_graphviz(myfile, ifg,make_vertex_writer(boost::get(&Timing_Node::layer, ifg),boost::get (&Timing_Node::name, ifg)));
    myfile.close();
    //build a map with the association name :: timing_vertex_t to pass to the following algorithms. only the 4th level nodes are needed
    std::pair<boost::filtered_graph<Timing_Graph,true_edge_predicate,lv4_vertex_predicate_c>::vertex_iterator, boost::filtered_graph<Timing_Graph,true_edge_predicate,lv4_vertex_predicate_c>::vertex_iterator> ftvp;
    
    for (ftvp = vertices(ifg); ftvp.first != ftvp.second; ++ftvp.first)
    {
        name_to_node_map.insert(std::make_pair(ifg[*ftvp.first].name,*ftvp.first ));
    }
    
    
    
    //for eaech processor, do a dfs or bfs to search interfered tasks. that is, master ports reached from a slave one.
    // start: slave port of processor: flag = true.
    // next: is_master, flag -> save, unset, continue
    // next: is_master, !flag -> continue
    // next: is_slave, !flag -> set
    // un po' piu complessa: devo salvare su una mappa le master attraversate e fare context swith con le slaves, di modo che le master siano sbiancate on backtrack delle slaves. questo perche lo stesso componente
    //puo essere attraversato in piu direzioni e se le master sono scurite si perdono informazioni.
    masters_task_setter_visitor::colormap master_setter_map = get(boost::vertex_color, ifg);
    
   
    for(std::vector<timing_vertex_t>::iterator processor_iter = processor_vertex_t.begin();processor_iter != processor_vertex_t.end();++processor_iter)
    {
        std::vector<timing_vertex_t> result;
        for (ftvp = vertices(ifg); ftvp.first != ftvp.second; ++ftvp.first)
            master_setter_map[*ftvp.first] = boost::default_color_type::white_color;
        masters_task_setter_visitor master_setter_vis = masters_task_setter_visitor(result, name_to_node_map);
        master_setter_vis.vertex_coloring=master_setter_map;
        boost::depth_first_visit(ifg, *processor_iter,master_setter_vis, master_setter_map);
        
        
        //analyze result.
        for(std::vector<timing_vertex_t>::iterator result_it = result.begin(); result_it != result.end(); ++result_it)
            for(std::vector <std::string>::iterator task_it =processor_to_task_map.at(*processor_iter).begin();task_it != processor_to_task_map.at(*processor_iter).end(); ++task_it)
                ig[*result_it].master_tasks.insert(*task_it);
    }

    
     for (tvp = vertices(ig); tvp.first != tvp.second; ++tvp.first)
    {
        PRINT_DEBUG("after master setter step. the master of: "+ig[*tvp.first].name+" are (size): "+boost::lexical_cast<std::string>(ig[*tvp.first].master_tasks.size()));
        for (std::set<std::string>::iterator debug_iter=ig[*tvp.first].master_tasks.begin();debug_iter!=ig[*tvp.first].master_tasks.end();++debug_iter)
            PRINT_DEBUG("and the task are: "+*debug_iter);
    }
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
  return Timing_Graph::null_vertex();
}
bool timing_internal_graph::search_path(std::string from, std::string to, Layer l)
{
    //check from and to are valid names for the graph:
    if (get_node_reference(from)== Timing_Graph::null_vertex())
        throw std::runtime_error ("node "+from+" does not exist in the graph");
    if (get_node_reference(to)== Timing_Graph::null_vertex())
        throw std::runtime_error ("node "+to+" does not exist in the graph");
    PRINT_DEBUG("from is: "+from +"and get_node_reference(from) returns vertex: "+ig[get_node_reference(from)].name);
    PRINT_DEBUG("to is: "+to +"and get_node_reference(from) returns vertex: "+ig[get_node_reference(to)].name);
    boost::filtered_graph<Timing_Graph,true_edge_predicate,layer_filter_vertex_predicate_c> ifg (ig,true_edge_predicate(ig),layer_filter_vertex_predicate_c(ig,l));
    PRINT_DEBUG("in the filtered graph those nodes are from: "+ifg[get_node_reference(from)].name+" and to: "+ifg[get_node_reference(to)].name);
    exploration_from_interferes_with_to_visitor::colormap master_setter_map = get(boost::vertex_color, ifg);
    std::pair<boost::filtered_graph<Timing_Graph,true_edge_predicate,layer_filter_vertex_predicate_c>::vertex_iterator, boost::filtered_graph<Timing_Graph,true_edge_predicate,layer_filter_vertex_predicate_c>::vertex_iterator> ftvp;
    for (ftvp = vertices(ifg); ftvp.first != ftvp.second; ++ftvp.first)
        master_setter_map[*ftvp.first] = boost::default_color_type::white_color;
    exploration_from_interferes_with_to_visitor master_setter_vis = exploration_from_interferes_with_to_visitor(get_node_reference(to), name_to_node_map, l);
    master_setter_vis.vertex_coloring=master_setter_map;
    
    
     try {
      boost::depth_first_visit(ifg, get_node_reference(from),master_setter_vis, master_setter_map);
    }
    catch (std::runtime_error e)
    {
        //std::cout << e.what();
        return false;
    }
    
    
    
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
return true;
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