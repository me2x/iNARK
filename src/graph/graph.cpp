#include "graph.h"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
// Visitor that throw an exception when finishing the destination vertex



Layer int_to_Layer(int i){
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
Priority int_to_Priority(int i){
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
std::string Layer_to_String(Layer l){
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
Component_Type int_To_Type(int i){
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
Component_Priority_Category int_To_Priority_Handler(int i){
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

bool source_graph::create_graph(std::string xml)
{
    using boost::property_tree::ptree;
    ptree pt;
    read_xml("/home/emanuele/iNARK/spec.xml",pt);
    // create a typedef for the Source_Graph type
    // Source_Graph g;
        BOOST_FOREACH(ptree::value_type &v,pt.get_child("root.components"))
        {
            vertex_t vt = boost::add_vertex(local_graph);
            switch (int_to_Layer((v.second.get_child("layer")).get_value<int>()))
            {
                case FUNCTION:
                {
                    local_graph[vt] = First_Level_Vertex();
                    break;
                }
                case TASK:
                {
                    local_graph[vt] = Second_Level_Vertex();
                    break;
                }    
                case CONTROLLER:
                {
                    local_graph[vt] = Third_Level_Vertex();
                    if(v.second.get_child_optional("priority_handling"))
                    {
                        local_graph[vt].OS_scheduler_type=int_To_Priority_Handler(v.second.get_child("priority_handling").get_value<int>());
                    }
                    else
                    {
                        error;
                    }
                    
                    break;
                }
                case RESOURCE:
                {
                    local_graph[vt] = Fourth_Level_Vertex();
                    if(v.second.get_child_optional("ports"))
                    {
                        local_graph[vt].ports = std::map<int,Port>();
                        BOOST_FOREACH(ptree::value_type &i_v,v.second.get_child("ports"))
                        {
                            Port p;
                            p.is_master = leggi;
                            p.id = i_v.second.get_child("id").get_value<int>();
                            p.associated_port_id = NO_PORT;
                            p.priority = i_v.second.get_child_optional("priority")?i_v.second.get_child("priority").get_value<int>():DEFAULT_PRIORITY;
                            local_graph[vt].ports.insert(std::make_pair<int,int>(i_v.second.get_child("id").get_value<int>(),p);
                        }
                    }
                    else
                    {
                        error;
                    }
                    
                    if(v.second.get_child_optional("type"))
                    {
                        local_graph[vt].Component_Type=int_To_Type(v.second.get_child("type").get_value<int>());
                    }
                    else
                    {
                        error;
                    }
                    
                    if(v.second.get_child_optional("priority_handling"))
                    {
                        local_graph[vt].Component_Priority_Category=int_To_Priority_Handler(v.second.get_child("priority_handling").get_value<int>());
                    }
                    else
                    {
                        error;
                    }
                
                    break;
                }
                case PHYSICAL:
                {
                    local_graph[vt] = Fifth_Level_Vertex();
                    break;
                }
                
                
                
            }
            local_graph[vt].name=(v.second.get_child("name")).get_value<std::string>();
            
            
           
            
            vertex_map.insert(std::make_pair(local_graph[vt].name, vt));
            PRINT_DEBUG(local_graph[vt].layer);
        }//m_modules.insert(v.second.data());
        BOOST_FOREACH(ptree::value_type &v,pt.get_child("root.edges"))
        {
            //parse data
            vertex_t from_node, to_node;
            int from_port,to_port;
            from_node = vertex_map.at(v.second.get_child("from.name").get_value<std::string>());
            to_node = vertex_map.at(v.second.get_child("to.name").get_value<std::string>());
            from_port = (v.second.get_child_optional("from.port"))? v.second.get_child("from.port").get_value<int>() : NO_PORT;
            to_port = (v.second.get_child_optional("to.port"))? v.second.get_child("to.port").get_value<int>() : NO_PORT;
            PRINT_DEBUG("from component is: "+local_graph[from_node].name+" and its port is: "+boost::lexical_cast<std::string>(from_port));
            PRINT_DEBUG("to  component is: "+local_graph[to_node].name+" and its port is: "+boost::lexical_cast<std::string>(to_port));
            PRINT_DEBUG("-----");
            edge_t e; bool b;
            //build edge(s) sempre entrambi.
            boost::tie(e,b) = boost::add_edge(from_node,to_node,local_graph);
            local_graph[e].from_port = from_port;
            local_graph[e].to_port = to_port;
            
            boost::tie(e,b) = boost::add_edge(to_node,from_node,local_graph);
            local_graph[e].from_port = to_port;
            local_graph[e].to_port = from_port;



        }

// DOES NOT HANDLES DOUBLE IN XML, creates a different vertex/edge.
        //TODO handling of doubles.
        
        
          std::ofstream myfile;
  myfile.open ("/home/emanuele/Documents/tmp_graph/source_graph.dot");
  boost::write_graphviz(myfile, local_graph,make_vertex_writer(boost::get(&Custom_Vertex::layer, local_graph),boost::get (&Custom_Vertex::name, local_graph),boost::get(&Custom_Vertex::ports, local_graph), boost::get(&Custom_Vertex::type,local_graph ), boost::get(&Custom_Vertex::priority_category,local_graph))
      ,make_edge_writer(boost::get(&Custom_Edge::from_port,local_graph),boost::get(&Custom_Edge::to_port,local_graph))
      //boost::make_label_writer(boost::get(&Custom_Edge::priority,local_graph))
);
  myfile.close();
        return true;
}
#if 0
//true -> a path exists, false does not.
bool custom_graph::search_component_dependences(std::string from, std::string to){

return false;
}
#endif


const Source_Graph source_graph::get_graph(){
    return local_graph;
}
