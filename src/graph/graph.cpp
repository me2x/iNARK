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

bool custom_graph::create_graph(std::string xml)
{
       using boost::property_tree::ptree;
       ptree pt;
       read_xml("/home/emanuele/iNARK/spec.xml",pt);
       // create a typedef for the Source_Graph type
       // Source_Graph g;
           BOOST_FOREACH(ptree::value_type &v,pt.get_child("root.components"))
          {
              vertex_t vt = boost::add_vertex(local_graph);
              local_graph[vt].layer=int_to_Layer((v.second.get_child("layer")).get_value<int>());
              local_graph[vt].name=(v.second.get_child("name")).get_value<std::string>();
              if(v.second.get_child_optional("ports"))
	      {
                local_graph[vt].ports = std::map<int,int>();
		BOOST_FOREACH(ptree::value_type &i_v,v.second.get_child("ports"))
                {
		  local_graph[vt].ports.insert(std::make_pair<int,int>(i_v.second.get_child("id").get_value<int>(),i_v.second.get_child("priority").get_value<int>()));
		}
              }
              else
	      {
		local_graph[vt].ports = std::map<int,int>();
	      }
              if(v.second.get_child_optional("type"))
	      {
		local_graph[vt].type=int_To_Type(v.second.get_child("type").get_value<int>());
              }
              else
              {
                local_graph[vt].type=BUS;//TODO tmp da eliminare
              }
              if(v.second.get_child_optional("priority_handling"))
	      {
		local_graph[vt].priority_category=int_To_Priority_Handler(v.second.get_child("priority_handling").get_value<int>());
              }
              else
	      {
		local_graph[vt].priority_category=ROUND_ROBIN;
	      }
              vertex_map.insert(std::make_pair(local_graph[vt].name, vt));
              PRINT_DEBUG(local_graph[vt].layer);
          }//m_modules.insert(v.second.data());
           BOOST_FOREACH(ptree::value_type &v,pt.get_child("root.edges"))
          {
               edge_t e; bool b;
               boost::tie(e,b) = boost::add_edge(vertex_map.at(v.second.get_child("from.name").get_value<std::string>()),vertex_map.at(v.second.get_child("to.name").get_value<std::string>()),local_graph);
               if (v.second.get_child_optional("priority")){
                   local_graph[e].priority=int_to_Priority(v.second.get_child("priority").get_value<int>());
               }
               else
               {
                   local_graph[e].priority=SAFETY_CRITICAL;
               }
               if (v.second.get_child_optional("to.port")){
		   local_graph[e].to_port.component_port =  v.second.get_child("to.port").get_value<int>();
		   local_graph[e].to_port.component_name =  v.second.get_child("to.name").get_value<std::string>();
               }
               else
	       {
		 local_graph[e].to_port.component_port = 0;
		 local_graph[e].to_port.component_name =  v.second.get_child("to.name").get_value<std::string>();
	       }
	       if (v.second.get_child_optional("from.port")){
		   local_graph[e].from_port.component_port =  v.second.get_child("from.port").get_value<int>();
		   local_graph[e].from_port.component_name =  v.second.get_child("from.name").get_value<std::string>();
               }
               else
	       {
		 local_graph[e].from_port.component_port = 0;
		 local_graph[e].from_port.component_name =  v.second.get_child("from.name").get_value<std::string>();
	       }

           }

// DOES NOT HANDLES DOUBLE IN XML, creates a different vertex/edge.
           //TODO handling of doubles.
           return true;
}

//true -> a path exists, false does not.
bool custom_graph::search_component_dependences(std::string from, std::string to){

return false;
}



const Source_Graph custom_graph::get_graph(){
    return local_graph;
}
