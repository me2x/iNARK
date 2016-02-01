#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include "debug_msg_handler.hpp"
#include <boost/graph/graphviz.hpp>
#include <boost/graph/undirected_dfs.hpp>

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




class Custom_Vertex
{
public:
    std::string name;
    
    virtual void explode_component(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) const = 0; //<old_comp.id <old_comp.port new_comp.id> >
};

class Custom_Edge
{
public:
    int to_port;
    int from_port;
};

// non ha sensooooo: tanto i tdma sono vertex.incoming_edges(), i priority sono PRIORITY_ENUM_SIZE, i RR sono 1.
// basta usare l'edge port come indicatore del priority e sono a posto. 
class Scheduler_Slot {
    Priority pr;
    int id;
};

class Port {
    bool is_master;
    int id;
    int associated_port_id = NO_PORT; //no associated port
    int priority = DEFAULT_PRIORITY; // default no priority
};

class First_Level_Vertex : Custom_Vertex{
public: 
    void explode_component(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) ;
};

class Second_Level_Vertex : Custom_Vertex{
public: 
    void explode_component(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) ;
};

class Third_Level_Vertex : Custom_Vertex{
public:    
    std::map <int, Scheduler_Slot> priority_slots; //serve ???? uno slot per ogni task, e all interno dello slot è segnato il livello di priorita. boh...
    Component_Priority_Category OS_scheduler_type;
    void explode_component(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) ;
};

class Fourth_Level_Vertex : Custom_Vertex{
public: 
    std::map <int, Port> ports_map;
    Component_Priority_Category component_priority_type;
    Component_Type component_type;
    void explode_component(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) ;
};

class Fifth_Level_Vertex : Custom_Vertex{
public: 
    void explode_component(Timing_Graph& graph, std::map<std::string, std::map< int, std::string> >& components_map ) ;
};

class Timing_Node{// aggiungere sottoclasse 4thlvlTimingNode con associate_port_name,master_tasks e is_master rimuovendoli da qua. forse meglio
public:
    std::string name;
    Layer layer; //serve reitrodurlo per fermare la ricerca al terzo o quarto o quinto livello e per togliere edges 2 to 3 in 2nd phase of preparation: task propagation 
};

class Fourth_Level_Timing_Node : Timing_Node {
    std::string associate_port_name; //default vuota. serve solo al 4th livello per le porte master&slave. NB non sono la slave che diventa master da un componente all altro ma la doppia porta che garantisce bidirezionalita
    std::vector<std::string> master_tasks; //vettore che viene riempito in task propagation
    bool is_master; //tiene traccia se un nodo rappresenta una porta master o una slave. ha senso solo al 4th lvl.
};


typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,Custom_Vertex,Custom_Edge>Source_Graph;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,boost::property<boost::vertex_color_t, boost::default_color_type, Timing_Node> > Timing_Graph; //edge custom non serve piu.
typedef boost::graph_traits<Source_Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Source_Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Source_Graph>::vertex_iterator vertex_iter;
typedef boost::graph_traits<Source_Graph>::out_edge_iterator ext_out_edge_iter;
typedef boost::graph_traits<Source_Graph>::edge_iterator edge_iter;

typedef boost::graph_traits<Timing_Graph>::vertex_descriptor inner_vertex_t;
typedef boost::graph_traits<Timing_Graph>::edge_descriptor inner_edge_t;
typedef boost::graph_traits<Timing_Graph>::vertex_iterator inner_vertex_iter;
typedef boost::graph_traits<Timing_Graph>::edge_iterator inner_edge_iter;


template <class Layer_Map,class Name_Map,class Ports_Map, class Type_Map, class Priority_Type_Map>
class vertex_writer {
public:
  vertex_writer(Layer_Map l, Name_Map n, Ports_Map p_m, Type_Map t, Priority_Type_Map p) : lm(l),nm(n),pm(p_m), tm(t),ptm(p) {}
  template <class Vertex>
  void operator()(std::ostream &out, const Vertex& v) const {
    PRINT_DEBUG("printing graph: node is: "+nm[v]+" and its type is: "+boost::lexical_cast<std::string>(tm[v])+" and its priority type is: "+boost::lexical_cast<std::string>(ptm[v]));
    out << "[label=< <FONT POINT-SIZE=\"20\">"<<nm[v]<<"</FONT><br/>"<<tm[v]<<"<br/><FONT POINT-SIZE=\"10\">"<<ptm[v]<<"</FONT> >";
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
  Ports_Map pm;
  Type_Map tm;
  Priority_Type_Map ptm;
};

template <class Layer_Map,class Name_Map,class Ports_Map, class Type_Map, class Priority_Type_Map>
inline vertex_writer<Layer_Map, Name_Map, Ports_Map,  Type_Map,  Priority_Type_Map> 
make_vertex_writer(Layer_Map l,Name_Map n,Ports_Map p, Type_Map t, Priority_Type_Map pt) {
  return vertex_writer<Layer_Map, Name_Map, Ports_Map, Type_Map, Priority_Type_Map>(l,n,p,t,pt);
}


template <class From_Port_Map,class To_Port_Map>
class edge_writer {
public:
  edge_writer(From_Port_Map n, To_Port_Map p_m) : fpm(n),tpm(p_m) {}
  template <class Edge>
  void operator()(std::ostream &out, const Edge& e) const {
    PRINT_DEBUG("printing graph: edge is ("+boost::lexical_cast<std::string>(fpm[e])+","+boost::lexical_cast<std::string>(tpm[e])+") ");
    out <<  "[taillabel=\"" << fpm[e] << "\", headlabel=\""<<tpm[e]<<"\"]";
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
