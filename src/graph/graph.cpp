#include "graph.hpp"

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
// Visitor that throw an exception when finishing the destination vertex

#include <memory>



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
            vertex_t vt = boost::add_vertex(*local_graph);
            Custom_Vertex vtx = Custom_Vertex();
            switch (int_to_Layer((v.second.get_child("layer")).get_value<int>()))
            {
                case FUNCTION:
                {
                    std::string name = (v.second.get_child("name")).get_value<std::string>();
                    vtx.create_L1_child(name);  
                    break;
                }
                case TASK:
                {
                    std::string name = (v.second.get_child("name")).get_value<std::string>();
                    vtx.create_L2_child(name); 
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
                            s.pr = i_v.second.get_child_optional("priority")?int_to_Priority(i_v.second.get_child("priority").get_value<int>()):Priority::NO_PRIORITY;
                            priority_slots->insert(std::make_pair(i_v.second.get_child("id").get_value<int>(),s));
                        }
                    std::string name = (v.second.get_child("name")).get_value<std::string>();   
                    vtx.create_L3_child(name,priority_slots,OS_scheduler_type);
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
                            auto res = ports_map->insert(std::make_pair(i_v.second.get_child("id").get_value<int>(),p));
                            if (!res.second)
                                throw std::runtime_error("input error: port already exists");
                        }
                    }
                    else
                    {
                        PRINT_DEBUG("error no ports in a component");
                        //TODO error handling
                        throw std::runtime_error("input error: layer 4 component without ports");
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
                    vtx.create_L4_child(name,ports_map,component_type,component_priority_type); 
                    
                    break;
                }
                case PHYSICAL:
                {
                    std::string name = (v.second.get_child("name")).get_value<std::string>();
                    vtx.create_L5_child(name); 
                    break;
                }
                
                
               
            }
            (*local_graph)[vt] = vtx;
            vertex_map.insert(std::make_pair(vtx.get_name(), vt));
            
            
           
             
           
        }
        PRINT_DEBUG("component reading and creation done");
        //m_modules.insert(v.second.data());
        
        
       
        
        
        
        BOOST_FOREACH(ptree::value_type &v,pt.get_child("root.edges"))
        {
            //parse data
            vertex_t from_node, to_node;
            int from_port,to_port;
            from_node = vertex_map.at(v.second.get_child("from.name").get_value<std::string>());
            to_node = vertex_map.at(v.second.get_child("to.name").get_value<std::string>());
            from_port = (v.second.get_child_optional("from.port"))? v.second.get_child("from.port").get_value<int>() : NO_PORT;
            to_port = (v.second.get_child_optional("to.port"))? v.second.get_child("to.port").get_value<int>() : NO_PORT;
            PRINT_DEBUG("from component is: "+(*local_graph)[from_node].get_name()+" and its port is: "+boost::lexical_cast<std::string>(from_port));
            PRINT_DEBUG("to  component is: "+(*local_graph)[to_node].get_name()+" and its port is: "+boost::lexical_cast<std::string>(to_port));
            PRINT_DEBUG("-----");
            edge_t e; bool b;
            //build edge(s). 
            //all intra layer edges are one - way, the inter layer are the mapped-on/ interfere with relations so must be bidirectional
            boost::tie(e,b) = boost::add_edge(from_node,to_node,(*local_graph));
            (*local_graph)[e].from_port = from_port;
            (*local_graph)[e].to_port = to_port;
            if ((*local_graph)[from_node].get_layer() != (*local_graph)[to_node].get_layer())
            {
                boost::tie(e,b) = boost::add_edge(to_node,from_node,(*local_graph));
                (*local_graph)[e].from_port = to_port;
                (*local_graph)[e].to_port = from_port;
            }


        }

// DOES NOT HANDLES DOUBLE IN XML, creates a different vertex/edge.
        //TODO handling of doubles.
        
