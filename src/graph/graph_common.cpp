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
                tmp.insert(std::make_pair(i,graph[vt].name));
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
            for (std::map<int, Port>::iterator iter = this->ports_map.begin();iter != this->ports_map.end() ;++iter)
            {
                ;
            }
            components_map.insert(std::make_pair(this->name, tmp));  
            break;
        }
        case PRIORITY:
        {
            std::map< int, std::string > tmp;    //port, new vtx name, inner map of the components_map parameter
            for (std::map<int, Port>::iterator iter = this->ports_map.begin();iter != this->ports_map.end() ;++iter)
            {
                ;
            }
            components_map.insert(std::make_pair(this->name, tmp));  
            break;
        }
        case TDMA:
        {
            std::map< int, std::string > tmp;    //port, new vtx name, inner map of the components_map parameter
            for (std::map<int, Port>::iterator iter = this->ports_map.begin();iter != this->ports_map.end() ;++iter)
            {
                ;
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

