#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "debug_msg_handler.hpp"
#include <boost/graph/graphviz.hpp>
#include <boost/graph/undirected_dfs.hpp>
#include <boost/graph/graph_utility.hpp>
#include <functional>
#ifndef GRAPH_COMMON_H
#define GRAPH_COMMON_H

#define NO_PORT -1
#define DEFAULT_PRIORITY 0

enum Layer{
    FUNCTION,
    TASK,
    CONTROLLER,
    RESOURCE,
    PHYSICAL,
    LAYER_ERROR
};

enum Priority{
    NO_PRIORITY,
    MISSION_CRITICAL,
    SAFETY_CRITICAL,
    PRIORITY_ENUM_SIZE
};

enum Component_Priority_Category{
    ROUND_ROBIN,
    PRIORITY,
    TDMA,
    PRIORITY_CATEGORY_ERROR
};

enum Component_Type{
  PROCESSOR,
  BUS,
  BRIDGE,
  PERIPHERAL,
  MEMORY,
  NOT_SPECIFIED,
  TYPE_ERROR
};

enum Search_Layers{
    S_CONTROLLER,
    S_RESOURCE,
    S_PHYSICAL,
    SEARCH_LAYER_END
};
enum Search_Types{
    TIMING,
    PROVA,
    SEARCH_TYPE_END
};
class Custom_Vertex;
class Custom_Edge;

namespace commons{
Layer int_to_Layer(int i);
Priority int_to_Priority(int i);
std::string Layer_to_String(Layer l);
Component_Type int_To_Type(int i);
Component_Priority_Category int_To_Priority_Handler(int i);
std::string get_search_layer_names(Search_Layers l);
std::string get_search_type_name(Search_Types t);
    
}
//timing node cannot be forward declared because of graph library: incomplete base
class Timing_Node{// subclass removed: when doing the search the graph is passed by copy and the runtime properties like subclasses are lost.
public:
    std::string name;
    Layer layer; //serve reitrodurlo per fermare la ricerca al terzo o quarto o quinto livello e per togliere edges 2 to 3 in 2nd phase of preparation: task propagation 
    Component_Type type = NOT_SPECIFIED;
    std::string associate_port_name = ""; //default vuota. serve solo al 4th livello per le porte master&slave. NB non sono la slave che diventa master da un componente all altro ma la doppia porta che garantisce bidirezionalita
    std::set<std::string> master_tasks; //vettore che viene riempito in task propagation
    bool is_master = false; //tiene traccia se un nodo rappresenta una porta master o una slave. ha senso solo al 4th lvl.
};



typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,Custom_Vertex,Custom_Edge>Source_Graph;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,boost::property<boost::vertex_color_t, boost::default_color_type, Timing_Node> > Timing_Graph; //edge custom non serve piu.

/*** 
 * this class is a common interface.
 * but it has to be instantiable in order to work with boost (don't know if the issue was due to the write graphwiz or the graph library, but if is not able to instantiate is not able to compile.
 * every single object is then modified with a pointer to a child class, which will actually contain the correct explode_component function tailored to the correct layer. 
 * for now only the timing graph is supported and the only possible eexplosion iis the timing one.
 * the lambda is needed: the graph is passed by copy and the runtime (aka child) information is lost. 
 * with this workaround the function pointer is modified with the function pointer of the child class, and when calling the father is calling the correct child one
*/

class Custom_Vertex
{
public:
    std::string name;
    Layer layer;
    std::function<void(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map )> explode_component_timing;// ; //<old_comp.id <old_comp.port new_comp.id> >
    template <class T>
    void add_function (T& elem)
    { 
        explode_component_timing = [elem] (Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) {elem.explode_component_timing(graph, components_map);};
    }
    
};

class Custom_Edge
{
public:
    int to_port;
    int from_port;
};

// non ha sensooooo: tanto i tdma sono vertex.incoming_edges(), i priority sono PRIORITY_ENUM_SIZE, i RR sono 1.
// basta usare l'edge port come indicatore del priority e sono a posto. 
struct Scheduler_Slot {
    Priority pr;
    int id;
};

struct Port {
    bool is_master;
    int id;
    int associated_port_id = NO_PORT; //no associated port
    int priority = DEFAULT_PRIORITY; // default no priority
};

