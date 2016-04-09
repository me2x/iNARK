#include "graph_common.hpp"


Layer commons::int_to_Layer(int i){
    switch(i)
    {
    case 1:
        return FUNCTION;
        break;
    case 2:
        return TASK;
        break;
    case 3:
        return CONTROLLER;
        break;
    case 4:
        return RESOURCE;
        break;
    case 5:
        return PHYSICAL;
        break;
    default:
        return LAYER_ERROR;
        break;
    }
}
Priority commons::int_to_Priority(int i){
    switch(i)
    {
    case 1:
        return NO_PRIORITY;
        break;
    case 2:
        return MISSION_CRITICAL;
        break;
    case 3:
        return SAFETY_CRITICAL;
        break;
    default:
        return PRIORITY_ENUM_SIZE;
        break;
    }
}
std::string commons::Layer_to_String(Layer l){
    switch(l)
    {
    case FUNCTION:
        return "function";
        break;
    case TASK:
        return "task" ;
        break;
    case CONTROLLER:
        return "controller";
        break;
    case RESOURCE:
        return "resource";
        break;
    case PHYSICAL:
        return "physical";
        break;
    default:
        return "error";
        break;
    }
}
Component_Type commons::int_To_Type(int i){
    switch(i)
    {
    case 1:
        return PROCESSOR;break;

    case 2:
        return BUS;break;

    case 3:
        return BRIDGE;break;

    case 4:
        return PERIPHERAL;break;
        
    case 5:
        return MEMORY;break;

    default:
        return TYPE_ERROR;break;
    }
}
Component_Priority_Category commons::int_To_Priority_Handler(int i){
switch(i)
    {
    case 1:
        return ROUND_ROBIN;break;

    case 2:
        return PRIORITY;break;

    case 3:
        return TDMA;break;

    default:
        return PRIORITY_CATEGORY_ERROR;break;
    }
}

std::string commons::get_search_layer_names(Search_Layers l)
{
    switch (l)
    {
        case S_CONTROLLER:
            return "Controller";break;
        case S_RESOURCE:
            return "Resource";break;
        case S_PHYSICAL:
            return "Physical";break;
        default:
            return "Error"; break;
    }
}

std::string commons::get_search_type_name(Search_Types t)
{
    switch (t)
    {
        case TIMING:
            return "Timing"; break;
        case PROVA:
            return "Stocazzo";break;
        default:
            return "Error"; break;
    }
}


/*Custom_Vertex::Custom_Vertex(const Custom_Vertex& CV)
{
    switch (CV.layer)
    {
        case FUNCTION:
                {
                    First_Level_Vertex();
                    this->layer = FUNCTION;
                    this->name = CV.name;
                    break;
                }
                case TASK:
                {
                    this = Second_Level_Vertex();
                    this->layer = TASK;
                    this->name = CV.name;
                    break;
                }    
                case CONTROLLER:
                {
                    Third_Level_Vertex vtx = Third_Level_Vertex();
                    vtx.OS_scheduler_type= reinterpret_cast<const Third_Level_Vertex&>(CV).OS_scheduler_type;
                    vtx.priority_slots = std::map<int,Scheduler_Slot>();
                    for(std::map<int,Scheduler_Slot>::const_iterator it = reinterpret_cast<const Third_Level_Vertex&>(CV).priority_slots.cbegin();it != reinterpret_cast<const Third_Level_Vertex&>(CV).priority_slots.cend();++it)
                        {
                            Scheduler_Slot s;
                            s.id = (*it).second.id;
                            s.pr = (*it).second.pr;
                            vtx.priority_slots.insert(std::make_pair((*it).first,s));
                        }
                    vtx.name = CV.name;    
                    return vtx;
                    break;
                }
                case RESOURCE:
                {
                    Fourth_Level_Vertex vtx = Fourth_Level_Vertex();
                    vtx.ports_map = std::map<int,Port>();
                    for(std::map<int,Port>::iterator it = static_cast<Fourth_Level_Vertex>(CV).ports_map.begin();it != static_cast<Fourth_Level_Vertex>(CV).ports_map.end();++it)
                        {
                            Port p;
                            p.is_master = (*iter).second.is_master;
                            p.id = (*iter).second.id;
                            p.associated_port_id =(*iter).second.associated_port_id;
                            p.priority = (*iter).second.priority;
                            vtx.ports_map.insert(std::make_pair((*iter).first,p));
                        }
                    vtx.component_type=static_cast<Fourth_Level_Vertex>(CV).component_type;
                    vtx.component_priority_type=static_cast<Fourth_Level_Vertex>(CV).component_priority_type;
                    vtx.name = CV.name;
                    this = vtx;
                    break;
                }
                case PHYSICAL:
                {
                    this = Fifth_Level_Vertex();
                    this->layer = PHYSICAL;
                    this->name = CV.name;
                    break;
                }
                default:
                    throw std::runtime_error("copy constructor without layer");
                    break;
        
        
    }
}

*/

