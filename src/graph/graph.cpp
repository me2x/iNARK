#include "graph.hpp"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
// Visitor that throw an exception when finishing the destination vertex





using namespace commons;

bool source_graph::create_graph_from_xml(std::string xml)
{
    using boost::property_tree::ptree;
    ptree pt;
    read_xml(xml,pt);
    // create a typedef for the Source_Graph type
    // Source_Graph g;
        BOOST_FOREACH(ptree::value_type &v,pt.get_child("root.components"))
        {
            
            switch (int_to_Layer((v.second.get_child("layer")).get_value<int>()))
            {
                case FUNCTION:
                {
                    std::string name = (v.second.get_child("name")).get_value<std::string>();
                    add_L1_node(name);  
                    break;
                }
                case TASK:
                {
                    std::string name = (v.second.get_child("name")).get_value<std::string>();
                    add_L2_node(name); 
                    break;
                }    
                case CONTROLLER:
                {
                    Component_Priority_Category OS_scheduler_type;
                    if(v.second.get_child_optional("priority_handling"))
                    {
                        OS_scheduler_type=int_To_Priority_Handler(v.second.get_child("priority_handling").get_value<int>());
                    }
                    else
                    {
                        PRINT_DEBUG("error no priority handling in os");
                        OS_scheduler_type= PRIORITY_CATEGORY_ERROR;
                        //TODO error handling
                    }
                    std::shared_ptr< std::map<int,Scheduler_Slot> > priority_slots(new std::map<int,Scheduler_Slot>());
                        BOOST_FOREACH(ptree::value_type &i_v,v.second.get_child("slots"))
                        {
                            Scheduler_Slot s;
                            s.id = i_v.second.get_child("id").get_value<int>();
                            s.pr = int_to_Priority(i_v.second.get_child("priority").get_value<int>());
                            priority_slots->insert(std::make_pair(i_v.second.get_child("id").get_value<int>(),s));
                        }
                    std::string name = (v.second.get_child("name")).get_value<std::string>();   
                    add_L3_node(name,priority_slots,OS_scheduler_type);
                    break;
                }
                case RESOURCE:
                {
                    std::shared_ptr<std::map<int,Port>> ports_map (new std::map<int,Port>());
                    if(v.second.get_child_optional("ports"))
                    {
                        BOOST_FOREACH(ptree::value_type &i_v,v.second.get_child("ports"))
                        {
                            Port p;
                            p.is_master = i_v.second.get_child("isMaster").get_value<int>()==1? true : false;
                            p.id = i_v.second.get_child("id").get_value<int>();
                            p.associated_port_id =i_v.second.get_child_optional("associatedPort")? i_v.second.get_child("associatedPort").get_value<int>():NO_PORT;
                            p.priority = i_v.second.get_child_optional("priority")?i_v.second.get_child("priority").get_value<int>():DEFAULT_PRIORITY;
                            ports_map->insert(std::make_pair(i_v.second.get_child("id").get_value<int>(),p));
                        }
                    }
                    else
                    {
                        PRINT_DEBUG("error no ports in a component");
                        //TODO error handling
                    }
                    Component_Type component_type;
                    if(v.second.get_child_optional("type"))
                    {
                        component_type=int_To_Type(v.second.get_child("type").get_value<int>());
                    }
                    else
                    {
                        PRINT_DEBUG("error no type in component: "+ v.second.get_child("name").get_value<std::string>());
                        component_type = TYPE_ERROR;
                        //TODO error handling
                    }
                    Component_Priority_Category component_priority_type;
                    if(v.second.get_child_optional("priority_handling"))
                    {
                        component_priority_type=int_To_Priority_Handler(v.second.get_child("priority_handling").get_value<int>());
                    }
                    else throw std::runtime_error("no priority handling in 4th level");
                    
                    std::string name = (v.second.get_child("name")).get_value<std::string>();
                    add_L4_node(name,ports_map,component_type,component_priority_type); 
                    
                    break;
                }
                case PHYSICAL:
                {
                    std::string name = (v.second.get_child("name")).get_value<std::string>();
                    add_L5_node(name); 
                    break;
                }
                
                
                
            }
            
            
            
           
             
           
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
            PRINT_DEBUG("from component is: "+(*local_graph)[from_node].name+" and its port is: "+boost::lexical_cast<std::string>(from_port));
            PRINT_DEBUG("to  component is: "+(*local_graph)[to_node].name+" and its port is: "+boost::lexical_cast<std::string>(to_port));
            PRINT_DEBUG("-----");
            edge_t e; bool b;
            //build edge(s). per qualsiasi caso entrambi tranne che se di 4th livello, in tal caso solo andata.
            boost::tie(e,b) = boost::add_edge(from_node,to_node,(*local_graph));
            (*local_graph)[e].from_port = from_port;
            (*local_graph)[e].to_port = to_port;
            if ((*local_graph)[from_node].layer != RESOURCE || (*local_graph)[to_node].layer != RESOURCE)
            {
                boost::tie(e,b) = boost::add_edge(to_node,from_node,(*local_graph));
                (*local_graph)[e].from_port = to_port;
                (*local_graph)[e].to_port = from_port;
            }


        }

// DOES NOT HANDLES DOUBLE IN XML, creates a different vertex/edge.
        //TODO handling of doubles.
        
        
          std::ofstream myfile;
  myfile.open ("/home/emanuele/Documents/tmp_graph/source_graph.dot");
  boost::write_graphviz(myfile, *local_graph,make_vertex_writer(boost::get(&Custom_Vertex::layer, *local_graph),boost::get (&Custom_Vertex::name, *local_graph))
      ,make_edge_writer(boost::get(&Custom_Edge::from_port,*local_graph),boost::get(&Custom_Edge::to_port,*local_graph))
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

void source_graph::add_L1_node(std::string name)
{
    vertex_t vt = boost::add_vertex(*local_graph);
    // it has to be done this way. changing to (*local_graph)[vt] = First_Level_Vertex(); and working with the local_graph[vt] is not a viable option. returns a bad function call.
    First_Level_Vertex vtx = First_Level_Vertex();
    vtx.layer = FUNCTION;
    vtx.name = name;
    (*local_graph)[vt] = vtx;
    (*local_graph)[vt].add_function(vtx);  
    vertex_map.insert(std::make_pair(name, vt));
}
void source_graph::add_L2_node(std::string name)
{
    vertex_t vt = boost::add_vertex(*local_graph);
    Second_Level_Vertex vtx = Second_Level_Vertex();
    vtx.layer = TASK;
    vtx.name = name;
    (*local_graph)[vt] = vtx;
    (*local_graph)[vt].add_function(vtx);  
    vertex_map.insert(std::make_pair(name, vt));
}
void source_graph::add_L3_node(std::string name, std::shared_ptr< std::map< int, Scheduler_Slot > > scheduler, Component_Priority_Category handling)
{
    vertex_t vt = boost::add_vertex(*local_graph);
    Third_Level_Vertex vtx = Third_Level_Vertex();
    vtx.layer = CONTROLLER;
    vtx.name = name;
    vtx.OS_scheduler_type = handling;
    vtx.priority_slots = scheduler; //not sure it works, dovrebbe essendo shared_ptr.
    (*local_graph)[vt] = vtx;
    (*local_graph)[vt].add_function(vtx); //potrebbe non andare. 
    //code here
    
    
     vertex_map.insert(std::make_pair(name, vt));
}
void source_graph::add_L4_node(std::string name, std::shared_ptr< std::map< int, Port > > ports, Component_Type type, Component_Priority_Category handling)
{
    vertex_t vt = boost::add_vertex(*local_graph);
    Fourth_Level_Vertex vtx = Fourth_Level_Vertex();
    vtx.layer = RESOURCE;
    vtx.name = name;
    vtx.component_priority_type = handling;
    vtx.ports_map = ports; //not sure it works
    vtx.component_type = type;
    (*local_graph)[vt] = vtx;
    (*local_graph)[vt].add_function(vtx); //potrebbe non andare. 
    //code here
    //code here
    
     vertex_map.insert(std::make_pair(name, vt));
}
void source_graph::add_L5_node(std::string name)
{
    vertex_t vt = boost::add_vertex(*local_graph);
    Fifth_Level_Vertex vtx = Fifth_Level_Vertex();
    vtx.layer = PHYSICAL;
    vtx.name = name;
    (*local_graph)[vt] = vtx;
    (*local_graph)[vt].add_function(vtx); //potrebbe non andare. 
    vertex_map.insert(std::make_pair(name, vt));
}
bool source_graph::create_graph(std::string xml_location)
{

}
//usare weak pointers? nel senso che l'idea è di avere sola lettura. weak non puo risalire xo. 
//quindi dovrebbe ricreare il shared_ptr a valle...nn ha molto senso...
std::shared_ptr< Source_Graph > source_graph::get_source_graph_ref()
{
    std::shared_ptr< Source_Graph > return_value = local_graph;
    return return_value;
}