class First_Level_Vertex : public Custom_Vertex{
public:    
    void explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) const;
    
};

class Second_Level_Vertex : public Custom_Vertex{
public: 
    void explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) const ;
};

class Third_Level_Vertex : public Custom_Vertex{
public:    
    std::map <int, Scheduler_Slot> priority_slots; //serve ???? uno slot per ogni task, e all interno dello slot è segnato il livello di priorita. boh...
    Component_Priority_Category OS_scheduler_type;
    void explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) const ;
};

class Fourth_Level_Vertex : public Custom_Vertex{
public: 
    std::map <int, Port> ports_map;
    Component_Priority_Category component_priority_type;
    Component_Type component_type;
    void explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) const ;
};

class Fifth_Level_Vertex : public Custom_Vertex{
public: 
    Fifth_Level_Vertex () {layer = Layer::PHYSICAL;}
    void explode_component_timing(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) const ;
};





typedef boost::graph_traits<Source_Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Source_Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Source_Graph>::vertex_iterator vertex_iter;
typedef boost::graph_traits<Source_Graph>::out_edge_iterator ext_out_edge_iter;
typedef boost::graph_traits<Source_Graph>::edge_iterator edge_iter;

typedef boost::graph_traits<Timing_Graph>::vertex_descriptor timing_vertex_t;
typedef boost::graph_traits<Timing_Graph>::edge_descriptor timing_edge_t;
typedef boost::graph_traits<Timing_Graph>::vertex_iterator timing_vertex_iter;
typedef boost::graph_traits<Timing_Graph>::edge_iterator timing_edge_iter;


template <class Layer_Map,class Name_Map>
class vertex_writer {
public:
  vertex_writer(Layer_Map l, Name_Map n) : lm(l),nm(n){}
  template <class Vertex>
  void operator()(std::ostream &out, const Vertex& v) const {
    PRINT_DEBUG("printing graph: node is: "+nm[v]);
    out << "[label=< <FONT POINT-SIZE=\"20\">"<<nm[v]<<"</FONT><br/> >";
    switch(lm[v]){
      case FUNCTION:
      {
	out<<",style=filled, fillcolor= yellow";
	break;
      }
      case TASK:
      {
	out<<",style=filled, fillcolor= red";
	break;
      }
      case CONTROLLER:
      {
	out<<",style=filled, fillcolor= green";
	break;
      }
      case RESOURCE:
      {
	out<<",style=filled, fillcolor= cyan";
	break;
      }
      case PHYSICAL:
      {break;}
      default:
      {
	PRINT_DEBUG("ERROR: lm[e] not a layer type ");
	throw std::runtime_error(boost::lexical_cast<std::string>("ERROR: lm[e] not a layer "));
      }
    }
    out << "]";
    //ports missing
  }
private:
  Layer_Map lm;
  Name_Map nm;
};

template <class Layer_Map,class Name_Map>
inline vertex_writer<Layer_Map, Name_Map> 
make_vertex_writer(Layer_Map l,Name_Map n) {
  return vertex_writer<Layer_Map, Name_Map>(l,n);
}


template <class From_Port_Map,class To_Port_Map>
class edge_writer {
public:
  edge_writer(From_Port_Map n, To_Port_Map p_m) : fpm(n),tpm(p_m) {}
  template <class Edge>
  void operator()(std::ostream &out, const Edge& e) const {
    PRINT_DEBUG("printing graph: edge is ("+boost::lexical_cast<std::string>(fpm[e])+","+boost::lexical_cast<std::string>(tpm[e])+") ");
    
    out <<  "["  <<((fpm[e] != -1) ? "taillabel=\"" +boost::lexical_cast<std::string>( fpm[e])+"\",": "") << (tpm[e] != -1 ?" headlabel=\""+  boost::lexical_cast<std::string>(tpm[e]) +"\"": "") <<"]";
    
  }
private:
  From_Port_Map fpm;
  To_Port_Map tpm;
};

template <class From_Port_Map,class To_Port_Map>
inline edge_writer<From_Port_Map, To_Port_Map> 
make_edge_writer(From_Port_Map n, To_Port_Map p) {
  return edge_writer<From_Port_Map, To_Port_Map>(n,p);
}

#endif
