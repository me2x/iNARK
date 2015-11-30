#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "iNARKConfig.h"

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

struct xml_data{
    std::set<Custom_Vertex> vertexes;
    std::set<Custom_Edge> edges;
};


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
              vertex_map.insert(std::make_pair(local_graph[vt].name, vt));
              std::cout<<local_graph[vt].layer<<std::endl;
          }//m_modules.insert(v.second.data());
           BOOST_FOREACH(ptree::value_type &v,pt.get_child("root.edges"))
          {
               edge_t e; bool b;
               boost::tie(e,b) = boost::add_edge(vertex_map.at(v.second.get_child("from").get_value<std::string>()),vertex_map.at(v.second.get_child("to").get_value<std::string>()),local_graph);
               local_graph[e].priority=int_to_Priority(v.second.get_child("priority").get_value<int>());
          }
// DOES NOT HANDLES DOUBLE IN XML, creates a different vertex/edge.
           return true;
}

//true -> a path exists, false does not
bool custom_graph::filter_and_explore_graph(Priority p,std::string from, std::string to){


const char* name = "0123456789";
       boost::filtered_graph<Graph, custom_edge_predicate_c> fg(local_graph,custom_edge_predicate_c(local_graph,p));
       std::cout<<"fg edges number is: ";boost::print_edges(fg,name);

    std::cout<<vertex_map.at(from);
    my_visitor vis = my_visitor(vertex_map.at(to));
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

    /*for(int i = 0; i<=5; i++)
    {
        vertex_t v = boost::add_vertex(g);
        g[v].layer=int_to_Layer(i+1);
        std::cout<<g[v].layer<<std::endl;
    }*/
   /*for (int i= 0; i<5; i++)
    {
        edge_t e; bool b;
        boost::tie(e,b) = boost::add_edge(boost::vertex(i,g),vertex(i+1,g),g);
        g[e].priority=MISSION_CRITICAL;
    }
        // get the property map for vertex indices
          typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
          IndexMap index = get(boost::vertex_index, g);

          std::cout << "vertices(g) = ";
          typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
          std::pair<vertex_iter, vertex_iter> vp;
          for (vp = vertices(g); vp.first != vp.second; ++vp.first)
          {
              vertex_t ver = *vp.first;
              std::cout<<Layer_to_String(g[ver].layer)<<std::endl;
          }


              //std::cout << index[*vp.first] <<  " ";
          std::cout << std::endl;
          boost::graph_traits<Graph>::edge_iterator ei, ei_end;
              for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
              {
                  edge_t ed = *ei;
                  std::cout<<g[ed].priority<<std::endl;
                  std::cout<<g[source(ed,g)].name<<" to "<<g[target(ed,g)].name<<std::endl;
              }
*/


