#include "graph_common.hpp"
/****
 
 
 Timing explosions
 
 
 ****/

void First_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map )  {
    PRINT_DEBUG("timing first level vertex building");
    timing_vertex_t vt = boost::add_vertex(graph);
    PRINT_DEBUG("vertex added");
    graph[vt].layer = layer;
    PRINT_DEBUG("layer added"+boost::lexical_cast<std::string>(layer));
    graph[vt].name = this->name;
    PRINT_DEBUG("everything ok");
    
}

void Second_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map )  {
    timing_vertex_t vt = boost::add_vertex(graph);
    
    graph[vt].layer = this->layer;
    PRINT_DEBUG("layer added"+boost::lexical_cast<std::string>(layer));
    graph[vt].name = this->name;
}

void Third_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map< std::string, std::map< int, std::string > >& components_map) 
{
    switch (this->OS_scheduler_type)
    {
        case ROUND_ROBIN:
        {
            timing_vertex_t vt = boost::add_vertex(graph);
            graph[vt].name = this->name;
            graph[vt].layer = this->layer;
            break;
        }
        case PRIORITY:
        {
            std::map< int, timing_vertex_t > tmp;    //pr, new vtx name, inner map of the components_map parameter. 
            std::map< int, std::string > priority_to_name;
            for(std::map <int, Scheduler_Slot>::iterator it = this->priority_slots->begin(); it != this->priority_slots->end();++it) 
            {
                if (tmp.count(it->second.pr) == 0)
                {
                    timing_vertex_t vt =  boost::add_vertex(graph);
                    graph[vt].layer=this->layer;
                    graph[vt].name=this->name+"$$"+boost::lexical_cast<std::string>(it->second.pr);
                    tmp.insert(std::make_pair(it->second.pr,vt));
                    priority_to_name.insert(std::make_pair(it->second.pr,graph[vt].name));
                    PRINT_DEBUG(graph[vt].name);
                }
            }
            std::map< int,timing_vertex_t >::iterator it, it2;
            it = tmp.begin();
            it2 = it;
            it2++;
            //add arrows.
            for ( ;it2 != tmp.end();++it2) 
            {
                timing_edge_t e; bool b;
                boost::tie(e,b) = boost::add_edge(it2->second,it->second,graph);
                it = it2;
            }
            
        components_map.insert(std::make_pair(this->name, priority_to_name));  
        break;
        }
        case TDMA:
        {
            std::map< int, std::string > tmp;    //port, new vtx name, inner map of the components_map parameter
            for (std::map<int, Scheduler_Slot>::const_iterator iter = this->priority_slots->begin();iter != this->priority_slots->end() ;++iter)
            {
                timing_vertex_t vt =  boost::add_vertex(graph);
                graph[vt].layer=this->layer;
                graph[vt].name=this->name+"$$"+boost::lexical_cast<std::string>((iter->second).id);
                tmp.insert(std::make_pair((*iter).second.id,graph[vt].name));
                PRINT_DEBUG(graph[vt].name);
            }
        components_map.insert(std::make_pair(this->name, tmp));  
        break;
        }
        default:
        {
            PRINT_DEBUG("error in transformation of layer 3: no OS_scheduler_type compatible");
            break;
            //TODO error handling
        }
            
    }
    

}

