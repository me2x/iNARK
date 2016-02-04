#include "graph_common.hpp"


void First_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) const {
    timing_vertex_t vt = boost::add_vertex(graph);
    graph[vt].name = this->name;
    graph[vt].layer = this->layer;
    
}

void Second_Level_Vertex::explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) const {
    timing_vertex_t vt = boost::add_vertex(graph);
    graph[vt].name = this->name;
    graph[vt].layer = this->layer;
    
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
            for (std::map<int, Scheduler_Slot>::iterator iter = this->priority_slots.begin();iter != this->priority_slots.end() ;++iter)
            {
                timing_vertex_t vt =  boost::add_vertex(graph);
                graph[vt].layer=this->layer;
                graph[vt].name=this->name+"$$"+boost::lexical_cast<std::string>(*iter->second.id);
                tmp.insert(std::make_pair((*iter).second.id,graph[vt].name));
                PRINT_DEBUG(graph[vt].name);
            }
        components_map.insert(std::make_pair(this->name, tmp));  
        break;
        }
        default
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
    switch (this->component_priority_type)
    {
        case ROUND_ROBIN:
        {
            std::map< int, std::string > tmp;    //port, new vtx name, inner map of the components_map parameter
            std::vector<timing_vertex_t> masters,slaves;
            for (std::map<int, Port>::iterator iter = this->ports_map.begin();iter != this->ports_map.end() ;++iter)
            {
                
                if ((*iter).second.is_master)
                {
                    timing_vertex_t vt =  boost::add_vertex(graph);
                    Fourth_Level_Timing_Node node = Fourth_Level_Timing_Node();
                    graph[vt] = node;
                    node.layer=this->layer;
                    node.name=this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.id);
                    (*iter).second.associated_port_id == NO_PORT ? node.associate_port_name = "" : node.associate_port_name = this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.associated_port_id);
                    node.is_master = true;
                    
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
                    Fourth_Level_Timing_Node node = Fourth_Level_Timing_Node();
                    graph[vt] = node;
                    node.layer=this->layer;
                    node.name=this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.id);
                    (*iter).second.associated_port_id == NO_PORT ? node.associate_port_name = "" : node.associate_port_name = this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.associated_port_id);
                    node.is_master = false;
                
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
            for (std::map<int, Port>::iterator iter = this->ports_map.begin();iter != this->ports_map.end() ;++iter)
            {
                if ((*iter).second.is_master)
                {
                    timing_vertex_t vt =  boost::add_vertex(graph);
                    Fourth_Level_Timing_Node node = Fourth_Level_Timing_Node();
                    graph[vt] = node;
                    node.layer=this->layer;
                    node.name=this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.id);
                    (*iter).second.associated_port_id == NO_PORT ? node.associate_port_name = "" : node.associate_port_name = this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.associated_port_id);
                    node.is_master = true;
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
                    timing_vertex_t vt =  boost::add_vertex(graph);
                    Fourth_Level_Timing_Node node = Fourth_Level_Timing_Node();
                    graph[vt] = node;
                    node.layer=this->layer;
                    node.name=this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.id);
                    (*iter).second.associated_port_id == NO_PORT ? node.associate_port_name = "" : node.associate_port_name = this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.associated_port_id);
                    node.is_master = false;
                    slaves.push_back(vt);
                    tmp.insert(std::make_pair((*iter).second.id,graph[vt].name));
                    PRINT_DEBUG(graph[vt].name);
                }
            }
            //internal edges creation
            for (std::map<int,std::vector< timing_vertex_t> >::reverse_iterator extern_iter = priority_map.rbegin();extern_iter != priority_map.rend();++extern_iter)
            {
                timing_vertex_t tmp;
                for (std::vector<timing_vertex_t>::iterator vertex_iter = (*extern_iter).second.begin();vertex_iter != (*extern_iter).second.end(); ++extern_iter)
                {
                    
                }
            }
            
            
            
            
            components_map.insert(std::make_pair(this->name, tmp));  
            break;
        }
        case TDMA:
        {
            std::map< int, std::string > tmp;    //port, new vtx name, inner map of the components_map parameter
            std::vector<timing_vertex_t> masters,slaves;
            for (std::map<int, Port>::iterator iter = this->ports_map.begin();iter != this->ports_map.end() ;++iter)
            {
                
                if ((*iter).second.is_master)
                {
                    timing_vertex_t vt =  boost::add_vertex(graph);
                    Fourth_Level_Timing_Node node = Fourth_Level_Timing_Node();
                    graph[vt] = node;
                    node.layer=this->layer;
                    node.name=this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.id);
                    (*iter).second.associated_port_id == NO_PORT ? node.associate_port_name = "" : node.associate_port_name = this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.associated_port_id);
                    node.is_master = true;
                    //finalize
                    masters.push_back(vt);
                    tmp.insert(std::make_pair((*iter).second.id,graph[vt].name));
                    PRINT_DEBUG(graph[vt].name);
                }
                else
                {
                    timing_vertex_t vt =  boost::add_vertex(graph);
                    Fourth_Level_Timing_Node node = Fourth_Level_Timing_Node();
                    graph[vt] = node;
                    node.layer=this->layer;
                    node.name=this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.id);
                    (*iter).second.associated_port_id == NO_PORT ? node.associate_port_name = "" : node.associate_port_name = this->name+"$$"+boost::lexical_cast<std::string>((*iter).second.associated_port_id);
                    node.is_master = false;
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
        default:
        {
            PRINT_DEBUG("error in transformation of layer 3: no OS_scheduler_type compatible");
            break;
            //TODO error handling
        }
            
        
    }

}