void First_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) const {
    PRINT_DEBUG("timing first level vertex building");
    timing_vertex_t vt = boost::add_vertex(graph);
    PRINT_DEBUG("vertex added");
    graph[vt].layer = layer;
    PRINT_DEBUG("layer added"+boost::lexical_cast<std::string>(layer));
    graph[vt].name = this->name;
    PRINT_DEBUG("everything ok");
    
}

void Second_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) const {
    timing_vertex_t vt = boost::add_vertex(graph);
    
    graph[vt].layer = this->layer;
    PRINT_DEBUG("layer added"+boost::lexical_cast<std::string>(layer));
    graph[vt].name = this->name;
}

void Third_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map< std::string, std::map< int, std::string > >& components_map) const
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
            timing_vertex_t vtx_vect[PRIORITY_ENUM_SIZE];
            std::map< int, std::string > tmp;    //port, new vtx name, inner map of the components_map parameter
            for(int i = 0; i < PRIORITY_ENUM_SIZE; i++) 
            {
                timing_vertex_t vt =  boost::add_vertex(graph);
                graph[vt].layer=this->layer;
                graph[vt].name=this->name+"$$"+boost::lexical_cast<std::string>(i);
                vtx_vect[i] = vt;
                tmp.insert(std::make_pair(i,graph[vt].name));
                PRINT_DEBUG(graph[vt].name);
            if (i!= 0)
            {
                timing_edge_t e; bool b;
                boost::tie(e,b) = boost::add_edge(vt,vtx_vect[i-1],graph);
            }
          }
        components_map.insert(std::make_pair(this->name, tmp));  
        break;
        }
        case TDMA:
        {
            std::map< int, std::string > tmp;    //port, new vtx name, inner map of the components_map parameter
            for (std::map<int, Scheduler_Slot>::const_iterator iter = this->priority_slots.begin();iter != this->priority_slots.end() ;++iter)
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

void Fourth_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map< std::string, std::map< int, std::string > >& components_map) const
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
            for (std::map<int, Port>::const_iterator iter = this->ports_map.begin();iter != this->ports_map.end() ;++iter)
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
            PRINT_DEBUG("4th level priority: portsmap size is: "+boost::lexical_cast<std::string>(this->ports_map.size()));
            for (std::map<int, Port>::const_iterator iter = this->ports_map.begin();iter != this->ports_map.end() ;++iter)
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
            bool flag = false;
            timing_vertex_t tmp_vtx;
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
            for(std::vector<timing_vertex_t>::iterator slave_iter = slaves.begin(); slave_iter != slaves.end(); ++slave_iter)
            {
                timing_edge_t e; bool b;
                boost::tie(e,b) = boost::add_edge(tmp_vtx,*slave_iter,graph); //monodirectional masters to slaves
            }
            components_map.insert(std::make_pair(this->name, tmp));  
            break;
        }
        case TDMA:
        {
            std::map< int, std::string > tmp;    //port, new vtx name, inner map of the components_map parameter
            std::vector<timing_vertex_t> masters,slaves;
            for (std::map<int, Port>::const_iterator iter = this->ports_map.begin();iter != this->ports_map.end() ;++iter)
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
            PRINT_DEBUG("error in transformation of layer 3: no priority type compatible");
            break;
            //TODO error handling
        }
            
        
    }

}

void Fifth_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map< std::string, std::map< int, std::string > >& components_map) const
{
    timing_vertex_t vt = boost::add_vertex(graph);
    graph[vt].name = this->name;
    graph[vt].layer = this->layer;
    
}