void Fourth_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map< std::string, std::map< int, std::string > >& components_map) 
{
    //questa è grama.
    //switch su tipo, all interno distinzione master/slave.
    //NB loop dependencies are not to be handled here: master will have edges to his own slave and this will not create problems since during the exploration those nodes will be black colored.
    switch (this->component_priority_type)
    {
        case ROUND_ROBIN:
        {
            std::map< int, std::string > tmp;    //port, new vtx name, inner map of the components_map parameter
            std::vector<timing_vertex_t> masters,slaves;
            for (std::map<int, Port>::const_iterator iter = this->ports_map->begin();iter != this->ports_map->end() ;++iter)
            {
                
                if ((*iter).second.is_master)
                {
                    timing_vertex_t vt =  boost::add_vertex(graph);
                    
                    graph[vt].layer=this->layer;
                    graph[vt].name=this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.id);
                    graph[vt].type=this->component_type;
                    (*iter).second.associated_port_id == NO_PORT ? graph[vt].associate_port_name = "" : graph[vt].associate_port_name = this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.associated_port_id);
                    graph[vt].is_master = true;
                    
                   
                    
                    //add edges with other masters
                    for(std::vector<timing_vertex_t>::iterator iter = masters.begin(); iter != masters.end(); ++iter)
                    {
                        timing_edge_t e; bool b;
                        boost::tie(e,b) = boost::add_edge(vt,*iter,graph); //bidirectionality needed
                        boost::tie(e,b) = boost::add_edge(*iter,vt,graph);
                    }
                    //finalize
                    masters.push_back(vt);
                    tmp.insert(std::make_pair((*iter).second.id,graph[vt].name));
                    PRINT_DEBUG(graph[vt].name);
                }
                else
                {
                    timing_vertex_t vt =  boost::add_vertex(graph);
                    graph[vt].layer=this->layer;
                    graph[vt].name=this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.id);
                    (*iter).second.associated_port_id == NO_PORT ? graph[vt].associate_port_name = "" : graph[vt].associate_port_name = this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.associated_port_id);
                    graph[vt].is_master = false;
                    graph[vt].type=this->component_type;
                  
                
                    slaves.push_back(vt);
                    tmp.insert(std::make_pair((*iter).second.id,graph[vt].name));
                    PRINT_DEBUG(graph[vt].name);
                }
            }
            //edges master to slaves
            for(std::vector<timing_vertex_t>::iterator master_iter = masters.begin(); master_iter != masters.end(); ++master_iter)
                for(std::vector<timing_vertex_t>::iterator slave_iter = slaves.begin(); slave_iter != slaves.end(); ++slave_iter)
                {
                    timing_edge_t e; bool b;
                    boost::tie(e,b) = boost::add_edge(*master_iter,*slave_iter,graph); //monodirectional masters to slaves
                }

            //insert tmp map
            components_map.insert(std::make_pair(this->name, tmp));  
            break;
        }
        case PRIORITY:
        {
            std::map< int, std::string > tmp;    //port, new vtx name, inner map of the components_map parameter
            std::map<int,std::vector< timing_vertex_t> > priority_map; //int is priority, will contain all vertex of that priority
            std::vector<timing_vertex_t> slaves;
            PRINT_DEBUG("4th level priority: portsmap size is: "+boost::lexical_cast<std::string>(this->ports_map->size()));
            for (std::map<int, Port>::const_iterator iter = this->ports_map->begin();iter != this->ports_map->end() ;++iter)
            {
                PRINT_DEBUG("4th level priority: iterating on port: "+boost::lexical_cast<std::string>((*iter).first));
                if ((*iter).second.is_master)
                {
                    PRINT_DEBUG("4th level priority: port is master");
                    timing_vertex_t vt =  boost::add_vertex(graph);
                    graph[vt].layer=this->layer;
                    graph[vt].name=this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.id);
                    (*iter).second.associated_port_id == NO_PORT ? graph[vt].associate_port_name = "" : graph[vt].associate_port_name = this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.associated_port_id);
                    graph[vt].is_master = true;
                    graph[vt].type=this->component_type;
           
                    if (priority_map.count((*iter).second.priority) == 0)
                    {
                        std::vector<timing_vertex_t> tmp_vect;
                        tmp_vect.push_back(vt);
                        priority_map.insert(std::make_pair((*iter).second.priority,tmp_vect));
                    }
                    else 
                    {
                        priority_map.at((*iter).second.priority).push_back(vt);
                    }
                    tmp.insert(std::make_pair((*iter).second.id,graph[vt].name));
                    PRINT_DEBUG(graph[vt].name);
                }
                else
                {
                    PRINT_DEBUG("4th level priority: port is slave");
                    timing_vertex_t vt =  boost::add_vertex(graph);
                    graph[vt].layer=this->layer;
                    graph[vt].name=this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.id);
                    (*iter).second.associated_port_id == NO_PORT ? graph[vt].associate_port_name = "" : graph[vt].associate_port_name = this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.associated_port_id);
                    graph[vt].is_master = false;
                    graph[vt].type=this->component_type;
                    slaves.push_back(vt);
                    tmp.insert(std::make_pair((*iter).second.id,graph[vt].name));
                    PRINT_DEBUG(graph[vt].name);
                }
            }
            //internal edges creation
            PRINT_DEBUG("ports created, internal edges creation start");
            bool flag = false;
            timing_vertex_t tmp_vtx;
            PRINT_DEBUG("priority map size is: "+boost::lexical_cast<std::string>(priority_map.size()));
            for (std::map<int,std::vector< timing_vertex_t> >::reverse_iterator extern_iter = priority_map.rbegin();extern_iter != priority_map.rend();++extern_iter)
            {
                PRINT_DEBUG("4th level priority: internal edges creation. priority is: "+boost::lexical_cast<std::string>((*extern_iter).first));
                std::vector<timing_vertex_t>::iterator vertex_iter = (*extern_iter).second.begin();
                if(flag)
                {
                    PRINT_DEBUG("4th level priority: flag is true: adding high to low ");
                    timing_edge_t e; bool b;
                    boost::tie(e,b) = boost::add_edge(tmp_vtx,*vertex_iter,graph); //monodirectional high pr to low, only one is needed
                }
                else
                {
                    PRINT_DEBUG("4th level priority: flag is false: skip");
                }
                tmp_vtx = *vertex_iter;
                //all to all in the same priority
                for (;vertex_iter != (*extern_iter).second.end(); ++vertex_iter)
                {
                    PRINT_DEBUG("4th level priority: inner loop, same priority looping extern_iter ");
                    for(std::vector<timing_vertex_t>::iterator vertex_iter2 = (*extern_iter).second.begin();vertex_iter2 != (*extern_iter).second.end();++vertex_iter2)
                    {
                        PRINT_DEBUG("4th level priority: inner loop, same priority looping inner_iter ");
                        if((*vertex_iter2)!=(*vertex_iter))
                        {
                            PRINT_DEBUG("4th level priority: inner loop, vertexes are differents and are: "+graph[*vertex_iter2].name+" and " +graph[*vertex_iter].name+ " , adding edge");
                            timing_edge_t e; bool b;
                            boost::tie(e,b) = boost::add_edge(*vertex_iter2,*vertex_iter,graph); //monodirectional, the opposite direction will be done when vertex iter and vertex iter 2 will have opposite values.
                        }
                        else 
                        {
                            PRINT_DEBUG("4th level priority: inner loop, vertexes equals, skip ");
                        }
                    }
                    
                }
                PRINT_DEBUG("4th level priority: after loop ");
                flag = true;
                
            }
            
            if (flag) //had to be skipd if only one slave is present.
            {
                for(std::vector<timing_vertex_t>::iterator slave_iter = slaves.begin(); slave_iter != slaves.end(); ++slave_iter)
                {
                    timing_edge_t e; bool b;
                    boost::tie(e,b) = boost::add_edge(tmp_vtx,*slave_iter,graph); //monodirectional masters to slaves
                }
            }
            components_map.insert(std::make_pair(this->name, tmp));  
            break;
        }
        case TDMA:
        {
            std::map< int, std::string > tmp;    //port, new vtx name, inner map of the components_map parameter
            std::vector<timing_vertex_t> masters,slaves;
            for (std::map<int, Port>::const_iterator iter = this->ports_map->begin();iter != this->ports_map->end() ;++iter)
            {
                
                if ((*iter).second.is_master)
                {
                    timing_vertex_t vt =  boost::add_vertex(graph);
                    graph[vt].layer=this->layer;
                    graph[vt].name=this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.id);
                    (*iter).second.associated_port_id == NO_PORT ? graph[vt].associate_port_name = "" : graph[vt].associate_port_name = this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.associated_port_id);
                    graph[vt].is_master = true;
                    graph[vt].type=this->component_type;
                    //finalize
                    masters.push_back(vt);
                    tmp.insert(std::make_pair((*iter).second.id,graph[vt].name));
                    PRINT_DEBUG(graph[vt].name);
                }
                else
                {
                    timing_vertex_t vt =  boost::add_vertex(graph);
                    graph[vt].layer=this->layer;
                    graph[vt].name=this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.id);
                    (*iter).second.associated_port_id == NO_PORT ? graph[vt].associate_port_name = "" : graph[vt].associate_port_name = this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.associated_port_id);
                    graph[vt].is_master = false;
                    graph[vt].type=this->component_type;
                    slaves.push_back(vt);
                    tmp.insert(std::make_pair((*iter).second.id,graph[vt].name));
                    PRINT_DEBUG(graph[vt].name);
                }
            }
            //edges master to slaves
            for(std::vector<timing_vertex_t>::iterator master_iter = masters.begin(); master_iter != masters.end(); ++master_iter)
                for(std::vector<timing_vertex_t>::iterator slave_iter = slaves.begin(); slave_iter != slaves.end(); ++slave_iter)
                {
                    if (graph[*master_iter].associate_port_name != graph[*slave_iter].name)
                    {
                    timing_edge_t e; bool b;
                    boost::tie(e,b) = boost::add_edge(*master_iter,*slave_iter,graph); //monodirectional masters to slaves
                    }
                }

            //insert tmp map
            components_map.insert(std::make_pair(this->name, tmp));
            break;
        }
        default:
        {
            PRINT_DEBUG("error in transformation of layer 4: no priority type compatible");
            break;
            //TODO error handling
        }
            
        
    }

    PRINT_DEBUG("L4 component explode finish");
}

