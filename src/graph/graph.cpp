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
        return PRIORITY_ERROR;
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
        return ROUND_ROBIN;break;

    case 2:
        return PRIORITY;break;

    case 3:
        return TDMA;break;

    default:
        return TYPE_ERROR;break;
    }
}



bool custom_graph::create_graph(std::string xml)
{
       using boost::property_tree::ptree;
       ptree pt;
       read_xml("/home/emanuele/iNARK/spec.xml",pt);
       // create a typedef for the Graph type
       // Graph g;
           BOOST_FOREACH(ptree::value_type &v,pt.get_child("root.components"))
          {
              vertex_t vt = boost::add_vertex(local_graph);
              local_graph[vt].layer=int_to_Layer((v.second.get_child("layer")).get_value<int>());
              local_graph[vt].name=(v.second.get_child("name")).get_value<std::string>();
              if(v.second.get_child_optional("ports")){
                  BOOST_FOREACH(ptree::value_type &i_v,v.second.get_child("ports"))
                  {
                      local_graph[vt].ports.insert(std::make_pair<int,Priority>(i_v.second.get_child("id").get_value<int>(),int_to_Priority(i_v.second.get_child("priority").get_value<int>())));
                  }
              }
              if(v.second.get_child_optional("type")){
                  local_graph[vt].type=int_To_Type(v.second.get_child("type").get_value<int>());
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
                   local_graph[e].priority=int_to_Priority(NO_PRIORITY);
               }
               if (v.second.get_child_optional("to.port")){
                edge_to_port_map.insert(std::make_pair<edge_t,int>(e,v.second.get_child("to.port").get_value<int>()));
               }


           }

// DOES NOT HANDLES DOUBLE IN XML, creates a different vertex/edge.
           //TODO handling of doubles.
           return true;
}

//true -> a path exists, false does not.
bool custom_graph::search_component_dependences(std::string from, std::string to){


#ifdef DEBUG
    const char* name = "0123456789abcdefghilmnopqrstuvz";
#endif
    boost::filtered_graph<Graph,extern_edge_predicate_c ,extern_vertex_predicate_c> fg(local_graph,extern_edge_predicate_c(local_graph),extern_vertex_predicate_c(local_graph));
       PRINT_DEBUG("fg edges number is: ");
#ifdef DEBUG
       boost::print_edges(fg,name);
#endif
    PRINT_DEBUG(vertex_map.at(from));
    extern_visitor vis = extern_visitor(vertex_map.at(to));
    try {
      boost::breadth_first_search(
        fg, vertex_map.at(from),boost::visitor(vis)
      );
    }
    catch (int exception) {
      return true;
    }
return false;
}



const Graph custom_graph::get_graph(){
    return local_graph;
}