#if 0     
          std::ofstream myfile;
  myfile.open ("/home/emanuele/Documents/tmp_graph/source_graph.dot");
  boost::write_graphviz(myfile, *local_graph,make_vertex_writer(boost::get( &Custom_Vertex::layer, *local_graph),boost::get (&get_name(), *local_graph))
      ,make_edge_writer(boost::get(&Custom_Edge::from_port,*local_graph),boost::get(&Custom_Edge::to_port,*local_graph))
      //boost::make_label_writer(boost::get(&Custom_Edge::priority,local_graph))
);
  myfile.close();
#endif
#ifdef SEM_CHECK
//make some checks on the semantic of the graph (i.e. ports are monodirectional, no edges skip a layer, ...)

//forall components l == 4; get outgoing and ingoing edges: check ports. if one port is on more than one edge in different directions throw error. 
/// voglio mantenere piu archi, significano design decisions not yet taken
/// si può fare solo ciclando su archi, magari 2 volte. ma non credo serva, costruisco struttura dati comp : port : in/out nel momento in cui viene incontrata. 
/// se poi esiste gia, check direzione sia uguale else error 
//alternative (forse meglio): slave port -> exit -> possono essere solo from. master port -> entry point -> solo to. 
//ciclo su edge: from: get component-> get port ->is master == false passed, else throw error.
//opposto per master.



//forall edges: if to > from+1 || to < from throw error  \\\ ovvero puo esseree solo uguale o maggiore di uno.
edge_iter ei, ei_end;
for (boost::tie(ei, ei_end) = boost::edges(*local_graph); ei != ei_end; ++ei)
    {
        vertex_t old_graph_source,old_graph_target;
        old_graph_source = boost::source(*ei,*local_graph);
        old_graph_target = boost::target(*ei,*local_graph);
        PRINT_DEBUG("the source component is: "+(*local_graph)[old_graph_source].get_name()+" and its port is: "+boost::lexical_cast<std::string>((*local_graph)[*ei].from_port));
        PRINT_DEBUG("the target component is: "+(*local_graph)[old_graph_target].get_name()+" and its port is: "+boost::lexical_cast<std::string>((*local_graph)[*ei].to_port));

        if (std::abs((*local_graph)[old_graph_target].get_layer() - (*local_graph)[old_graph_source].get_layer())>1)
            throw std::runtime_error("Input error: edge is skipping layers");
        if ((*local_graph)[old_graph_target].get_layer() == Layer::RESOURCE &&  (*local_graph)[old_graph_source].get_layer()== Layer::RESOURCE)
        {
            std::shared_ptr<Fourth_Level_Vertex> target_ptr = (*local_graph)[old_graph_target].get_shared_ptr_l4();
            std::shared_ptr<Fourth_Level_Vertex> source_ptr = (*local_graph)[old_graph_source].get_shared_ptr_l4();
            if (!(target_ptr->ports_map->at((*local_graph)[*ei].to_port).is_master))
                throw std::runtime_error("Input error: entry port "+boost::lexical_cast<std::string>((*local_graph)[*ei].to_port) + " is not master of the component "+(*local_graph)[old_graph_target].get_name());
             if ((source_ptr->ports_map->at((*local_graph)[*ei].from_port).is_master))
                throw std::runtime_error("Input error: exit port "+boost::lexical_cast<std::string>((*local_graph)[*ei].from_port) + " is not slave of the component "+(*local_graph)[old_graph_source].get_name());
        }
    }

//components have different names. eeppero questo mi viene "gratis" poi, quando uso il nome come chiave di mappa: basterebbe mettere il controllo là.
//ora come ora crea entrambi e "first come first serve"
std::pair<vertex_iter, vertex_iter> vp;
std::set<std::string> names;
    for (vp = boost::vertices(*local_graph); vp.first != vp.second; ++vp.first)
    {
        if (names.find((*local_graph)[*vp.first].get_name())!= names.end())
            throw std::runtime_error("Input error: two components with the same name");
        else
            names.insert((*local_graph)[*vp.first].get_name());
    }

#endif



  return true;
}


std::shared_ptr< Source_Graph > source_graph::get_source_graph_ref()
{
    std::shared_ptr< Source_Graph > return_value = local_graph;
    return return_value;
}