void Fifth_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map< std::string, std::map< int, std::string > >& components_map) 
{
    timing_vertex_t vt = boost::add_vertex(graph);
    graph[vt].name = this->name;
    graph[vt].layer = this->layer;
    
}


/***
 
 
 
 FTA explosion 
 
 
 ***/
void First_Level_Vertex::explode_component_FTA(FT_Graph& graph)
{
    ft_vertex_t vt = boost::add_vertex(graph);
    graph[vt].name = this->name;
    graph[vt].layer = this->layer;
}
void Second_Level_Vertex::explode_component_FTA(FT_Graph& graph)
{
    ft_vertex_t vt = boost::add_vertex(graph);
    graph[vt].name = this->name;
    graph[vt].layer = this->layer;
}
void Third_Level_Vertex::explode_component_FTA(FT_Graph& graph)
{
    ft_vertex_t vt = boost::add_vertex(graph);
    graph[vt].crit = this->OS_scheduler_type;
    graph[vt].name = this->name;
    graph[vt].layer = this->layer;
}
void Fourth_Level_Vertex::explode_component_FTA(FT_Graph& graph)
{
    ft_vertex_t vt = boost::add_vertex(graph);
    graph[vt].crit = this->component_priority_type;
    graph[vt].name = this->name;
    graph[vt].layer = this->layer;
}
void Fifth_Level_Vertex::explode_component_FTA(FT_Graph& graph)
{
    ft_vertex_t vt = boost::add_vertex(graph);
    graph[vt].name = this->name;
    graph[vt].layer = this->layer;
}

